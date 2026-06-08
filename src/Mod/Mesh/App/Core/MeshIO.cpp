/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#ifndef _PreComp_
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string_view>
#endif

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "IO/Reader3MF.h"
#include "IO/ReaderNAS.h"
#include "IO/ReaderOBJ.h"
#include "IO/ReaderOFF.h"
#include "IO/ReaderPLY.h"
#include "IO/ReaderSMF.h"
#include "IO/ReaderSTL.h"
#include "IO/Writer3MF.h"
#include "IO/WriterInventor.h"
#include "IO/WriterOBJ.h"
#include "IO/WriterPLY.h"
#include "IO/WriterSTL.h"
#include "IO/WriterVRML.h"
#include <Base/Builder3D.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Placement.h>
#include <Base/Reader.h>
#include <Base/Sequencer.h>
#include <Base/Stream.h>
#include <Base/Tools.h>
#include <Base/Writer.h>
#include <zipios++/gzipoutputstream.h>
#include <zipios++/zipoutputstream.h>

#include "Builder.h"
#include "Definitions.h"
#include "Degeneration.h"
#include "Iterator.h"
#include "MeshIO.h"
#include "MeshKernel.h"


namespace sp = std::placeholders;
using namespace MeshCore;

namespace MeshCore
{

// This is a workaround for the issue described at:
// https://github.com/Zipios/Zipios/issues/43#issue-1618151314
//
// The workaround creates a tmp. ZIP file and uses the Python API
// to open the file zipios++ isn't able to handle and to copy over
// the files.
class ZipFixer
{
public:
    explicit ZipFixer(const char* filename)
        : tmp {Base::FileInfo::getTempFileName()}
    {
        Base::ZipTools::rewrite(filename, tmp.filePath());
        str.open(tmp, std::ios::in | std::ios::binary);
    }

    ~ZipFixer()
    {
        tmp.deleteFile();
    }

    Base::ifstream& getStream()
    {
        return str;
    }

private:
    Base::FileInfo tmp;
    Base::ifstream str;
};

}  // namespace MeshCore

// --------------------------------------------------------------

bool Material::operator==(const Material& mat) const
{
    if (binding != mat.binding) {
        return false;
    }
    if (ambientColor != mat.ambientColor) {
        return false;
    }
    if (diffuseColor != mat.diffuseColor) {
        return false;
    }
    if (specularColor != mat.specularColor) {
        return false;
    }
    if (emissiveColor != mat.emissiveColor) {
        return false;
    }
    if (shininess != mat.shininess) {
        return false;
    }
    if (transparency != mat.transparency) {
        return false;
    }
    return true;
}

bool Material::operator!=(const Material& mat) const
{
    return !operator==(mat);
}

// --------------------------------------------------------------

// clang-format off
static constexpr auto numInput {11};
using InputItem = std::pair<std::string, MeshIO::Format>;
static std::array<InputItem, numInput> inputFormats {{
    {"bms", MeshIO::Format::BMS},
    {"ply", MeshIO::Format::PLY},
    {"stl", MeshIO::Format::STL},
    {"ast", MeshIO::Format::ASTL},
    {"obj", MeshIO::Format::OBJ},
    {"nas", MeshIO::Format::NAS},
    {"bdf", MeshIO::Format::NAS},
    {"off", MeshIO::Format::OFF},
    {"smf", MeshIO::Format::SMF},
    {"3mf", MeshIO::Format::ThreeMF},
    {"iv", MeshIO::Format::IV},
}};
// clang-format on

std::vector<std::string> MeshInput::supportedMeshFormats()
{
    std::vector<std::string> fmt;
    fmt.reserve(inputFormats.size());
    std::transform(inputFormats.cbegin(), inputFormats.cend(),
                   std::back_inserter(fmt), [](const InputItem& item) {
        return item.first;
    });
    return fmt;
}

MeshIO::Format MeshInput::getFormat(const char* FileName)
{
    Base::FileInfo fi(FileName);
    auto it = std::find_if(inputFormats.begin(), inputFormats.end(),
                           [&fi](const InputItem& item) {
        return fi.hasExtension(item.first.c_str());
    });

    if (it != inputFormats.end()) {
        return it->second;
    }

    throw Base::FileException("File extension not supported", FileName);
}

bool MeshInput::LoadAny(const char* FileName)
{
    // clang-format off
    // NOLINTBEGIN
    using InputFunc = std::pair<std::string, std::function<bool(std::istream&)>>;
    std::array<InputFunc, numInput> inputFuncs {{
        {"bms", std::bind(&MeshInput::LoadBMS,      this, sp::_1)},
        {"ply", std::bind(&MeshInput::LoadPLY,      this, sp::_1)},
        {"stl", std::bind(&MeshInput::LoadSTL,      this, sp::_1)},
        {"ast", std::bind(&MeshInput::LoadSTL,      this, sp::_1)},
        {"nas", std::bind(&MeshInput::LoadNastran,  this, sp::_1)},
        {"bdf", std::bind(&MeshInput::LoadNastran,  this, sp::_1)},
        {"off", std::bind(&MeshInput::LoadOFF,      this, sp::_1)},
        {"smf", std::bind(&MeshInput::LoadSMF,      this, sp::_1)},
        {"iv",  std::bind(&MeshInput::LoadInventor, this, sp::_1)},
        {"3mf", [this, FileName](std::istream& input) {
            try {
                return Load3MF(input);
            }
            catch (const zipios::FCollException&) {
                ZipFixer zip(FileName);
                return Load3MF(zip.getStream());
            }
        }},
        {"obj", [this, FileName](std::istream& input) {
            return LoadOBJ(input, FileName);
        }},
    }};
    // NOLINTEND
    // clang-format on

    // ask for read permission
    Base::FileInfo fi(FileName);
    if (!fi.exists() || !fi.isFile()) {
        throw Base::FileException("File does not exist", FileName);
    }
    if (!fi.isReadable()) {
        throw Base::FileException("No permission on the file", FileName);
    }

    auto it = std::find_if(inputFuncs.begin(), inputFuncs.end(),
                           [&fi](const InputFunc& item) {
        return fi.hasExtension(item.first.c_str());
    });

    if (it != inputFuncs.end()) {
        Base::ifstream str(fi, std::ios::in | std::ios::binary);
        return it->second(str);
    }

    throw Base::FileException("File extension not supported", FileName);
}

bool MeshInput::LoadFormat(std::istream& input, MeshIO::Format fmt)
{
    // clang-format off
    // NOLINTBEGIN
    static constexpr auto numInput {12};
    using InputFunc = std::pair<MeshIO::Format, std::function<bool(std::istream&)>>;
    std::array<InputFunc, numInput> inputFuncs {{
        {MeshIO::BMS,  std::bind(&MeshInput::LoadBMS,       this, sp::_1)},
        {MeshIO::PLY,  std::bind(&MeshInput::LoadPLY,       this, sp::_1)},
        {MeshIO::APLY, std::bind(&MeshInput::LoadPLY,       this, sp::_1)},
        {MeshIO::STL,  std::bind(&MeshInput::LoadSTL,       this, sp::_1)},
        {MeshIO::ASTL, std::bind(&MeshInput::LoadAsciiSTL,  this, sp::_1)},
        {MeshIO::BSTL, std::bind(&MeshInput::LoadBinarySTL, this, sp::_1)},
        {MeshIO::NAS,  std::bind(&MeshInput::LoadNastran,   this, sp::_1)},
        {MeshIO::OFF,  std::bind(&MeshInput::LoadOFF,       this, sp::_1)},
        {MeshIO::SMF,  std::bind(&MeshInput::LoadSMF,       this, sp::_1)},
        {MeshIO::ThreeMF, std::bind(&MeshInput::Load3MF,    this, sp::_1)},
        {MeshIO::IV ,  std::bind(&MeshInput::LoadInventor,  this, sp::_1)},
        {MeshIO::OBJ, [this](std::istream& input) {
            return LoadOBJ(input);
        }},
    }};
    // NOLINTEND
    // clang-format on

    auto it = std::find_if(inputFuncs.begin(), inputFuncs.end(),
                           [fmt](const InputFunc& item) {
        return (item.first == fmt);
    });

    if (it != inputFuncs.end()) {
        return it->second(input);
    }

    throw Base::FileException("Unsupported file format");
}

