//  SMESH  SMESH_MeshVSLink : Connection of SMESH with MeshVS from OCC 
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File      : SMESH_MeshVSLink.cxx
// Created   : Mon Dec 1 09:00:00 2008
// Author    : Sioutis Fotios
// Module    : SMESH

#ifndef _SMESH_MeshVSLink_HeaderFile
#define _SMESH_MeshVSLink_HeaderFile

#if OCC_VERSION_HEX < 0x070000
#ifndef _Handle_SMESH_MeshVSLink_HeaderFile
#include <Handle_SMESH_MeshVSLink.hxx>
#endif
#endif

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _TColStd_PackedMapOfInteger_HeaderFile
#include <TColStd_PackedMapOfInteger.hxx>
#endif
#ifndef _TColStd_HArray2OfInteger_HeaderFile
#include <TColStd_HArray2OfInteger.hxx>
#endif
#ifndef _TColStd_HArray2OfReal_HeaderFile
#include <TColStd_HArray2OfReal.hxx>
#endif
#ifndef _MeshVS_DataSource3D_HeaderFile
#include <MeshVS_DataSource3D.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _MeshVS_EntityType_HeaderFile
#include <MeshVS_EntityType.hxx>
#endif
#ifndef _TColStd_HArray1OfInteger_HeaderFile
#include <TColStd_HArray1OfInteger.hxx>
#endif
#ifndef TColStd_Array1OfReal_HeaderFile
#include <TColStd_Array1OfReal.hxx>
#endif
#ifndef MeshVS_HArray1OfSequenceOfInteger_HeaderFile
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#endif
#ifndef _SMESH_Mesh_HeaderFile
#include <SMESH_Mesh.hxx>
#endif
#ifndef _Standard_Version_HeaderFile
#include <Standard_Version.hxx>
#endif

#if OCC_VERSION_HEX >= 0x070000
DEFINE_STANDARD_HANDLE(SMESH_MeshVSLink, MeshVS_DataSource3D)
#endif
class SMESH_MeshVSLink : public MeshVS_DataSource3D {

  public:
	// Methods PUBLIC
	//

	//! Constructor <br>
	Standard_EXPORT SMESH_MeshVSLink(const SMESH_Mesh *aMesh);

	//Not implemented yet
	/*

    Standard_EXPORT   Standard_Boolean GetNodeNormal(const int ranknode,const int ElementId,double& nx,double& ny,double& nz) const;
    Standard_EXPORT   Standard_Boolean GetNormalsByElement(const int Id,const Standard_Boolean IsNodal,const int MaxNodes,Handle(TColStd_HArray1OfReal)& Normals) const;
	Standard_EXPORT   void GetAllGroups(TColStd_PackedMapOfInteger& Ids) const;
    Standard_EXPORT   Standard_Boolean GetGroup(const int Id,MeshVS_EntityType& Type,TColStd_PackedMapOfInteger& Ids) const;
    Standard_EXPORT   Standard_Address GetGroupAddr(const int ID) const;
	Standard_EXPORT   Standard_Boolean IsAdvancedSelectionEnabled() const;
	Standard_EXPORT   Bnd_Box GetBoundingBox() const;
    Standard_EXPORT   Standard_Boolean GetDetectedEntities(const Handle(MeshVS_Mesh)& Prs,const double X,const double Y,const double aTol,Handle(TColStd_HPackedMapOfInteger)& Nodes,Handle(TColStd_HPackedMapOfInteger)& Elements) ;
    Standard_EXPORT   Standard_Boolean GetDetectedEntities(const Handle(MeshVS_Mesh)& Prs,const double XMin,const double YMin,const double XMax,const double YMax,const double aTol,Handle(TColStd_HPackedMapOfInteger)& Nodes,Handle(TColStd_HPackedMapOfInteger)& Elements) ;
    Standard_EXPORT   Standard_Boolean GetDetectedEntities(const Handle(MeshVS_Mesh)& Prs,const TColgp_Array1OfPnt2d& Polyline,const Bnd_Box2d& aBox,const double aTol,Handle(TColStd_HPackedMapOfInteger)& Nodes,Handle(TColStd_HPackedMapOfInteger)& Elements) ;
	Standard_EXPORT   Standard_Boolean GetDetectedEntities(const Handle(MeshVS_Mesh)& Prs,Handle(TColStd_HPackedMapOfInteger)& Nodes,Handle(TColStd_HPackedMapOfInteger)& Elements) ;
	*/

	//! Returns geometry information about node ( if IsElement is False ) or element ( IsElement is True ) <br>
	//! by coordinates. For element this method must return all its nodes coordinates in the strict order: X, Y, Z and <br>
	//! with nodes order is the same as in wire bounding the face or link. NbNodes is number of nodes of element. <br>
	//! It is recommended to return 1 for node. Type is an element type. <br>
    Standard_EXPORT   Standard_Boolean GetGeom(const int ID,const Standard_Boolean IsElement,TColStd_Array1OfReal& Coords,int& NbNodes,MeshVS_EntityType& Type) const override;

    Standard_EXPORT   Standard_Boolean Get3DGeom(const int ID,int& NbNodes,Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const override;

	//! This method is similar to GetGeom, but returns only element or node type. This method is provided for <br>
	//! a fine performance. <br>
    Standard_EXPORT   Standard_Boolean GetGeomType(const int ID,const Standard_Boolean IsElement,MeshVS_EntityType& Type) const override;

	//! This method returns by number an address of any entity which represents element or node data structure. <br>
    Standard_EXPORT   Standard_Address GetAddr(const int ID,const Standard_Boolean IsElement) const override;

	//! This method returns information about what node this element consist of. <br>
    Standard_EXPORT /*virtual*/  Standard_Boolean GetNodesByElement(const int ID,TColStd_Array1OfInteger& NodeIDs,int& NbNodes) const override;

	//! This method returns map of all nodes the object consist of. <br>
	Standard_EXPORT  const TColStd_PackedMapOfInteger& GetAllNodes() const override;

	//! This method returns map of all elements the object consist of. <br>
	Standard_EXPORT  const TColStd_PackedMapOfInteger& GetAllElements() const override;

	//! This method calculates normal of face, which is using for correct reflection presentation. <br>
	//! There is default method, for advance reflection this method can be redefined. <br>
    Standard_EXPORT Standard_Boolean GetNormal(const int Id,const int Max,double& nx,double& ny,double& nz) const override;

	//! This method returns map of all groups the object contains. <br>
	Standard_EXPORT void GetAllGroups(TColStd_PackedMapOfInteger& Ids) const override;

	// Type management
	//
#if OCC_VERSION_HEX >= 0x070000
    DEFINE_STANDARD_RTTIEXT(SMESH_MeshVSLink,MeshVS_DataSource3D)
#else
    Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
#endif

  protected:
	// Methods PROTECTED
	//

	// Fields PROTECTED
	//

  private:
	// Methods PRIVATE
	//

	// Fields PRIVATE
	//
	SMESH_Mesh *myMesh;
	TColStd_PackedMapOfInteger myNodes;
	TColStd_PackedMapOfInteger myElements;
	TColStd_PackedMapOfInteger myGroups;
};
#include <SMESH_MeshVSLink.ixx>
// other Inline functions and methods (like "C++: function call" methods)
//
#endif
