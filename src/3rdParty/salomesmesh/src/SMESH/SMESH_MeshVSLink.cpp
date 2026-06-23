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

//local headers
#include <SMESH_MeshVSLink.hxx>
#include <SMESHDS_Group.hxx>

//occ headers
#include <TColgp_Array1OfXYZ.hxx>
#include <MeshVS_HArray1OfSequenceOfInteger.hxx>
#include <SMDS_VolumeTool.hxx>

//BEGIN sortNodes CHANGE /*
#include <gp_XYZ.hxx>

#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfReal.hxx>

#if OCC_VERSION_HEX >= 0x070000
IMPLEMENT_STANDARD_RTTIEXT(SMESH_MeshVSLink,MeshVS_DataSource3D)
#endif

#define MAX_SORT_NODE_COUNT 12

typedef std::map<double, int> T_Double_NodeID_Map;

//=======================================================================
//function : sortNodes
//purpose  :
//=======================================================================
bool sortNodes (const SMDS_MeshElement* theTool, const int* idNodes, int theNodeCount, int *myResult)
{
  if (theNodeCount < 3) return false;
  //INITIAL VARS
  TColgp_Array1OfXYZ myNodeList(1, theNodeCount);
  TColgp_Array1OfVec myVectList(1, theNodeCount);
  TColStd_Array1OfReal myAngleList(1, theNodeCount);
  gp_XYZ BaryCenter(0.,0.,0.);
  //int myResult[MAX_SORT_NODE_COUNT];
  //INITIALIZE THE POINTS
  for (int i = 1; i <= theNodeCount; i++ ) {
	const SMDS_MeshNode *n = theTool->GetNode( idNodes[i-1] );
	gp_XYZ aPoint(n->X(), n->Y(), n->Z());
	myNodeList.SetValue(i, aPoint);
  }
  //CALCULATE THE BARYCENTER
  for (int i = 1; i <= theNodeCount; i++ )
	BaryCenter += myNodeList.Value(i);
  BaryCenter /= theNodeCount;
  //CREATE THE VECTORS
  for (int i = 1; i <= theNodeCount; i++ ) {
	gp_Vec aVector(BaryCenter, myNodeList.Value(i));
	myVectList.SetValue(i, aVector);
  }
  //CALCULATE THE NORMAL USING FIRST THREE POINTS
  gp_XYZ q1 = myNodeList.Value(2)-myNodeList.Value(1);
  gp_XYZ q2 = myNodeList.Value(3)-myNodeList.Value(1);
  gp_XYZ normal  = q1 ^ q2;
  double modul = normal.Modulus();
  if ( modul > 0 )
	normal /= modul;
  //COUNT THE ANGLE OF THE FIRST WITH EACH
  for (int i = 1; i <= theNodeCount; i++ )
	myAngleList.SetValue(i, myVectList.Value(1).AngleWithRef(myVectList.Value(i), normal));
  //CREATE THE RESULT MAP (WILL SORT THE VERTICES)
  T_Double_NodeID_Map myMap;
  for (int i = 1; i <= theNodeCount; i++ )
	myMap.insert( make_pair(myAngleList.Value(i), idNodes[i-1]));
  int resID = 0;
  T_Double_NodeID_Map::iterator it;
  for(it = myMap.begin(); it!= myMap.end(); ++it)
	myResult[resID++] = it->second;

  return true;
}
//END sortNodes CHANGE */

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
SMESH_MeshVSLink::SMESH_MeshVSLink(const SMESH_Mesh *aMesh)
{
  myMesh = (SMESH_Mesh*) aMesh;
  //add the nodes
  SMDS_NodeIteratorPtr aNodeIter = myMesh->GetMeshDS()->nodesIterator();
  for(;aNodeIter->more();) {
	const SMDS_MeshNode* aNode = aNodeIter->next();
	myNodes.Add( aNode->GetID() );
  }
  //add the edges
  SMDS_EdgeIteratorPtr 	anEdgeIter = myMesh->GetMeshDS()->edgesIterator();
  for(;anEdgeIter->more();) {
	const SMDS_MeshEdge* anElem = anEdgeIter->next();
	myElements.Add( anElem->GetID() );
  }
  //add the faces
  SMDS_FaceIteratorPtr 	aFaceIter = myMesh->GetMeshDS()->facesIterator();
  for(;aFaceIter->more();) {
	const SMDS_MeshFace* anElem = aFaceIter->next();
	myElements.Add( anElem->GetID() );
  }
  //add the volumes
  SMDS_VolumeIteratorPtr aVolumeIter = myMesh->GetMeshDS()->volumesIterator();
  for(;aVolumeIter->more();) {
	const SMDS_MeshVolume* anElem = aVolumeIter->next();
	myElements.Add( anElem->GetID() );
  }
  //add the groups
  const std::set<SMESHDS_GroupBase*>& groups = myMesh->GetMeshDS()->GetGroups();
  if (!groups.empty()) {
	std::set<SMESHDS_GroupBase*>::const_iterator GrIt = groups.begin();
	for (; GrIt != groups.end(); GrIt++) {
	  SMESHDS_Group* grp = dynamic_cast<SMESHDS_Group*>(*GrIt);
	  if (!grp || grp->IsEmpty()) continue;
	  myGroups.Add(grp->GetID());
	}
  }
}

