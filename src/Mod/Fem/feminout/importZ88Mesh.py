# ***************************************************************************
# *   Copyright (c) 2016 Bernd Hahnebach <bernd@bimstatik.org>              *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

__title__ = "Mesh import and export for Z88 mesh file format"
__author__ = "Bernd Hahnebach"
__url__ = "https://www.freecad.org"

## @package importZ88Mesh
#  \ingroup FEM
#  \brief FreeCAD Z88 Mesh reader and writer for FEM workbench

import os

import FreeCAD
from FreeCAD import Console

from femmesh import meshtools
from builtins import open as pyopen

# ************************************************************************************************
# ********* generic FreeCAD import and export methods ********************************************
# names are fix given from FreeCAD, these methods are called from FreeCAD
# they are set in FEM modules Init.py


def open(filename):
    """called when freecad opens a file
    a FEM mesh object is created in a new document"""

    docname = os.path.splitext(os.path.basename(filename))[0]
    return insert(filename, docname)


def insert(filename, docname):
    """called when freecad wants to import a file
    a FEM mesh object is created in a existing document"""

    try:
        doc = FreeCAD.getDocument(docname)
    except NameError:
        doc = FreeCAD.newDocument(docname)
    FreeCAD.ActiveDocument = doc

    import_z88_mesh(filename, docname)
    return doc


def export(objectslist, filename):
    "called when freecad exports a file"
    if len(objectslist) != 1:
        Console.PrintError("This exporter can only export one object.\n")
        return
    obj = objectslist[0]
    if not obj.isDerivedFrom("Fem::FemMeshObject"):
        Console.PrintError("No FEM mesh object selected.\n")
        return
    femnodes_mesh = obj.FemMesh.Nodes
    femelement_table = meshtools.get_femelement_table(obj.FemMesh)
    z88_element_type = get_z88_element_type(obj.FemMesh, femelement_table)
    f = pyopen(filename, "w")
    write_z88_mesh_to_file(femnodes_mesh, femelement_table, z88_element_type, f)
    f.close()


# ************************************************************************************************
# ********* module specific methods **************************************************************
# reader:
# - a method uses a FemMesh instance, creates the FreeCAD document object and returns this object
# - a method creates and returns a FemMesh (no FreeCAD document object) out of the FEM mesh dict
# - a method reads the data from the mesh file or converts data and returns FEM mesh dictionary
#
# writer:
# - a method directly writes a FemMesh to the mesh file
# - a method takes a file handle, mesh data and writes to the file handle


# ********* reader *******************************************************************************
def import_z88_mesh(filename, analysis=None, docname=None):
    """read a FEM mesh from a Z88 mesh file and
    insert a FreeCAD FEM Mesh object in the ActiveDocument
    """

    try:
        doc = FreeCAD.getDocument(docname)
    except NameError:
        try:
            doc = FreeCAD.ActiveDocument
        except NameError:
            doc = FreeCAD.newDocument()
    FreeCAD.ActiveDocument = doc

    mesh_name = os.path.basename(os.path.splitext(filename)[0])

    femmesh = read(filename)
    if femmesh:
        mesh_object = doc.addObject("Fem::FemMeshObject", mesh_name)
        mesh_object.FemMesh = femmesh

    return mesh_object


def read(filename):
    """read a FemMesh from a Z88 mesh file and return the FemMesh"""
    # no document object is created, just the FemMesh is returned

    mesh_data = read_z88_mesh(filename)
    from . import importToolsFem

    return importToolsFem.make_femmesh(mesh_data)