bool MeshInput::LoadBMS(std::istream& input)
{
    _rclMesh.Read(input);
    return true;
}

/** Loads an STL file either in binary or ASCII format.
 * Therefore the file header gets checked to decide if the file is binary or not.
 */
bool MeshInput::LoadSTL(std::istream& input)
{
    ReaderSTL reader(_rclMesh);
    return reader.Load(input);
}

/** Loads an ASCII STL file. */
bool MeshInput::LoadAsciiSTL(std::istream& input)
{
    ReaderSTL reader(_rclMesh);
    return reader.LoadAscii(input);
}

/** Loads a binary STL file. */
bool MeshInput::LoadBinarySTL(std::istream& input)
{
    ReaderSTL reader(_rclMesh);
    return reader.LoadBinary(input);
}

/** Loads an OBJ file. */
bool MeshInput::LoadOBJ(std::istream& input)
{
    ReaderOBJ reader(this->_rclMesh, this->_material);
    if (reader.Load(input)) {
        _groupNames = reader.GetGroupNames();
        return true;
    }

    return false;
}

bool MeshInput::LoadOBJ(std::istream& input, const char* filename)
{
    ReaderOBJ reader(this->_rclMesh, this->_material);
    if (reader.Load(input)) {
        _groupNames = reader.GetGroupNames();
        if (this->_material && this->_material->binding == MeshCore::MeshIO::PER_FACE) {
            Base::FileInfo fi(filename);
            std::string fn = fi.dirPath() + "/" + this->_material->library;
            fi.setFile(fn);
            Base::ifstream mtl(fi, std::ios::in | std::ios::binary);
            reader.LoadMaterial(mtl);
            mtl.close();
        }

        return true;
    }

    return false;
}

/** Loads an SMF file. */
bool MeshInput::LoadSMF(std::istream& input)
{
    ReaderSMF reader(_rclMesh);
    return reader.Load(input);
}

/** Loads an OFF file. */
bool MeshInput::LoadOFF(std::istream& input)
{
    ReaderOFF reader(_rclMesh);
    return reader.Load(input);
}

bool MeshInput::LoadPLY(std::istream& input)
{
    ReaderPLY reader(this->_rclMesh, this->_material);
    return reader.Load(input);
}

bool MeshInput::LoadMeshNode(std::istream& input)
{
    boost::regex rx_p("^v\\s+([-+]?[0-9]*)\\.?([0-9]+([eE][-+]?[0-9]+)?)"
                      "\\s+([-+]?[0-9]*)\\.?([0-9]+([eE][-+]?[0-9]+)?)"
                      "\\s+([-+]?[0-9]*)\\.?([0-9]+([eE][-+]?[0-9]+)?)\\s*$");
    boost::regex rx_f(R"(^f\s+([0-9]+)\s+([0-9]+)\s+([0-9]+)\s*$)");
    boost::regex rx_e("\\s*]\\s*");
    boost::cmatch what;

    MeshPointArray meshPoints;
    MeshFacetArray meshFacets;

    std::string line;
    float fX {}, fY {}, fZ {};
    unsigned int i1 = 1, i2 = 1, i3 = 1;
    MeshGeomFacet clFacet;

    if (!input || input.bad()) {
        return false;
    }

    std::streambuf* buf = input.rdbuf();
    if (!buf) {
        return false;
    }

    while (std::getline(input, line)) {
        boost::algorithm::to_lower(line);
        if (boost::regex_match(line.c_str(), what, rx_p)) {
            fX = (float)std::atof(what[1].first);
            fY = (float)std::atof(what[4].first);
            fZ = (float)std::atof(what[7].first);
            meshPoints.push_back(MeshPoint(Base::Vector3f(fX, fY, fZ)));
        }
        else if (boost::regex_match(line.c_str(), what, rx_f)) {
            i1 = std::atoi(what[1].first);
            i2 = std::atoi(what[2].first);
            i3 = std::atoi(what[3].first);
            meshFacets.push_back(MeshFacet(i1 - 1, i2 - 1, i3 - 1));
        }
        else if (boost::regex_match(line.c_str(), what, rx_e)) {
            break;
        }
    }

    this->_rclMesh.Clear();  // remove all data before

    MeshCleanup meshCleanup(meshPoints, meshFacets);
    meshCleanup.RemoveInvalids();
    MeshPointFacetAdjacency meshAdj(meshPoints.size(), meshFacets);
    meshAdj.SetFacetNeighbourhood();
    this->_rclMesh.Adopt(meshPoints, meshFacets);

    return true;
}

/** Loads the mesh object from an XML file. */
void MeshInput::LoadXML(Base::XMLReader& reader)
{
    MeshPointArray cPoints;
    MeshFacetArray cFacets;

    //  reader.readElement("Mesh");

    reader.readElement("Points");
    int Cnt = reader.getAttributeAsInteger("Count");

    cPoints.resize(Cnt);
    for (int i = 0; i < Cnt; i++) {
        reader.readElement("P");
        cPoints[i].x = (float)reader.getAttributeAsFloat("x");
        cPoints[i].y = (float)reader.getAttributeAsFloat("y");
        cPoints[i].z = (float)reader.getAttributeAsFloat("z");
    }
    reader.readEndElement("Points");

    reader.readElement("Faces");
    Cnt = reader.getAttributeAsInteger("Count");

    cFacets.resize(Cnt);
    for (int i = 0; i < Cnt; i++) {
        reader.readElement("F");
        cFacets[i]._aulPoints[0] = reader.getAttributeAsInteger("p0");
        cFacets[i]._aulPoints[1] = reader.getAttributeAsInteger("p1");
        cFacets[i]._aulPoints[2] = reader.getAttributeAsInteger("p2");
        cFacets[i]._aulNeighbours[0] = reader.getAttributeAsInteger("n0");
        cFacets[i]._aulNeighbours[1] = reader.getAttributeAsInteger("n1");
        cFacets[i]._aulNeighbours[2] = reader.getAttributeAsInteger("n2");
    }

    reader.readEndElement("Faces");
    reader.readEndElement("Mesh");

    _rclMesh.Adopt(cPoints, cFacets);
}

/** Loads a 3MF file. */
bool MeshInput::Load3MF(std::istream& input)
{
    Reader3MF reader(input);
    reader.Load();
    std::vector<int> ids = reader.GetMeshIds();
    if (!ids.empty()) {
        const int topLevel = ids[0];
        MeshKernel compound = reader.GetMesh(topLevel);
        compound.Transform(reader.GetTransform(topLevel));
        const std::string name = reader.GetName(topLevel);
        if (!name.empty()) {
            _objectName = name;
        }

        for (std::size_t index = 1; index < ids.size(); index++) {
            MeshKernel mesh = reader.GetMesh(ids[index]);
            mesh.Transform(reader.GetTransform(ids[index]));
            compound.Merge(mesh);
        }

        _rclMesh = compound;
        return true;
    }

    return false;
}

/** Loads an OpenInventor file. */
bool MeshInput::LoadInventor(std::istream& input)
{
    Base::InventorLoader loader(input);
    if (!loader.read()) {
        return false;
    }

    if (!loader.isValid()) {
        return false;
    }

    const auto& points = loader.getPoints();
    const auto& faces = loader.getFaces();

    MeshPointArray meshPoints;
    meshPoints.reserve(points.size());
    std::transform(points.begin(),
                   points.end(),
                   std::back_inserter(meshPoints),
                   [](const Base::Vector3f& v) {
                       return MeshPoint(v);
                   });

    MeshFacetArray meshFacets;
    meshFacets.reserve(faces.size());
    std::transform(faces.begin(),
                   faces.end(),
                   std::back_inserter(meshFacets),
                   [](const Base::InventorLoader::Face& f) {
                       return MeshFacet(f.p1, f.p2, f.p3);
                   });

    MeshCleanup meshCleanup(meshPoints, meshFacets);
    meshCleanup.RemoveInvalids();
    MeshPointFacetAdjacency meshAdj(meshPoints.size(), meshFacets);
    meshAdj.SetFacetNeighbourhood();
    this->_rclMesh.Adopt(meshPoints, meshFacets);

    if (loader.isNonIndexed()) {
        if (!MeshEvalDuplicatePoints(this->_rclMesh).Evaluate()) {
            MeshFixDuplicatePoints(this->_rclMesh).Fixup();
        }
    }

    return true;
}