//================================================================
// Function : GetGeom
// Purpose  :
//================================================================
bool SMESH_MeshVSLink::GetGeom
   ( const int ID, const bool IsElement,
	TColStd_Array1OfReal& Coords, int& NbNodes,
	MeshVS_EntityType& Type ) const
{
  if( IsElement ) {
	const SMDS_MeshElement* myElem = myMesh->GetMeshDS()->FindElement(ID);
	if (!myElem) return false;
	if (myElem->GetType() == SMDSAbs_Edge)
	  Type = MeshVS_ET_Link;
	else if (myElem->GetType() == SMDSAbs_Face)
	  Type = MeshVS_ET_Face;
	else if (myElem->GetType() == SMDSAbs_Volume)
	  Type = MeshVS_ET_Volume;
	else
	  Type = MeshVS_ET_Element;
	NbNodes = myElem->NbNodes();
	int nbCoord = 1;
	for(int i = 0; i < NbNodes; i++ ) {
	  Coords(nbCoord++) = myElem->GetNode(i)->X();
	  Coords(nbCoord++) = myElem->GetNode(i)->Y();
	  Coords(nbCoord++) = myElem->GetNode(i)->Z();
	}
  }
  else {
	const SMDS_MeshNode* myNode = myMesh->GetMeshDS()->FindNode(ID);
	if (!myNode) return false;
	if (myNode->GetType() == SMDSAbs_Node)
	  Type = MeshVS_ET_Node;
	else
	  Type = MeshVS_ET_0D;
	NbNodes = 1;
	Coords(1) = myNode->X();
	Coords(2) = myNode->Y();
	Coords(3) = myNode->Z();
  }
  return true;
}