def read_z88_mesh(z88_mesh_input):
    """reads a z88 mesh file z88i1.txt (Z88OSV14) or z88structure.txt (Z88AuroraV3)
    and extracts the nodes and elements
    """
    nodes = {}
    elements_hexa8 = {}
    elements_penta6 = {}
    elements_tetra4 = {}
    elements_tetra10 = {}
    elements_penta15 = {}
    elements_hexa20 = {}
    elements_tria3 = {}
    elements_tria6 = {}
    elements_quad4 = {}
    elements_quad8 = {}
    elements_seg2 = {}
    elements_seg3 = {}

    input_continues = False
    # elem = -1
    z88_element_type = 0

    z88_mesh_file = pyopen(z88_mesh_input, "r")
    mesh_info = z88_mesh_file.readline().strip().split()

    nodes_dimension = int(mesh_info[0])
    nodes_count = int(mesh_info[1])
    elements_count = int(mesh_info[2])
    kflag = int(mesh_info[4])
    # for non rotational elements is --> kflag = 0 --> cartesian, kflag = 1 polar coordinates
    if kflag:
        Console.PrintError("KFLAG = 1, Rotational coordinates not supported at the moment\n")
        return {}
    nodes_first_line = 2  # first line is mesh_info
    nodes_last_line = nodes_count + 1
    elemts_first_line = nodes_last_line + 1
    elements_last_line = elemts_first_line - 1 + elements_count * 2

    Console.PrintLog(f"{nodes_count}\n")
    Console.PrintLog(f"{elements_count}\n")
    Console.PrintLog(f"{nodes_last_line}\n")
    Console.PrintLog(f"{elemts_first_line}\n")
    Console.PrintLog(f"{elements_last_line}\n")

    z88_mesh_file.seek(0)  # go back to the beginning of the file
    for no, line in enumerate(z88_mesh_file):
        lno = no + 1
        linecolumns = line.split()

        if lno >= nodes_first_line and lno <= nodes_last_line:
            # node line
            node_no = int(linecolumns[0])
            node_x = float(linecolumns[2])
            node_y = float(linecolumns[3])
            if nodes_dimension == 2:
                node_z = 0.0
            elif nodes_dimension == 3:
                node_z = float(linecolumns[4])
            nodes[node_no] = FreeCAD.Vector(node_x, node_y, node_z)

        if lno >= elemts_first_line and lno <= elements_last_line:
            # first element line
            if not input_continues:
                elem_no = int(linecolumns[0])
                z88_element_type = int(linecolumns[1])
                input_continues = True

            # second element line
            elif input_continues:
                # not supported elements
                if z88_element_type == 8:
                    # torus8
                    Console.PrintError("Z88 Element No. 8, torus8\n")
                    Console.PrintError("Rotational elements are not supported at the moment\n")
                    return {}
                elif z88_element_type == 12:
                    # torus12
                    Console.PrintError("Z88 Element No. 12, torus12\n")
                    Console.PrintError("Rotational elements are not supported at the moment\n")
                    return {}
                elif z88_element_type == 15:
                    # torus6
                    Console.PrintError("Z88 Element No. 15, torus6\n")
                    Console.PrintError("Rotational elements are not supported at the moment\n")
                    return {}
                elif z88_element_type == 19:
                    # platte16
                    Console.PrintError("Z88 Element No. 19, platte16\n")
                    Console.PrintError("Not supported at the moment\n")
                    return {}
                elif z88_element_type == 21:
                    # schale16, mixture made from hexa8 and hexa20 (thickness is linear)
                    Console.PrintError("Z88 Element No. 21, schale16\n")
                    Console.PrintError("Not supported at the moment\n")
                    return {}
                elif z88_element_type == 22:
                    # schale12, mixtrue made from prism6 and prism15 (thickness is linear)
                    Console.PrintError("Z88 Element No. 22, schale12\n")
                    Console.PrintError("Not supported at the moment\n")
                    return {}

                # supported elements
                elif (
                    z88_element_type == 2
                    or z88_element_type == 4
                    or z88_element_type == 5
                    or z88_element_type == 9
                    or z88_element_type == 13
                    or z88_element_type == 25
                ):
                    # stab4 or stab5 or welle5 or beam13 or beam25 Z88 --> seg2 FreeCAD
                    # N1, N2
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    elements_seg2[elem_no] = (nd1, nd2)
                    input_continues = False
                elif z88_element_type == 3 or z88_element_type == 14 or z88_element_type == 24:
                    # scheibe3 or scheibe14 or schale24 Z88 --> tria6 FreeCAD
                    # N1, N2, N3, N4, N5, N6
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    nd5 = int(linecolumns[4])
                    nd6 = int(linecolumns[5])
                    elements_tria6[elem_no] = (nd1, nd2, nd3, nd4, nd5, nd6)
                    input_continues = False
                elif z88_element_type == 7 or z88_element_type == 20 or z88_element_type == 23:
                    # scheibe7 or platte20 or schale23 Z88 --> quad8 FreeCAD
                    # N1, N2, N3, N4, N5, N6, N7, N8
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    nd5 = int(linecolumns[4])
                    nd6 = int(linecolumns[5])
                    nd7 = int(linecolumns[6])
                    nd8 = int(linecolumns[7])
                    elements_quad8[elem_no] = (nd1, nd2, nd3, nd4, nd5, nd6, nd7, nd8)
                    input_continues = False
                elif z88_element_type == 17:
                    # volume17 Z88 --> tetra4 FreeCAD
                    # N4, N2, N3, N1
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    elements_tetra4[elem_no] = (nd4, nd2, nd3, nd1)
                    input_continues = False
                elif z88_element_type == 16:
                    # volume16 Z88 --> tetra10 FreeCAD
                    # N1, N2, N4, N3, N5, N8, N10, N7, N6, N9
                    # Z88 to FC is different as FC to Z88
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    nd5 = int(linecolumns[4])
                    nd6 = int(linecolumns[5])
                    nd7 = int(linecolumns[6])
                    nd8 = int(linecolumns[7])
                    nd9 = int(linecolumns[8])
                    nd10 = int(linecolumns[9])
                    elements_tetra10[elem_no] = (nd1, nd2, nd4, nd3, nd5, nd8, nd10, nd7, nd6, nd9)
                    input_continues = False
                elif z88_element_type == 1:
                    # volume1 Z88 --> hexa8 FreeCAD
                    # N1, N2, N3, N4, N5, N6, N7, N8
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    nd5 = int(linecolumns[4])
                    nd6 = int(linecolumns[5])
                    nd7 = int(linecolumns[6])
                    nd8 = int(linecolumns[7])
                    elements_hexa8[elem_no] = (nd1, nd2, nd3, nd4, nd5, nd6, nd7, nd8)
                    input_continues = False
                elif z88_element_type == 10:
                    # volume10 Z88 --> hexa20 FreeCAD
                    # N2, N3, N4, N1, N6, N7, N8, N5, N10, N11
                    # N12, N9,  N14, N15, N16, N13, N18, N19, N20, N17
                    # or turn by 90 degree and they match !
                    # N1, N2, N3, N4, N5, N6, N7, N8, N9, N10
                    # N11, N12, N13, N14, N15, N16, N17, N18, N19, N20
                    nd1 = int(linecolumns[0])
                    nd2 = int(linecolumns[1])
                    nd3 = int(linecolumns[2])
                    nd4 = int(linecolumns[3])
                    nd5 = int(linecolumns[4])
                    nd6 = int(linecolumns[5])
                    nd7 = int(linecolumns[6])
                    nd8 = int(linecolumns[7])
                    nd9 = int(linecolumns[8])
                    nd10 = int(linecolumns[9])
                    nd11 = int(linecolumns[10])
                    nd12 = int(linecolumns[11])
                    nd13 = int(linecolumns[12])
                    nd14 = int(linecolumns[13])
                    nd15 = int(linecolumns[14])
                    nd16 = int(linecolumns[15])
                    nd17 = int(linecolumns[16])
                    nd18 = int(linecolumns[17])
                    nd19 = int(linecolumns[18])
                    nd20 = int(linecolumns[19])
                    elements_hexa20[elem_no] = (
                        nd1,
                        nd2,
                        nd3,
                        nd4,
                        nd5,
                        nd6,
                        nd7,
                        nd8,
                        nd9,
                        nd10,
                        nd11,
                        nd12,
                        nd13,
                        nd14,
                        nd15,
                        nd16,
                        nd17,
                        nd18,
                        nd19,
                        nd20,
                    )
                    input_continues = False

                # unknown elements
                # some examples have -1 for some teaching reasons to show some other stuff
                else:
                    Console.PrintError("Unknown element\n")
                    return {}

    for n in nodes:
        Console.PrintLog(str(n) + "  " + str(nodes[n]) + "\n")
    for e in elements_tria6:
        Console.PrintLog(str(e) + "  " + str(elements_tria6[e]) + "\n")
    FreeCAD.Console.PrintLog("\n")

    z88_mesh_file.close()

    return {
        "Nodes": nodes,
        "Seg2Elem": elements_seg2,
        "Seg3Elem": elements_seg3,
        "Tria3Elem": elements_tria3,
        "Tria6Elem": elements_tria6,
        "Quad4Elem": elements_quad4,
        "Quad8Elem": elements_quad8,
        "Tetra4Elem": elements_tetra4,
        "Tetra10Elem": elements_tetra10,
        "Hexa8Elem": elements_hexa8,
        "Hexa20Elem": elements_hexa20,
        "Penta6Elem": elements_penta6,
        "Penta15Elem": elements_penta15,
    }