/** Loads a Nastran file. */
bool MeshInput::LoadNastran(std::istream& input)
{
    ReaderNAS reader(_rclMesh);
    return reader.Load(input);
}

// --------------------------------------------------------------

// clang-format off
static constexpr auto numOutput {22};
using OutputItem = std::pair<std::string, MeshIO::Format>;
static std::array<OutputItem, numOutput> outputFormats {{
    {"bms", MeshIO::Format::BMS},
    {"ply", MeshIO::Format::PLY},
    {"stl", MeshIO::Format::BSTL},
    {"ast", MeshIO::Format::ASTL},
    {"obj", MeshIO::Format::OBJ},
    {"nas", MeshIO::Format::NAS},
    {"bdf", MeshIO::Format::NAS},
    {"off", MeshIO::Format::OFF},
    {"smf", MeshIO::Format::SMF},
    {"3mf", MeshIO::Format::ThreeMF},
    {"x3d", MeshIO::Format::X3D},
    {"x3dz", MeshIO::Format::X3DZ},
    {"xhtml", MeshIO::Format::X3DOM},
    {"vrml", MeshIO::Format::VRML},
    {"wrl", MeshIO::Format::VRML},
    {"wrz", MeshIO::Format::WRZ},
    {"amf", MeshIO::Format::AMF},
    {"asy", MeshIO::Format::ASY},
    {"idtf", MeshIO::Format::IDTF},
    {"mgl", MeshIO::Format::MGL},
    {"iv", MeshIO::Format::IV},
    {"py", MeshIO::Format::PY},
}};
// clang-format on

std::string MeshOutput::stl_header = "MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-"
                                     "MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH\n";

void MeshOutput::SetSTLHeaderData(const std::string& header)
{
    if (header.size() > 80) {
        stl_header = header.substr(0, 80);
    }
    else if (header.size() < 80) {
        std::fill(stl_header.begin(), stl_header.end(), ' ');
        std::copy(header.begin(), header.end(), stl_header.begin());
    }
    else {
        stl_header = header;
    }
}

std::string MeshOutput::asyWidth = "500";
std::string MeshOutput::asyHeight = "500";

void MeshOutput::SetAsymptoteSize(const std::string& w, const std::string& h)
{
    asyWidth = w;
    asyHeight = h;
}

void MeshOutput::Transform(const Base::Matrix4D& mat)
{
    _transform = mat;
    if (mat != Base::Matrix4D()) {
        apply_transform = true;
    }
}

std::vector<std::string> MeshOutput::supportedMeshFormats()
{
    std::vector<std::string> fmt;
    fmt.reserve(outputFormats.size());
    std::transform(outputFormats.cbegin(), outputFormats.cend(),
                   std::back_inserter(fmt), [](const OutputItem& item) {
        return item.first;
    });
    return fmt;
}

MeshIO::Format MeshOutput::GetFormat(const char* FileName)
{
    Base::FileInfo fi(FileName);
    auto it = std::find_if(outputFormats.begin(), outputFormats.end(),
                           [&fi](const OutputItem& item) {
        return fi.hasExtension(item.first.c_str());
    });

    if (it != outputFormats.end()) {
        return it->second;
    }

    return MeshIO::Undefined;
}