//================================================================
// Function : Get3DGeom
// Purpose  :
//================================================================
bool  SMESH_MeshVSLink::Get3DGeom
   ( const int ID, int& NbNodes,
	 Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const
{
  //check validity of element
  const SMDS_MeshElement* myVolume = myMesh->GetMeshDS()->FindElement(ID);
  if (!myVolume) return false;
  if (myVolume->GetType() != SMDSAbs_Volume) return false;

  //initialize VolumeTool
  SMDS_VolumeTool aTool;
  aTool.Set(myVolume);
  //set the nodes number
  NbNodes = aTool.NbNodes();// myVolume->NbNodes();
  //check validity or create Data
  int NbFaces = aTool.NbFaces();
  if (Data.IsNull())
	Data = new MeshVS_HArray1OfSequenceOfInteger(1, NbFaces);
  else if (Data->Length() != NbFaces) {
	Data.Nullify();
	Data = new MeshVS_HArray1OfSequenceOfInteger(1, NbFaces);
  }
  //iterate the faces and their nodes and add them to Data
  for (int itr=0;itr < NbFaces;itr++) {
	int NbThisFaceNodeCount = aTool.NbFaceNodes(itr);
	const int *FaceIndices = aTool.GetFaceNodesIndices(itr);
	int sortedFaceIndices[MAX_SORT_NODE_COUNT];
	TColStd_SequenceOfInteger aSeq;
	if (sortNodes(myVolume, FaceIndices, NbThisFaceNodeCount, sortedFaceIndices)) {
	  for (int itrX=0;itrX < NbThisFaceNodeCount;itrX++)
		aSeq.Append(sortedFaceIndices[itrX]);
	} else {
      for (int itrX=0;itrX < NbThisFaceNodeCount;itrX++)
		aSeq.Append(FaceIndices[itrX]);
	}
	Data->SetValue(itr+1, aSeq);
  }
  return true;
}

//================================================================
// Function : GetGeomType
// Purpose  :
//================================================================
bool SMESH_MeshVSLink::GetGeomType
	( const int ID,
	  const bool IsElement,
	  MeshVS_EntityType& Type ) const
{
  if( IsElement ) {
	const SMDS_MeshElement* myElem = myMesh->GetMeshDS()->FindElement(ID);
	if (!myElem) return false;
	if (myElem->GetType() == SMDSAbs_Edge)
	  Type = MeshVS_ET_Link;
	else if (myElem->GetType() == SMDSAbs_Face)
	  Type = MeshVS_ET_Face;
	else if (myElem->GetType() == SMDSAbs_Volume)
	  Type = MeshVS_ET_Volume;
	else
	  Type = MeshVS_ET_Element;
  }
  else {
	const SMDS_MeshNode* myNode = myMesh->GetMeshDS()->FindNode(ID);
	if (!myNode) return false;
	if (myNode->GetType() == SMDSAbs_Node)
	  Type = MeshVS_ET_Node;
	else
	  Type = MeshVS_ET_0D;
  }
  return true;
}

//================================================================
// Function : GetAddr
// Purpose  :
//================================================================
Standard_Address SMESH_MeshVSLink::GetAddr
	( const int, const bool ) const
{
  return NULL;
}

//================================================================
// Function : GetNodesByElement
// Purpose  :
//================================================================
bool SMESH_MeshVSLink::GetNodesByElement
	( const int ID,TColStd_Array1OfInteger& NodeIDs,int& NbNodes ) const
{
  const SMDS_MeshElement* myElem = myMesh->GetMeshDS()->FindElement(ID);
  if (!myElem) return false;
  NbNodes = myElem->NbNodes();
  for(int i = 0; i < NbNodes; i++ ) {
	const SMDS_MeshNode* aNode = myElem->GetNode(i);
	if (!aNode) return false;
	NodeIDs.SetValue(i+1, aNode->GetID());
  }
  return true;
}

//================================================================
// Function : GetAllNodes
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& SMESH_MeshVSLink::GetAllNodes() const
{
  return myNodes;
}

//================================================================
// Function : GetAllElements
// Purpose  :
//================================================================
const TColStd_PackedMapOfInteger& SMESH_MeshVSLink::GetAllElements() const
{
  return myElements;
}

//================================================================
// Function : GetAllElements
// Purpose  :
//================================================================
void SMESH_MeshVSLink::GetAllGroups(TColStd_PackedMapOfInteger& Ids) const
{
  Ids = myGroups;
}

//================================================================
// Function : GetNormal
// Purpose  :
//================================================================
bool SMESH_MeshVSLink::GetNormal
	( const int Id, const int Max,
	  double& nx, double& ny,double& nz ) const
{
  if(Max<3) return false;
  const SMDS_MeshElement* myElem = myMesh->GetMeshDS()->FindElement(Id);
  if(!myElem) return false;
  if(myElem->NbNodes() < 3) return false;
  gp_XYZ normal;
  gp_XYZ nodes[3];
  for (int itr = 0;itr < 3;itr++)
	nodes[itr] = gp_XYZ(myElem->GetNode(itr)->X(), myElem->GetNode(itr)->Y(), myElem->GetNode(itr)->Z());
  normal = (nodes[1]-nodes[0]) ^ (nodes[2]-nodes[0]);
  if ( normal.Modulus() > 0 )
	normal /= normal.Modulus();
  nx = normal.X();
  ny = normal.Y();
  nz = normal.Z();
  return true;
}