# ********* writer *******************************************************************************
def write(fem_mesh, filename):
    """directly write a FemMesh to a Z88 mesh file format
    fem_mesh: a FemMesh"""

    if not fem_mesh.isDerivedFrom("Fem::FemMesh"):
        Console.PrintError("Not a FemMesh was given as parameter.\n")
        return
    femnodes_mesh = fem_mesh.Nodes
    femelement_table = meshtools.get_femelement_table(fem_mesh)
    z88_element_type = get_z88_element_type(fem_mesh, femelement_table)
    f = pyopen(filename, "w")
    write_z88_mesh_to_file(femnodes_mesh, femelement_table, z88_element_type, f)
    f.close()


def write_z88_mesh_to_file(femnodes_mesh, femelement_table, z88_element_type, f):
    node_dimension = 3  # 2 for 2D not supported
    if (
        z88_element_type == 4
        or z88_element_type == 17
        or z88_element_type == 16
        or z88_element_type == 1
        or z88_element_type == 10
    ):
        node_dof = 3
    elif z88_element_type == 23 or z88_element_type == 24:
        node_dof = 6  # schalenelemente
    else:
        Console.PrintError("Error: wrong z88_element_type.\n")
        return
    node_count = len(femnodes_mesh)
    element_count = len(femelement_table)
    dofs = node_dof * node_count
    unknown_flag = 0
    written_by = "written by FreeCAD"

    # first line, some z88 specific stuff
    f.write(
        "{} {} {} {} {} {}\n".format(
            node_dimension, node_count, element_count, dofs, unknown_flag, written_by
        )
    )
    # nodes
    for node in femnodes_mesh:
        vec = femnodes_mesh[node]
        f.write(f"{node} {node_dof} {vec.x:.6f} {vec.y:.6f} {vec.z:.6f}\n")
    # elements
    for element in femelement_table:
        # z88_element_type is checked for every element
        # but mixed elements are not supported up to date
        n = femelement_table[element]
        if (
            z88_element_type == 2
            or z88_element_type == 4
            or z88_element_type == 5
            or z88_element_type == 9
            or z88_element_type == 13
            or z88_element_type == 25
        ):
            # seg2 FreeCAD --> stab4 Z88
            # N1, N2
            f.write(f"{element} {z88_element_type}\n")
            f.write(f"{n[0]} {n[1]}\n")
        elif z88_element_type == 3 or z88_element_type == 14 or z88_element_type == 24:
            # tria6 FreeCAD --> schale24 Z88
            # N1, N2, N3, N4, N5, N6
            f.write(f"{element} {z88_element_type}\n")
            f.write(f"{n[0]} {n[1]} {n[2]} {n[3]} {n[4]} {n[5]}\n")
        elif z88_element_type == 7 or z88_element_type == 20 or z88_element_type == 23:
            # quad8 FreeCAD --> schale23 Z88
            # N1, N2, N3, N4, N5, N6, N7, N8
            f.write(f"{element} {z88_element_type}\n")
            f.write(f"{n[0]} {n[1]} {n[2]} {n[3]} {n[4]} {n[5]} {n[6]} {n[7]}\n")
        elif z88_element_type == 17:
            # tetra4 FreeCAD --> volume17 Z88
            # N4, N2, N3, N1
            f.write(f"{element} {z88_element_type}\n")
            f.write(f"{n[3]} {n[1]} {n[2]} {n[0]}\n")
        elif z88_element_type == 16:
            # tetra10 FreeCAD --> volume16 Z88
            # N1, N2, N4, N3, N5, N9, N8, N6, N10, N7, FC to Z88 is different as Z88 to FC
            f.write(f"{element} {z88_element_type}\n")
            f.write(
                "{} {} {} {} {} {} {} {} {} {}\n".format(
                    n[0], n[1], n[3], n[2], n[4], n[8], n[7], n[5], n[9], n[6]
                )
            )
        elif z88_element_type == 1:
            # hexa8 FreeCAD --> volume1 Z88
            # N1, N2, N3, N4, N5, N6, N7, N8
            f.write(f"{element} {z88_element_type}\n")
            f.write(f"{n[0]} {n[1]} {n[2]} {n[3]} {n[4]} {n[5]} {n[6]} {n[7]}\n")
        elif z88_element_type == 10:
            # hexa20 FreeCAD --> volume10 Z88
            # N2, N3, N4, N1, N6, N7, N8, N5, N10, N11
            # N12, N9,  N14, N15, N16, N13, N18, N19, N20, N17
            # or turn by 90 degree and they match !
            # N1, N2, N3, N4, N5, N6, N7, N8, N9, N10
            # N11, N12, N13, N14, N15, N16, N17, N18, N19, N20
            f.write(f"{element} {z88_element_type}\n")
            f.write(
                "{} {} {} {} {} {} {} {} {} {} "
                "{} {} {} {} {} {} {} {} {} {}\n".format(
                    n[0],
                    n[1],
                    n[2],
                    n[3],
                    n[4],
                    n[5],
                    n[6],
                    n[7],
                    n[8],
                    n[9],
                    n[10],
                    n[11],
                    n[12],
                    n[13],
                    n[14],
                    n[15],
                    n[16],
                    n[17],
                    n[18],
                    n[19],
                )
            )
        else:
            Console.PrintError(f"Writing of Z88 elementtype {z88_element_type} not supported.\n")
            # TODO support schale12 (made from prism15) and schale16 (made from hexa20)
            return


# Helper
def get_z88_element_type(femmesh, femelement_table=None):
    return z88_ele_types[meshtools.get_femmesh_eletype(femmesh, femelement_table)]


z88_ele_types = {
    "tetra4": 17,
    "tetra10": 16,
    "hexa8": 1,
    "hexa20": 10,
    "tria3": 0,  # no tria3 available in Z88
    "tria6": 24,
    "quad4": 0,  # no quad4 available in Z88
    "quad8": 23,
    "seg2": 4,  # 3D Truss element
    "seg3": 4,  # 3D Truss element
    "None": 0,
}