/// Save in a file, format is decided by the extension if not explicitly given
bool MeshOutput::SaveAny(const char* FileName, MeshIO::Format format) const
{
    // ask for write permission
    Base::FileInfo file(FileName);
    Base::FileInfo directory(file.dirPath());
    if ((file.exists() && !file.isWritable()) || !directory.exists() || !directory.isWritable()) {
        throw Base::FileException("No write permission for file", FileName);
    }

    MeshIO::Format fileformat = format;
    if (fileformat == MeshIO::Undefined) {
        fileformat = GetFormat(FileName);
    }

    Base::ofstream str(file, std::ios::out | std::ios::binary);

    if (fileformat == MeshIO::BMS) {
        SaveBMS(str);
    }
    else if (fileformat == MeshIO::BSTL) {
        MeshOutput aWriter(_rclMesh);
        aWriter.Transform(this->_transform);

        // write file
        bool ok = false;
        ok = aWriter.SaveBinarySTL(str);
        if (!ok) {
            throw Base::FileException("Export of STL mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::ASTL) {
        MeshOutput aWriter(_rclMesh);
        aWriter.SetObjectName(objectName);
        aWriter.Transform(this->_transform);

        // write file
        bool ok = false;
        ok = aWriter.SaveAsciiSTL(str);
        if (!ok) {
            throw Base::FileException("Export of STL mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::OBJ) {
        // write file
        if (!SaveOBJ(str, FileName)) {
            throw Base::FileException("Export of OBJ mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::SMF) {
        // write file
        if (!SaveSMF(str)) {
            throw Base::FileException("Export of SMF mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::OFF) {
        // write file
        if (!SaveOFF(str)) {
            throw Base::FileException("Export of OFF mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::PLY) {
        // write file
        if (!SaveBinaryPLY(str)) {
            throw Base::FileException("Export of PLY mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::APLY) {
        // write file
        if (!SaveAsciiPLY(str)) {
            throw Base::FileException("Export of PLY mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::IDTF) {
        // write file
        if (!SaveIDTF(str)) {
            throw Base::FileException("Export of IDTF mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::MGL) {
        // write file
        if (!SaveMGL(str)) {
            throw Base::FileException("Export of MGL mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::IV) {
        // write file
        if (!SaveInventor(str)) {
            throw Base::FileException("Export of Inventor mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::X3D) {
        // write file
        if (!SaveX3D(str)) {
            throw Base::FileException("Export of X3D failed", FileName);
        }
    }
    else if (fileformat == MeshIO::X3DZ) {
        // Compressed X3D is nothing else than a GZIP'ped X3D ascii file
        zipios::GZIPOutputStream gzip(str);
        // write file
        if (!SaveX3D(gzip)) {
            throw Base::FileException("Export of compressed X3D mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::X3DOM) {
        // write file
        if (!SaveX3DOM(str)) {
            throw Base::FileException("Export of X3DOM failed", FileName);
        }
    }
    else if (fileformat == MeshIO::ThreeMF) {
        // write file
        if (!Save3MF(str)) {
            throw Base::FileException("Export of 3MF failed", FileName);
        }
    }
    else if (fileformat == MeshIO::PY) {
        // write file
        if (!SavePython(str)) {
            throw Base::FileException("Export of Python mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::VRML) {
        // write file
        if (!SaveVRML(str)) {
            throw Base::FileException("Export of VRML mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::WRZ) {
        // Compressed VRML is nothing else than a GZIP'ped VRML ascii file
        // str.close();
        // Base::ogzstream gzip(FileName, std::ios::out | std::ios::binary);
        // Hint: The compression level seems to be higher than with ogzstream
        // which leads to problems to load the wrz file in debug mode, the
        // application simply crashes.
        zipios::GZIPOutputStream gzip(str);
        // write file
        if (!SaveVRML(gzip)) {
            throw Base::FileException("Export of compressed VRML mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::NAS) {
        // write file
        if (!SaveNastran(str)) {
            throw Base::FileException("Export of NASTRAN mesh failed", FileName);
        }
    }
    else if (fileformat == MeshIO::ASY) {
        // write file
        if (!SaveAsymptote(str)) {
            throw Base::FileException("Export of ASY mesh failed", FileName);
        }
    }
    else {
        throw Base::FileException("File format not supported", FileName);
    }

    return true;
}

bool MeshOutput::SaveFormat(std::ostream& str, MeshIO::Format fmt) const
{
    // clang-format off
    // NOLINTBEGIN
    static constexpr auto numOutput {19};
    using InputFunc = std::pair<MeshIO::Format, std::function<bool(std::ostream&)>>;
    std::array<InputFunc, numOutput> outputFuncs {{
        {MeshIO::BMS,  std::bind(&MeshOutput::SaveBMS,       this, sp::_1)},
        {MeshIO::ASTL, std::bind(&MeshOutput::SaveAsciiSTL,  this, sp::_1)},
        {MeshIO::BSTL, std::bind(&MeshOutput::SaveBinarySTL, this, sp::_1)},
        {MeshIO::SMF,  std::bind(&MeshOutput::SaveSMF,       this, sp::_1)},
        {MeshIO::OFF,  std::bind(&MeshOutput::SaveOFF,       this, sp::_1)},
        {MeshIO::IDTF, std::bind(&MeshOutput::SaveIDTF,      this, sp::_1)},
        {MeshIO::MGL,  std::bind(&MeshOutput::SaveMGL,       this, sp::_1)},
        {MeshIO::IV ,  std::bind(&MeshOutput::SaveInventor,  this, sp::_1)},
        {MeshIO::X3D,  std::bind(&MeshOutput::SaveX3D,       this, sp::_1)},
        {MeshIO::X3DOM, std::bind(&MeshOutput::SaveX3DOM,    this, sp::_1)},
        {MeshIO::VRML, std::bind(&MeshOutput::SaveVRML,      this, sp::_1)},
        // it's up to the client to create the needed stream
        {MeshIO::WRZ,  std::bind(&MeshOutput::SaveVRML,      this, sp::_1)},
        {MeshIO::ThreeMF, std::bind(&MeshOutput::Save3MF,    this, sp::_1)},
        {MeshIO::NAS,  std::bind(&MeshOutput::SaveNastran,   this, sp::_1)},
        {MeshIO::PLY,  std::bind(&MeshOutput::SaveBinaryPLY, this, sp::_1)},
        {MeshIO::APLY, std::bind(&MeshOutput::SaveAsciiPLY,  this, sp::_1)},
        {MeshIO::PY,   std::bind(&MeshOutput::SavePython,    this, sp::_1)},
        {MeshIO::ASY,  std::bind(&MeshOutput::SaveAsymptote, this, sp::_1)},
        {MeshIO::OBJ, [this](std::ostream& str) {
            return SaveOBJ(str);
        }},
    }};
    // NOLINTEND
    // clang-format on

    auto it = std::find_if(outputFuncs.begin(), outputFuncs.end(),
                           [fmt](const InputFunc& item) {
        return (item.first == fmt);
    });

    if (it != outputFuncs.end()) {
        return it->second(str);
    }

    throw Base::FileException("Unsupported file format");
}

/** Saves a BMS file. */
bool MeshOutput::SaveBMS(std::ostream& output) const
{
    _rclMesh.Write(output);
    return true;
}

/** Saves the mesh object into an ASCII file. */
bool MeshOutput::SaveAsciiSTL(std::ostream& output) const
{
    WriterSTL writer(this->_rclMesh);
    writer.SetTransform(this->_transform);
    writer.SetObjectName(this->objectName);
    return writer.SaveAscii(output);
}

/** Saves the mesh object into a binary file. */
bool MeshOutput::SaveBinarySTL(std::ostream& output) const
{
    WriterSTL writer(this->_rclMesh);
    writer.SetTransform(this->_transform);
    writer.SetHeaderData(stl_header);
    return writer.SaveBinary(output);
}

/** Saves an OBJ file. */
bool MeshOutput::SaveOBJ(std::ostream& out) const
{
    WriterOBJ writer(this->_rclMesh, this->_material);
    writer.SetTransform(this->_transform);
    writer.SetGroups(this->_groups);
    return writer.Save(out);
}

bool MeshOutput::SaveOBJ(std::ostream& out, const char* filename) const
{
    WriterOBJ writer(this->_rclMesh, this->_material);
    writer.SetTransform(this->_transform);
    writer.SetGroups(this->_groups);
    if (writer.Save(out)) {
        if (this->_material && this->_material->binding == MeshCore::MeshIO::PER_FACE) {
            Base::FileInfo fi(filename);
            std::string fn = fi.dirPath() + "/" + this->_material->library;
            fi.setFile(fn);
            Base::ofstream mtl(fi, std::ios::out | std::ios::binary);
            writer.SaveMaterial(mtl);
            mtl.close();
        }

        return true;
    }

    return false;
}

/** Saves an SMF file. */
bool MeshOutput::SaveSMF(std::ostream& out) const
{
    // http://people.sc.fsu.edu/~jburkardt/data/smf/smf.txt
    const MeshPointArray& rPoints = _rclMesh.GetPoints();
    const MeshFacetArray& rFacets = _rclMesh.GetFacets();

    if (!out || out.bad()) {
        return false;
    }

    Base::SequencerLauncher seq("saving...", _rclMesh.CountPoints() + _rclMesh.CountFacets());

    // Header
    out << "#$SMF 1.0\n";
    out << "#$vertices " << rPoints.size() << '\n';
    out << "#$faces " << rFacets.size() << '\n';
    out << "#\n";
    out << "# Created by FreeCAD <https://www.freecad.org>\n";

    out.precision(6);
    out.setf(std::ios::fixed | std::ios::showpoint);

    // vertices
    Base::Vector3f pt;
    std::size_t index = 0;
    for (auto it = rPoints.begin(); it != rPoints.end(); ++it, ++index) {
        if (this->apply_transform) {
            pt = this->_transform * *it;
        }
        else {
            pt.Set(it->x, it->y, it->z);
        }

        out << "v " << pt.x << " " << pt.y << " " << pt.z << '\n';
        seq.next(true);  // allow one to cancel
    }

    // facet indices
    for (const auto& it : rFacets) {
        out << "f " << it._aulPoints[0] + 1 << " " << it._aulPoints[1] + 1 << " "
            << it._aulPoints[2] + 1 << '\n';
        seq.next(true);  // allow one to cancel
    }

    return true;
}

/** Saves an Asymptote file. */
bool MeshOutput::SaveAsymptote(std::ostream& out) const
{
    out << "/*\n"
           " * Created by FreeCAD <https://www.freecad.org>\n"
           " */\n\n";

    out << "import three;\n\n";

    if (!asyWidth.empty()) {
        out << "size(" << asyWidth;
        if (!asyHeight.empty()) {
            out << ", " << asyHeight;
        }
        out << ");\n\n";
    }

    Base::BoundBox3f bbox = _rclMesh.GetBoundBox();
    Base::Vector3f center = bbox.GetCenter();
    this->_transform.multVec(center, center);
    Base::Vector3f camera(center);
    camera.x += std::max<float>(std::max<float>(bbox.LengthX(), bbox.LengthY()), bbox.LengthZ());
    Base::Vector3f target(center);
    Base::Vector3f upvec(0.0F, 0.0F, 1.0F);

    out << "// CA:Camera, OB:Camera\n"
        << "currentprojection = orthographic(camera = (" << camera.x << ", " << camera.y << ", "
        << camera.z << "),\n"
        << "                                 target = (" << target.x << ", " << target.y << ", "
        << target.z
        << "),\n"
           "                                 showtarget = false,\n"
           "                                 up = ("
        << upvec.x << ", " << upvec.y << ", " << upvec.z << "));\n\n";

    // out << "// LA:Spot, OB:Lamp\n"
    //     << "// WO:World\n"
    //     << "currentlight = light(diffuse = rgb(1, 1, 1),\n"
    //        "                     specular = rgb(1, 1, 1),\n"
    //        "                     background = rgb(0.078281, 0.16041, 0.25),\n"
    //        "                     0.56639, 0.21839, 0.79467);\n\n";

    out << "// ME:Mesh, OB:Mesh\n";

    MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);
    clIter.Transform(this->_transform);
    clIter.Begin();
    clEnd.End();

    const MeshPointArray& rPoints = _rclMesh.GetPoints();
    const MeshFacetArray& rFacets = _rclMesh.GetFacets();
    bool saveVertexColor = (_material && _material->binding == MeshIO::PER_VERTEX
                            && _material->diffuseColor.size() == rPoints.size());
    bool saveFaceColor = (_material && _material->binding == MeshIO::PER_FACE
                          && _material->diffuseColor.size() == rFacets.size());
    // global mesh color
    Base::Color mc(0.8F, 0.8F, 0.8F);
    if (_material && _material->binding == MeshIO::OVERALL && _material->diffuseColor.size() == 1) {
        mc = _material->diffuseColor[0];
    }

    std::size_t index = 0;
    const MeshGeomFacet* pclFacet {};
    while (clIter < clEnd) {
        pclFacet = &(*clIter);

        out << "draw(surface(";

        // vertices
        for (const auto& pnt : pclFacet->_aclPoints) {
            out << '(' << pnt.x << ", " << pnt.y << ", " << pnt.z << ")--";
        }

        out << "cycle";

        if (saveVertexColor) {
            const MeshFacet& face = rFacets[index];
            out << ",\n             new pen[] {";
            for (int i = 0; i < 3; i++) {
                const Base::Color& c = _material->diffuseColor[face._aulPoints[i]];
                out << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
                if (i < 2) {
                    out << ", ";
                }
            }
            out << "}));\n";
        }
        else if (saveFaceColor) {
            const Base::Color& c = _material->diffuseColor[index];
            out << "),\n     rgb(" << c.r << ", " << c.g << ", " << c.b << "));\n";
        }
        else {
            out << "),\n     rgb(" << mc.r << ", " << mc.g << ", " << mc.b << "));\n";
        }

        ++clIter;
        ++index;
    }

    return true;
}

/** Saves an OFF file. */
bool MeshOutput::SaveOFF(std::ostream& out) const
{
    const MeshPointArray& rPoints = _rclMesh.GetPoints();
    const MeshFacetArray& rFacets = _rclMesh.GetFacets();

    if (!out || out.bad()) {
        return false;
    }

    Base::SequencerLauncher seq("saving...", _rclMesh.CountPoints() + _rclMesh.CountFacets());

    bool exportColor = false;
    if (_material) {
        if (_material->binding == MeshIO::PER_FACE) {
            Base::Console().Warning(
                "Cannot export color information because it's defined per face");
        }
        else if (_material->binding == MeshIO::PER_VERTEX) {
            if (_material->diffuseColor.size() != rPoints.size()) {
                Base::Console().Warning("Cannot export color information because there is a "
                                        "different number of points and colors");
            }
            else {
                exportColor = true;
            }
        }
        else if (_material->binding == MeshIO::OVERALL) {
            if (_material->diffuseColor.empty()) {
                Base::Console().Warning(
                    "Cannot export color information because there is no color defined");
            }
            else {
                exportColor = true;
            }
        }
    }

    if (exportColor) {
        out << "COFF\n";
    }
    else {
        out << "OFF\n";
    }
    out << rPoints.size() << " " << rFacets.size() << " 0\n";

    // vertices
    Base::Vector3f pt;
    std::size_t index = 0;
    for (auto it = rPoints.begin(); it != rPoints.end(); ++it, ++index) {
        if (this->apply_transform) {
            pt = this->_transform * *it;
        }
        else {
            pt.Set(it->x, it->y, it->z);
        }

        if (exportColor) {
            Base::Color c;
            if (_material->binding == MeshIO::PER_VERTEX) {
                c = _material->diffuseColor[index];
            }
            else {
                c = _material->diffuseColor.front();
            }

            int r = static_cast<int>(c.r * 255.0F);
            int g = static_cast<int>(c.g * 255.0F);
            int b = static_cast<int>(c.b * 255.0F);
            int a = static_cast<int>(c.a * 255.0F);

            out << pt.x << " " << pt.y << " " << pt.z << " " << r << " " << g << " " << b << " "
                << a << '\n';
        }
        else {
            out << pt.x << " " << pt.y << " " << pt.z << '\n';
        }
        seq.next(true);  // allow one to cancel
    }

    // facet indices (no texture and normal indices)
    for (const auto& it : rFacets) {
        out << "3 " << it._aulPoints[0] << " " << it._aulPoints[1] << " " << it._aulPoints[2]
            << '\n';
        seq.next(true);  // allow one to cancel
    }

    return true;
}

bool MeshOutput::SaveBinaryPLY(std::ostream& out) const
{
    WriterPLY writer(this->_rclMesh, this->_material);
    writer.SetTransform(this->_transform);
    return writer.SaveBinary(out);
}

bool MeshOutput::SaveAsciiPLY(std::ostream& out) const
{
    WriterPLY writer(this->_rclMesh, this->_material);
    writer.SetTransform(this->_transform);
    return writer.SaveAscii(out);
}

bool MeshOutput::SaveMeshNode(std::ostream& output)
{
    const MeshPointArray& rPoints = _rclMesh.GetPoints();
    const MeshFacetArray& rFacets = _rclMesh.GetFacets();

    if (!output || output.bad()) {
        return false;
    }

    // vertices
    output << "[" << '\n';
    if (this->apply_transform) {
        Base::Vector3f pt;
        for (const auto& it : rPoints) {
            pt = this->_transform * it;
            output << "v " << pt.x << " " << pt.y << " " << pt.z << '\n';
        }
    }
    else {
        for (const auto& it : rPoints) {
            output << "v " << it.x << " " << it.y << " " << it.z << '\n';
        }
    }
    // facet indices (no texture and normal indices)
    for (const auto& it : rFacets) {
        output << "f " << it._aulPoints[0] + 1 << " " << it._aulPoints[1] + 1 << " "
               << it._aulPoints[2] + 1 << '\n';
    }
    output << "]" << '\n';

    return true;
}

/** Saves the mesh object into an XML file. */
void MeshOutput::SaveXML(Base::Writer& writer) const
{
    const MeshPointArray& rPoints = _rclMesh.GetPoints();
    const MeshFacetArray& rFacets = _rclMesh.GetFacets();

    //  writer << writer.ind() << "<Mesh>" << '\n';

    writer.incInd();
    writer.Stream() << writer.ind() << "<Points Count=\"" << _rclMesh.CountPoints() << "\">"
                    << '\n';

    writer.incInd();
    if (this->apply_transform) {
        Base::Vector3f pt;
        for (const auto& it : rPoints) {
            pt = this->_transform * it;
            writer.Stream() << writer.ind() << "<P "
                            << "x=\"" << pt.x << "\" "
                            << "y=\"" << pt.y << "\" "
                            << "z=\"" << pt.z << "\"/>" << '\n';
        }
    }
    else {
        for (const auto& it : rPoints) {
            writer.Stream() << writer.ind() << "<P "
                            << "x=\"" << it.x << "\" "
                            << "y=\"" << it.y << "\" "
                            << "z=\"" << it.z << "\"/>" << '\n';
        }
    }
    writer.decInd();
    writer.Stream() << writer.ind() << "</Points>" << '\n';

    // write the faces
    writer.Stream() << writer.ind() << "<Faces Count=\"" << _rclMesh.CountFacets() << "\">" << '\n';

    writer.incInd();
    for (const auto& it : rFacets) {
        writer.Stream() << writer.ind() << "<F "
                        << "p0=\"" << it._aulPoints[0] << "\" "
                        << "p1=\"" << it._aulPoints[1] << "\" "
                        << "p2=\"" << it._aulPoints[2] << "\" "
                        << "n0=\"" << it._aulNeighbours[0] << "\" "
                        << "n1=\"" << it._aulNeighbours[1] << "\" "
                        << "n2=\"" << it._aulNeighbours[2] << "\"/>" << '\n';
    }
    writer.decInd();
    writer.Stream() << writer.ind() << "</Faces>" << '\n';

    writer.Stream() << writer.ind() << "</Mesh>" << '\n';
    writer.decInd();
}

/** Saves the mesh object into a 3MF file. */
bool MeshOutput::Save3MF(std::ostream& output) const
{
    Writer3MF writer(output);
    writer.AddMesh(_rclMesh, _transform, objectName);
    return writer.Save();
}

/** Writes an IDTF file. */
bool MeshOutput::SaveIDTF(std::ostream& str) const
{
    if (!str || str.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    const MeshPointArray& pts = _rclMesh.GetPoints();
    const MeshFacetArray& fts = _rclMesh.GetFacets();
    std::string resource = objectName;
    if (resource.empty()) {
        resource = "Resource";
    }

    str.precision(6);
    str.setf(std::ios::fixed | std::ios::showpoint);

    str << "FILE_FORMAT \"IDTF\"\n"
        << "FORMAT_VERSION 100\n\n";

    str << Base::tabs(0) << "NODE \"MODEL\" {\n";
    str << Base::tabs(1) << "NODE_NAME \"FreeCAD\"\n";
    str << Base::tabs(1) << "PARENT_LIST {\n";
    str << Base::tabs(2) << "PARENT_COUNT 1\n";
    str << Base::tabs(2) << "PARENT 0 {\n";
    str << Base::tabs(3) << "PARENT_NAME \"<NULL>\"\n";
    str << Base::tabs(3) << "PARENT_TM {\n";
    str << Base::tabs(4) << "1.000000 0.000000 0.000000 0.000000\n";
    str << Base::tabs(4) << "0.000000 1.000000 0.000000 0.000000\n";
    str << Base::tabs(4) << "0.000000 0.000000 1.000000 0.000000\n";
    str << Base::tabs(4) << "0.000000 0.000000 0.000000 1.000000\n";
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(2) << "}\n";
    str << Base::tabs(1) << "}\n";
    str << Base::tabs(1) << "RESOURCE_NAME \"" << resource << "\"\n";
    str << Base::tabs(0) << "}\n\n";

    str << Base::tabs(0) << "RESOURCE_LIST \"MODEL\" {\n";
    str << Base::tabs(1) << "RESOURCE_COUNT 1\n";
    str << Base::tabs(1) << "RESOURCE 0 {\n";
    str << Base::tabs(2) << "RESOURCE_NAME \"" << resource << "\"\n";
    str << Base::tabs(2) << "MODEL_TYPE \"MESH\"\n";
    str << Base::tabs(2) << "MESH {\n";
    str << Base::tabs(3) << "FACE_COUNT " << fts.size() << '\n';
    str << Base::tabs(3) << "MODEL_POSITION_COUNT " << pts.size() << '\n';
    str << Base::tabs(3) << "MODEL_NORMAL_COUNT " << 3 * fts.size() << '\n';
    str << Base::tabs(3) << "MODEL_DIFFUSE_COLOR_COUNT 0\n";
    str << Base::tabs(3) << "MODEL_SPECULAR_COLOR_COUNT 0\n";
    str << Base::tabs(3) << "MODEL_TEXTURE_COORD_COUNT 0\n";
    str << Base::tabs(3) << "MODEL_BONE_COUNT 0\n";
    str << Base::tabs(3) << "MODEL_SHADING_COUNT 1\n";
    str << Base::tabs(3) << "MODEL_SHADING_DESCRIPTION_LIST {\n";
    str << Base::tabs(4) << "SHADING_DESCRIPTION 0 {\n";
    str << Base::tabs(5) << "TEXTURE_LAYER_COUNT 0\n";
    str << Base::tabs(5) << "SHADER_ID 0\n";
    str << Base::tabs(4) << "}\n";
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(3) << "MESH_FACE_POSITION_LIST {\n";
    for (const auto& ft : fts) {
        str << Base::tabs(4) << ft._aulPoints[0] << " " << ft._aulPoints[1] << " "
            << ft._aulPoints[2] << '\n';
    }
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(3) << "MESH_FACE_NORMAL_LIST {\n";
    int index = 0;
    for (auto it = fts.begin(); it != fts.end(); ++it) {
        str << Base::tabs(4) << index << " " << index + 1 << " " << index + 2 << '\n';
        index += 3;
    }
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(3) << "MESH_FACE_SHADING_LIST {\n";
    for (auto it = fts.begin(); it != fts.end(); ++it) {
        str << Base::tabs(4) << "0\n";
    }
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(3) << "MODEL_POSITION_LIST {\n";
    for (const auto& pt : pts) {
        str << Base::tabs(4) << pt.x << " " << pt.y << " " << pt.z << '\n';
    }
    str << Base::tabs(3) << "}\n";
    str << Base::tabs(3) << "MODEL_NORMAL_LIST {\n";
    for (const auto& ft : fts) {
        MeshGeomFacet face = _rclMesh.GetFacet(ft);
        Base::Vector3f normal = face.GetNormal();
        str << Base::tabs(4) << normal.x << " " << normal.y << " " << normal.z << '\n';
        str << Base::tabs(4) << normal.x << " " << normal.y << " " << normal.z << '\n';
        str << Base::tabs(4) << normal.x << " " << normal.y << " " << normal.z << '\n';
    }

    str << Base::tabs(3) << "}\n";
    str << Base::tabs(2) << "}\n";
    str << Base::tabs(1) << "}\n";
    str << Base::tabs(0) << "}\n";

    return true;
}

/** Writes an MGL file. */
bool MeshOutput::SaveMGL(std::ostream& str) const
{
    /*
    light on
    list t 0 1 2 | 0 1 3 | 0 2 3 | 1 2 3
    list xt 1 1 0 0
    list yt -1 -1 1 0
    list zt -1 -1 -1 1
    triplot t xt yt zt 'b'
    #triplot t xt yt zt '#k'
    */
    if (!str || str.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    const MeshPointArray& pts = _rclMesh.GetPoints();
    const MeshFacetArray& fts = _rclMesh.GetFacets();

    str.precision(2);
    str.setf(std::ios::fixed | std::ios::showpoint);

    str << "light on\n";
    str << "list t ";
    for (const auto& ft : fts) {
        str << ft._aulPoints[0] << " " << ft._aulPoints[1] << " " << ft._aulPoints[2] << " | ";
    }
    str << std::endl;

    str << "list xt ";
    for (const auto& pt : pts) {
        str << pt.x << " ";
    }
    str << std::endl;

    str << "list yt ";
    for (const auto& pt : pts) {
        str << pt.y << " ";
    }
    str << std::endl;

    str << "list zt ";
    for (const auto& pt : pts) {
        str << pt.z << " ";
    }
    str << std::endl;

    str << "triplot t xt yt zt 'b'" << std::endl;
    str << "#triplot t xt yt zt '#k'" << std::endl;

    return true;
}

/** Writes an OpenInventor file. */
bool MeshOutput::SaveInventor(std::ostream& output) const
{
    WriterInventor writer(_rclMesh, _material);
    writer.SetTransform(_transform);
    return writer.Save(output);
}

/** Writes an X3D file. */
bool MeshOutput::SaveX3D(std::ostream& out) const
{
    if (!out || out.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    // XML header info
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    return SaveX3DContent(out, false);
}

/** Writes an X3D file. */
bool MeshOutput::SaveX3DContent(std::ostream& out, bool exportViewpoints) const
{
    if (!out || out.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    const MeshPointArray& pts = _rclMesh.GetPoints();
    const MeshFacetArray& fts = _rclMesh.GetFacets();
    Base::BoundBox3f bbox = _rclMesh.GetBoundBox();
    if (apply_transform) {
        bbox = bbox.Transformed(_transform);
    }

    Base::Color mat(0.65F, 0.65F, 0.65F);
    if (_material && _material->binding == MeshIO::Binding::OVERALL) {
        if (!_material->diffuseColor.empty()) {
            mat = _material->diffuseColor.front();
        }
    }
    bool saveVertexColor = (_material && _material->binding == MeshIO::PER_VERTEX
                            && _material->diffuseColor.size() == pts.size());
    bool saveFaceColor = (_material && _material->binding == MeshIO::PER_FACE
                          && _material->diffuseColor.size() == fts.size());

    Base::SequencerLauncher seq("Saving...", _rclMesh.CountFacets() + 1);
    out.precision(6);
    out.setf(std::ios::fixed | std::ios::showpoint);

    // Header info
    out << R"(<X3D profile="Immersive" version="3.2" xmlns:xsd=)"
        << "\"http://www.w3.org/2001/XMLSchema-instance\" xsd:noNamespaceSchemaLocation="
        << "\"http://www.web3d.org/specifications/x3d-3.2.xsd\" width=\"1280px\"  "
           "height=\"1024px\">\n";
    out << "  <head>\n"
        << "    <meta name=\"generator\" content=\"FreeCAD\"/>\n"
        << "    <meta name=\"author\" content=\"\"/> \n"
        << "    <meta name=\"company\" content=\"\"/>\n"
        << "  </head>\n";

    // Beginning
    out << "  <Scene>\n";

    if (exportViewpoints) {
        auto viewpoint = [&out](const char* text,
                                const Base::Vector3f& cnt,
                                const Base::Vector3f& pos,
                                const Base::Vector3f& axis,
                                float angle) {
            out << "    <Viewpoint id=\"" << text << "\" centerOfRotation=\"" << cnt.x << " "
                << cnt.y << " " << cnt.z << "\" position=\"" << pos.x << " " << pos.y << " "
                << pos.z << "\" orientation=\"" << axis.x << " " << axis.y << " " << axis.z << " "
                << angle << R"(" description="camera" fieldOfView="0.9">)"
                << "</Viewpoint>\n";
        };

        Base::Vector3f cnt = bbox.GetCenter();
        float dist = 1.2F * bbox.CalcDiagonalLength();
        float dist3 = 0.577350F * dist;  // sqrt(1/3) * dist

        viewpoint("Iso",
                  cnt,
                  Base::Vector3f(cnt.x + dist3, cnt.y - dist3, cnt.z + dist3),
                  Base::Vector3f(0.742906F, 0.307722F, 0.594473F),
                  1.21712F);
        viewpoint("Front",
                  cnt,
                  Base::Vector3f(cnt.x, cnt.y - dist, cnt.z),
                  Base::Vector3f(1.0F, 0.0F, 0.0F),
                  1.5707964F);
        viewpoint("Back",
                  cnt,
                  Base::Vector3f(cnt.x, cnt.y + dist, cnt.z),
                  Base::Vector3f(0.0F, 0.707106F, 0.707106F),
                  3.141592F);
        viewpoint("Right",
                  cnt,
                  Base::Vector3f(cnt.x + dist, cnt.y, cnt.z),
                  Base::Vector3f(0.577350F, 0.577350F, 0.577350F),
                  2.094395F);
        viewpoint("Left",
                  cnt,
                  Base::Vector3f(cnt.x - dist, cnt.y, cnt.z),
                  Base::Vector3f(-0.577350F, 0.577350F, 0.577350F),
                  4.188790F);
        viewpoint("Top",
                  cnt,
                  Base::Vector3f(cnt.x, cnt.y, cnt.z + dist),
                  Base::Vector3f(0.0F, 0.0F, 1.0F),
                  0.0F);
        viewpoint("Bottom",
                  cnt,
                  Base::Vector3f(cnt.x, cnt.y, cnt.z - dist),
                  Base::Vector3f(1.0F, 0.0F, 0.0F),
                  3.141592F);
    }

    if (apply_transform) {
        Base::Placement p(_transform);
        const Base::Vector3d& v = p.getPosition();
        const Base::Rotation& r = p.getRotation();
        Base::Vector3d axis;
        double angle {};
        r.getValue(axis, angle);
        out << "    <Transform "
            << "translation='" << v.x << " " << v.y << " " << v.z << "' "
            << "rotation='" << axis.x << " " << axis.y << " " << axis.z << " " << angle << "'>\n";
    }
    else {
        out << "    <Transform>\n";
    }
    out << "      <Shape>\n";
    out << "        <Appearance>\n"
           "          <Material diffuseColor='"
        << mat.r << " " << mat.g << " " << mat.b
        << "' shininess='0.9' specularColor='1 1 1'></Material>\n"
           "        </Appearance>\n";

    out << "        <IndexedFaceSet solid=\"false\" ";
    if (saveVertexColor) {
        out << "colorPerVertex=\"true\" ";
    }
    else if (saveFaceColor) {
        out << "colorPerVertex=\"false\" ";
    }

    out << "coordIndex=\"";
    for (const auto& ft : fts) {
        out << ft._aulPoints[0] << " " << ft._aulPoints[1] << " " << ft._aulPoints[2] << " -1 ";
    }
    out << "\">\n";

    out << "          <Coordinate point=\"";
    for (const auto& pt : pts) {
        out << pt.x << " " << pt.y << " " << pt.z << ", ";
    }
    out << "\"/>\n";

    // write colors per vertex or face
    if (saveVertexColor || saveFaceColor) {
        out << "          <Color color=\"";
        for (const auto& c : _material->diffuseColor) {
            out << c.r << " " << c.g << " " << c.b << ", ";
        }
        out << "\"/>\n";
    }

    // End
    out << "        </IndexedFaceSet>\n"
        << "      </Shape>\n"
        << "    </Transform>\n"
        << "    <Background groundColor=\"0.7 0.7 0.7\" skyColor=\"0.7 0.7 0.7\" />\n"
        << "    <NavigationInfo/>\n"
        << "  </Scene>\n"
        << "</X3D>\n";

    return true;
}

/** Writes an X3DOM file. */
bool MeshOutput::SaveX3DOM(std::ostream& out) const
{
    if (!out || out.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    // See:
    // https://stackoverflow.com/questions/31976056/unable-to-color-faces-using-indexedfaceset-in-x3dom
    //
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" "
           "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
    out << "<html xmlns='http://www.w3.org/1999/xhtml'>\n"
        << "  <head>\n"
        << "    <script type='text/javascript' src='http://www.x3dom.org/download/x3dom.js'> "
           "</script>\n"
        << "    <link rel='stylesheet' type='text/css' "
           "href='http://www.x3dom.org/download/x3dom.css'></link>\n"
        << "  </head>\n";

    auto onclick = [&out](const char* text) {
        out << "  <button onclick=\"document.getElementById('" << text
            << "').setAttribute('set_bind','true');\">" << text << "</button>\n";
    };

    onclick("Iso");
    onclick("Front");
    onclick("Back");
    onclick("Right");
    onclick("Left");
    onclick("Top");
    onclick("Bottom");

#if 0  // https://stackoverflow.com/questions/32305678/x3dom-how-to-make-zoom-buttons
    function zoom (delta) {
        var x3d = document.getElementById("right");
        var vpt = x3d.getElementsByTagName("Viewpoint")[0];
        vpt.fieldOfView = parseFloat(vpt.fieldOfView) + delta;
    }

    <button onclick="zoom(0.15);">Zoom out</button>
#endif

    SaveX3DContent(out, true);

    out << "</html>\n";

    return true;
}

/** Writes a Nastran file. */
bool MeshOutput::SaveNastran(std::ostream& output) const
{
    if (!output || output.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    MeshPointIterator clPIter(_rclMesh);
    clPIter.Transform(this->_transform);
    MeshFacetIterator clTIter(_rclMesh);
    int iIndx = 1;

    Base::SequencerLauncher seq("Saving...", _rclMesh.CountFacets() + 1);

    output.precision(3);
    output.setf(std::ios::fixed | std::ios::showpoint);
    for (clPIter.Init(); clPIter.More(); clPIter.Next()) {
        float x = clPIter->x;
        float y = clPIter->y;
        float z = clPIter->z;

        output << "GRID";

        output << std::setfill(' ') << std::setw(12) << iIndx;
        output << std::setfill(' ') << std::setw(16) << x;
        output << std::setfill(' ') << std::setw(8) << y;
        output << std::setfill(' ') << std::setw(8) << z;
        output << '\n';

        iIndx++;
        seq.next();
    }

    iIndx = 1;
    for (clTIter.Init(); clTIter.More(); clTIter.Next()) {
        output << "CTRIA3";

        output << std::setfill(' ') << std::setw(10) << iIndx;
        output << std::setfill(' ') << std::setw(8) << (int)0;
        output << std::setfill(' ') << std::setw(8) << clTIter.GetIndices()._aulPoints[1] + 1;
        output << std::setfill(' ') << std::setw(8) << clTIter.GetIndices()._aulPoints[0] + 1;
        output << std::setfill(' ') << std::setw(8) << clTIter.GetIndices()._aulPoints[2] + 1;
        output << '\n';

        iIndx++;
        seq.next();
    }

    output << "ENDDATA";

    return true;
}

/** Writes a Python module */
bool MeshOutput::SavePython(std::ostream& str) const
{
    if (!str || str.bad() || (_rclMesh.CountFacets() == 0)) {
        return false;
    }

    MeshFacetIterator clIter(_rclMesh);
    clIter.Transform(this->_transform);
    str.precision(4);
    str.setf(std::ios::fixed | std::ios::showpoint);

    str << "faces = [\n";
    for (clIter.Init(); clIter.More(); clIter.Next()) {
        const MeshGeomFacet& rFacet = *clIter;
        for (const auto& pnt : rFacet._aclPoints) {
            str << "[" << pnt.x << "," << pnt.y << "," << pnt.z << "],";
        }
        str << '\n';
    }

    str << "]\n";

    return true;
}

/** Writes a VRML file. */
bool MeshOutput::SaveVRML(std::ostream& output) const
{
    WriterVRML writer(_rclMesh, _material);
    writer.SetTransform(_transform);
    return writer.Save(output);
}

// ----------------------------------------------------------------------------

MeshCleanup::MeshCleanup(MeshPointArray& p, MeshFacetArray& f)
    : pointArray(p)
    , facetArray(f)
{}

void MeshCleanup::SetMaterial(Material* mat)
{
    materialArray = mat;
}

void MeshCleanup::RemoveInvalids()
{
    // first mark all points as invalid
    pointArray.SetFlag(MeshPoint::INVALID);
    std::size_t numPoints = pointArray.size();

    // Now go through the facets and invalidate facets with wrong indices
    // If a facet is valid all its referenced points are validated again
    // Points that are not referenced are still invalid and thus can be deleted
    for (auto& it : facetArray) {
        for (PointIndex point : it._aulPoints) {
            // vertex index out of range
            if (point >= numPoints) {
                it.SetInvalid();
                break;
            }
        }

        // validate referenced points
        if (it.IsValid()) {
            pointArray[it._aulPoints[0]].ResetInvalid();
            pointArray[it._aulPoints[1]].ResetInvalid();
            pointArray[it._aulPoints[2]].ResetInvalid();
        }
    }

    // Remove the invalid items
    RemoveInvalidFacets();
    RemoveInvalidPoints();
}

void MeshCleanup::RemoveInvalidFacets()
{
    MeshIsFlag<MeshFacet> flag;
    std::size_t countInvalidFacets =
        std::count_if(facetArray.begin(), facetArray.end(), [flag](const MeshFacet& f) {
            return flag(f, MeshFacet::INVALID);
        });
    if (countInvalidFacets > 0) {

        // adjust the material array if needed
        if (materialArray && materialArray->binding == MeshIO::PER_FACE
            && materialArray->diffuseColor.size() == facetArray.size()) {
            std::vector<Base::Color> colors;
            colors.reserve(facetArray.size() - countInvalidFacets);
            for (std::size_t index = 0; index < facetArray.size(); index++) {
                if (facetArray[index].IsValid()) {
                    colors.push_back(materialArray->diffuseColor[index]);
                }
            }

            materialArray->diffuseColor.swap(colors);
        }

        MeshFacetArray copy_facets(facetArray.size() - countInvalidFacets);
        // copy all valid facets to the new array
        std::remove_copy_if(facetArray.begin(),
                            facetArray.end(),
                            copy_facets.begin(),
                            [flag](const MeshFacet& f) {
                                return flag(f, MeshFacet::INVALID);
                            });
        facetArray.swap(copy_facets);
    }
}

void MeshCleanup::RemoveInvalidPoints()
{
    MeshIsFlag<MeshPoint> flag;
    std::size_t countInvalidPoints =
        std::count_if(pointArray.begin(), pointArray.end(), [flag](const MeshPoint& p) {
            return flag(p, MeshPoint::INVALID);
        });
    if (countInvalidPoints > 0) {
        // generate array of decrements
        std::vector<PointIndex> decrements;
        decrements.resize(pointArray.size());
        PointIndex decr = 0;

        MeshPointArray::_TIterator p_end = pointArray.end();
        std::vector<PointIndex>::iterator decr_it = decrements.begin();
        for (auto p_it = pointArray.begin(); p_it != p_end; ++p_it, ++decr_it) {
            *decr_it = decr;
            if (!p_it->IsValid()) {
                decr++;
            }
        }

        // correct point indices of the facets
        MeshFacetArray::_TIterator f_end = facetArray.end();
        for (auto f_it = facetArray.begin(); f_it != f_end; ++f_it) {
            f_it->_aulPoints[0] -= decrements[f_it->_aulPoints[0]];
            f_it->_aulPoints[1] -= decrements[f_it->_aulPoints[1]];
            f_it->_aulPoints[2] -= decrements[f_it->_aulPoints[2]];
        }

        // delete point, number of valid points
        std::size_t validPoints = pointArray.size() - countInvalidPoints;

        // adjust the material array if needed
        if (materialArray && materialArray->binding == MeshIO::PER_VERTEX
            && materialArray->diffuseColor.size() == pointArray.size()) {
            std::vector<Base::Color> colors;
            colors.reserve(validPoints);
            for (std::size_t index = 0; index < pointArray.size(); index++) {
                if (pointArray[index].IsValid()) {
                    colors.push_back(materialArray->diffuseColor[index]);
                }
            }

            materialArray->diffuseColor.swap(colors);
        }

        MeshPointArray copy_points(validPoints);
        // copy all valid facets to the new array
        std::remove_copy_if(pointArray.begin(),
                            pointArray.end(),
                            copy_points.begin(),
                            [flag](const MeshPoint& p) {
                                return flag(p, MeshPoint::INVALID);
                            });
        pointArray.swap(copy_points);
    }
}

// ----------------------------------------------------------------------------

MeshPointFacetAdjacency::MeshPointFacetAdjacency(std::size_t p, MeshFacetArray& f)
    : numPoints(p)
    , facets(f)
{
    Build();
}

void MeshPointFacetAdjacency::Build()
{
    std::vector<std::size_t> numFacetAdjacency(numPoints);
    for (const auto& it : facets) {
        numFacetAdjacency[it._aulPoints[0]]++;
        numFacetAdjacency[it._aulPoints[1]]++;
        numFacetAdjacency[it._aulPoints[2]]++;
    }

    pointFacetAdjacency.resize(numPoints);
    for (std::size_t i = 0; i < numPoints; i++) {
        pointFacetAdjacency[i].reserve(numFacetAdjacency[i]);
    }

    std::size_t numFacets = facets.size();
    for (std::size_t i = 0; i < numFacets; i++) {
        for (PointIndex ptIndex : facets[i]._aulPoints) {
            pointFacetAdjacency[ptIndex].push_back(i);
        }
    }
}

void MeshPointFacetAdjacency::SetFacetNeighbourhood()
{
    std::size_t numFacets = facets.size();
    for (std::size_t index = 0; index < numFacets; index++) {
        MeshFacet& facet1 = facets[index];
        for (int i = 0; i < 3; i++) {
            std::size_t n1 = facet1._aulPoints[i];
            std::size_t n2 = facet1._aulPoints[(i + 1) % 3];

            bool success = false;
            const std::vector<std::size_t>& refFacets = pointFacetAdjacency[n1];
            for (std::size_t it : refFacets) {
                if (it != index) {
                    MeshFacet& facet2 = facets[it];
                    if (facet2.HasPoint(n2)) {
                        facet1._aulNeighbours[i] = it;
                        success = true;
                        break;
                    }
                }
            }

            if (!success) {
                facet1._aulNeighbours[i] = FACET_INDEX_MAX;
            }
        }
    }
}
