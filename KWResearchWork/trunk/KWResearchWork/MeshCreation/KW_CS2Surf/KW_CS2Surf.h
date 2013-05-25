#pragma once
#ifndef KW_CS2SURF_H
#define KW_CS2SURF_H

#include "../../stdafx.h"
#include "../Ctr2SufManager/Ctr2SufManager.h"

//some predefined threshold values
const int EXTRU_OUTBOUND_PERTRU_DIST=60;

const int MESH_REFINE_MAX_TIME=1024;
const double SS_COMBINE_CYLINDER_SHRINK_DIST=50;

class KW_CS2Surf : public Ctr2SufManager
{
public:
	KW_CS2Surf(void);
	
	~KW_CS2Surf(void);

	//clear all the data
	void Reset();

	void Render();

	//read contour from vector<CurveNetwork>
	void readContourFromVec(vector<CurveNetwork> vecCurveNetwork,float* fPreSetBBox,vector<vector<Point_3>>& MeshBoundingProfile3D);
	//convert the data in vecTempCN to float and then back to double
	//this is a temporary approach,it's better to use float in all mesh creation function
	void LowerPrecision();

	//generate mesh from contour
	bool ctr2sufProc(vector<vector<Point_3> >& MeshBoundingProfile3D,vector<Point_3>& vecTestPoint);

	//render cross section single cylinder related
	int GetCSCylinderNum() {return this->vecSinglePoly.size();}
	int GetRenderSinglePolyIndex() {return this->iRenderSinglePoly;}
	void SetRenderSinglePolyIndex(int iInput) {this->iRenderSinglePoly=iInput;}
	//render cross section union cylinder related
	int GetUNCylinderNum() {return this->vecUnionPoly.size();}
	int GetRenderUNPolyIndex() {return this->iRenderUnionPoly;}
	void SetRenderUNPolyIndex(int iInput) {this->iRenderUnionPoly=iInput;}
	//render subspace related
	int GetSSNum() {return this->iSSspacenum;}

	//sketch to split the originally connected components (cylinders)
	bool SketchSplit(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport);
	//sketch to join the originally irrelevant components (cylinders)
	bool SketchJoin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport);

	//set contour name
	void SetCtrFileName(CString CSIn) {this->CtrName=CSIn;}	

private:
	
	//kw debug test
	CString CtrName;

	//test points
	vector<Point_3> vecTestPoint;
	vector<Segment_3> vecTestSeg;
	//single generated cylinders
	vector<KW_Mesh> vecSinglePoly;
	//render each cross section cylinder
	int iRenderSinglePoly;
	//unions of cylinders 
	vector<KW_Mesh> vecUnionPoly;
	//render each union cylinder
	int iRenderUnionPoly;

	vector<Triangle_3> vecTestTri;

	//store the resized curve network
	vector<CurveNetwork> vecTempCN;

	//for each face,save the polygon info
	vector<PolygonOnFace> vecPOF;
	////for each face,save the PolyhedronFromPOF generated from PolygonOnFace
	//vector<PolyhedronFromPOF> vecPFPOF;

	//points&triangles computed in each subspace
	//generate initial submesh in each subspace
	vector<vector<Point_3>> vecvecSubPoint;
	vector<vector<vector<int>>> vecvecSubSurf;
	//face points in each subspace,each face maintain a point vector
	vector<vector<vector<Point_3>>>	vecvecvecFacePoint;
	//id of the subspace each vecSubPoint&vecSubSurf belong to
	vector<int> vecSubSurfSSId;


	//initial polyhedron
	KW_Mesh InitPolyh;
	//Polyhedron InitPolyh;

	//subspace information,using stl to represent instead of array,to make subspace combination easier
	//some unuseful variables in Ctr2SufManager are ignored
	//kw: position of bounding points of all subspaces
	vector<float> vecSSver;
	//kw: indices of two endpoints of all bounding edges
	vector<int> vecSSedge;
	//kw: number of faces of all subspaces
	int iSSfacenum;
	//kw: number of edges on each face
	vector<int> vecSSfaceedgenum;
	//kw: indices of edges on each face
	vector<vector<int>> vecvecSSface;
	//kw: index of the plane each face lies on
	vector<int> vecSSface_planeindex;
	//kw: number of subspaces
	int iSSspacenum;
	//kw: number of faces in each subspace
	vector<int> vecSSspacefacenum;
	//kw: indices of faces in each subspace
	vector<vector<int>> vecvecSSspace;
	//mesh formed from each subspace
	vector<KW_Mesh> vecSSMesh;
	//subspace information

	//clear the above subspace information
	void clearStlSubspaceInfo();
	//partition,save result into stl defined above
	void partitionStl();
	//number == number ssfacenum of Ctr2SufManager
	//vector<ResortedFace> vecResortFace;//defined in Ctr2SufManager
	//sort face info: boundary face or not,four vertices of each face (in order),
	//orientation of each face in each subspace, height of each face in each subspace...
	void SortFaceInfo();
	//combine subspace,after PutCNtoFace()
	//during combination,the deleted subspaces are left empty instead of acturally deleted 
	//so as to vecResortFace and vecSSMesh
	void CombineSS();
	//collect the subspaces need to combine
	void CollectSSToCombine(vector<Int_Int_Pair>& vecSpacesToCombine,vector<int>& vecValidFace);
	//get subspace to combine
	//iCurrSSID: id of the current subspace,iValidFaceID: id of the only face in the current subspace containing CN
	//iAdjFaceID: id of the face adjacent to the two subspaces
	//iNbSpaceID: neighbor subspace to combine
	void GetSSToCombine(int iCurrSSID,int iValidFaceID,int& iAdjFaceID,int& iNbSpaceID);
	//combine two subspaces
	void CombineTwoSS(int iLeftSSId,int iDelSSId,int iValidFaceId);
	//get a square composed of four input points
	//void GetSquareFace(vector<Point_3>& InOutPoints);//defined in Ctr2SufManager
	//compute which orientation does the subspace lie on w.r.t. the face
	//and the height vector of the subspace w.r.t. the face 
	//orientations for faces on the XOY plane: +z: true -z: false
	//orientations for faces on the YOZ plane: +x: true -x: false
	//orientations for faces on the XOZ plane: +y: true -y: false
	void GetFacePara(int iFaceId,int iSubspaceId,Point_3 SSCenter,bool& bOrient,Vector_3& HeightVec); 
	//get the center of the subspace
	void GetSSCenter(int iSubspaceId,Point_3& SSCenter);
	//compute mesh formed by each subspace
	void GetSSMesh();



	//put all curve network to each face in each subspace
	void PutCNtoFace();
	//subtract the inner of a hole from the outer of a hole/circle
	void SubtractInnerHole(CurveNetwork InputCN,Pwh_list_2& CSSubtractResult);
	//get intersection of curve network and one face
	//the orientation of the face points will change to CCW
	bool IntersectCnFace(vector<Point_3>& InputFace,CurveNetwork InputCN,Pwh_list_2 CSSubtractResult,PolygonOnFace& InOutPOF);
	//get intersection of the subtraction result and face
	void IntersectCSSubFace(Polygon_2 Face2D,Pwh_list_2& CSSubtractResult,list<bool>& listBIntersectFace);
	//convert all 2d polygons to 3D
	void Polygon2Dto3D(CurveNetwork InputCN,Pwh_list_2 PwhResult2D,list<bool> listBIntersectFace,PolygonOnFace& InOutPOF);
	//Align computed PolygonOnFace with CurveNetwork & subspace face
	void AlignPOFData(CurveNetwork InCN,vector<Point_3> InFace,PolygonOnFace& InOutPOF);

	//generate initial mesh
	void GenInitMesh();
	//generate submesh in each subspace
	bool GenSubMesh(int iSubSpaceId,vector<Point_3>& vecSubPoint,vector<vector<int>>& vecSubSurf,vector<vector<Point_3>>& vecvecFacePoint);
	//compute the unions of all cylinders in the subspace
	//TotalIntersectPwh: Pwhs related to intersection calculation
//	void ComputeUnionInSubspace(vector<Int_Int_Pair> IntersectPwh,vector<PolyhedronFromPOF> vecPFPOF,GmpPolyhedron& ResultPolyh);
	//used in the initial reconstruction process
	void ComputeUnionInSubspace(vector<PolyhedronFromPOF> vecPFPOF,KW_Mesh& ResultPolyh);
	//used in the sketch-assist topology join
	void ComputeUnionInSubspace(vector<KW_Mesh> vecSinglePoly,KW_Mesh& ResultPolyh);
	//used in the sketch-assist topology split
	void ComputeUnionInSubspace(vector<KW_Mesh> vecSplitPoly,vector<KW_Mesh> vecOtherPoly,KW_Mesh& ResultPolyh);
	//format conversion between cgal and carve csg
	void CGALKW_MeshToCarveArray(KW_Mesh PolyIn,vector<CarveVertex>& verts,vector<CarveFace>& faces);
	void CarvePolyToCGALKW_Mesh(CarvePoly* pPolyIn,	KW_Mesh& PolyOut);
	//generate polyhedrons from POF
	//IntersectPwh: polygons on the current face who have intersections with the bounding edges of the face
	//setFacePoint: all the points on the bounding faces of the subspace
	void POFToPFPOF(int iFaceId,int iSubSpaceId,vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri,bool bIsolatedCylin,
		PolyhedronFromPOF& InOutPFPOF,vector<Point_3>& vecFacePoint);

	//collect the subspace info,to judge if the cylinder is totally inside the subspace and its shrinked version
	//also shrink the subspace to help further computation
	void ShrinkCollectSSInfo(KW_Mesh& SSInOut,vector<vector<Triangle_3>>& vecSSFaceTri,vector<vector<Triangle_3>>& vecShrinkSSFaceTri);
	//compute the extruded bound of the cs
	//Pwh3DIn:original CS on subspace face
	//Pwh3DOut: corresponding extruded CS point positions
	//ExtruCenter: center of Pwh3DOut 
	bool GetOutBound(int iSubSpaceId,ResortedFace FaceInfo,int iFaceId,vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri,
		bool bIsolatedCylin,Polygon_with_holes_3 Pwh3DIn,Vector_3 HeightVec,Polygon_with_holes_3& Pwh3DOut,Point_3& ExtruCenter);
	//check the intersection between the extruded cylinder and subspace
	bool CheckCylinderInsideSS(vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri,Polygon_with_holes_3& PwhExtru);
	//judge if a point is inside a subspace in mesh creation algorithm 
	//(omit checking some triangles on the same plane if intersection point on other triangles on this plane has been detected)
	//can be extended to general polyhedron
	bool JudgePointInsideSS(Point_3 PointToJudge,vector<vector<Triangle_3>> vecSSFaceTri);
	//strict: judge the distance from the point to each face of the subspace, if not large enough, then consider as outside
	bool JudgePointInsideSSStrict(Point_3 PointToJudge,vector<vector<Triangle_3>> vecSSFaceTri);
	//compute one polyhedron from one Polygon_with_holes_3
	//InPwh2 is used to compute the triangulation(the top and bottom of the cylinder)
//	void GetOnePolyhFromPwh3(Polygon_with_holes_3 InPwh3,Polygon_with_holes_2 InPwh2,vector<Point_3> PerturbedOutBound,
//		Vector_3 HeightVec,bool bOrient,KW_Mesh& OutPolyh);
	//void GetOnePolyhFromPwh3(Polygon_with_holes_3 InPwh3,Polygon_with_holes_2 InPwh2,vector<Point_3> PerturbedOutBound,
	//	Vector_3 HeightVec,KW_Mesh& OutPolyh);
	//CSPwh3:Pwh of the cross sections;ExtruPwh3: Pwh of the extruded cross sections
	void GetOnePolyhFromPwh3(Polygon_with_holes_3 CSPwh3,Polygon_with_holes_2 CSPwh2,Polygon_with_holes_3 ExtruPwh3,Polygon_with_holes_2 ExtruPwh2,
		Vector_3 HeightVec,KW_Mesh& OutPolyh);
	//build the side faces of the silhouette cylinder
	//each element of vecSideTri is the three vertex indices of a triangle
	//iStartId; the id of the start vertex
	//iEdgeSize is the number of edges(i.e. vertices) on original polygon
	void BuildSideFaces(int iStartId,int iEdgeSize,int iTotalPointNum,vector<vector<int>>& vecSideTri);
	//build the top and bottom faces
	//the bottom faces correspond to the input CS
	//the top faces are the extruded faces
	//void BuildTopBotFaces(Polygon_with_holes_2 InPwh2,vector<vector<int>>& vecTopTri,vector<vector<int>>& vecBotTri);
	//bBottom: true: bottom face;false:top face(triangle vertex orders need to be inverted)
	void BuildTopBotFaces(Polygon_with_holes_2 InPwh2,vector<vector<int>>& vecOutTri,bool bBottom=true);
	//remove the triangles on the faces of one
//	void RemoveFaceTriangles(int iSubSpaceId,GmpPolyhedron GmpPolyhIn,vector<vector<Point_3>> vecvecFacePoint,vector<Point_3>& vecSubPoint,vector<vector<int>>& vecSubSurf);
	void RemoveFaceTriangles(int iSubSpaceId,KW_Mesh PolyhIn,vector<vector<Point_3>> vecvecFacePoint,vector<Point_3>& vecSubPoint,vector<vector<int>>& vecSubSurf);
	//stitch all submeshes together
	void StitchMesh(vector<vector<Point_3>> vecvecSubPoint,	vector<vector<vector<int>>> vecvecSubSurf,KW_Mesh& OutPolyh);
	
	//functions related to sketch-assist topology editing	
	//get selected cylinders in SketchJoin
	//iSSId: id of the subspace that the cylinders in
	//DstPoint: destination point to stretch the first selected cylinder to,computed as the 
	//average of the intersection points of the ray from eye to the last point on UserScrPoint and the second selected cylinder
	bool SJGetSelCylin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport,vector<int>& vecSelCylin,int& iSSId,Point_3& DstPoint);
	//stretch the first selected clinder in SketchJoin
	void SJStretchCylin(KW_Mesh& InOutCylin,Point_3 NewFaceCenter);
	//get selected cylinders in SketchSplit
	//iSSId: id of the subspace that the cylinders in
	bool SSGetSelCylin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport,vector<int>& vecSelCylin,int& iSSId);


	//////////////temporarily unuseful functions/////////////////////////////////////////////////
	//intersect the polyhedron and subspace, and perturb the points on other facets of the subspace mesh
	bool IntersectPolySS(int iFaceId,int iSubSpaceId,KW_Mesh& InOutPolyh);
	//extend the subspace mesh to the opposite direction of the starting face (the face contains the CS) normal,
	//to make the bottom face of the cylinder completely contained in the subspace 
	//(avoid the overlapping of the faces of the subspace and the cylinder);
	//meanwhile,adjust the other faces to make the result of intersection of the cylinder and subspace mesh
	//totally inside the subspace mesh
	void AdjustSSMesh(int iStartFaceId,KW_Mesh InSSMesh,KW_Mesh& OutSSMesh);
	////perturb the positions of an extruded outer boundary of the cylinder, to avoid 
	////that the cross sections on orthogonal planes are covered by the cylinder
	//get the positions of an extruded outer boundary of the cylinder
	//if pertubation is necessary and performed,return true;otherwise,just record the extruded outer boundary and return false
	//bool PerturbOneOutBound(ResortedFace FaceInfo,Polygon_with_holes_3 Pwh3D,Vector_3 HeightVec,vector<Point_3>& NewOutBound,int iSubSpaceId);
	//////////////temporarily unuseful functions/////////////////////////////////////////////////





	//refine and smooth the initial mesh
	void PostProcMesh();
	//get the constraint edges
	bool GetConstraintEdges(vector<Halfedge_handle>& vecConstrEdge,set<Vertex_handle>& vecConstrVertex);
	//put data into Mesh* mesh defined Ctr2SufManager
	void SaveToMesh(vector<Halfedge_handle> vecConstrEdge);
	//refine and smooth
	void RefineSmooth(float RefiAlpha0, float RefiAlphaN, int times, float SmoRatio,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex);
	//liepa refine,exactly the same the the method in Tao Ju's code(mesh.cpp)
	void LiepaRefine(float alpha,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex);
	//pre-compute average edge length for all vertices before refinement
	void PreComputeAveEdgeLen(vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex);
	//edge swaping in mesh refine
	void SwapEdge(vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex);
	//triangle splitting in mesh refine
	//return true if any triangle is splitted
	bool SplitTriangle(float alpha,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex);
	//constrained laplacian smooth
	void ConstrLaplacianSmooth(float fRatio, int iStimes,set<Vertex_handle> setConstrVertex);
	//test
	void TestSmooth(float fRatio, int iStimes,set<Vertex_handle> setConstrVertex);
	//fibermesh smooth
	void FiberMeshSmooth(float SmoRatio,int times);


	//render subspaces(stored in stl instead of array)
	void RenderSubspaceStl();
	//render polygons on faces
	void RenderPolyOnFace();
	//render testpoints
	void RenderTestPoints();
	//render test segments
	void RenderTestSeg();
	//render polyhedrons
	void RenderSinglePolyh();
	void RenderUnionPolyh();
	//render test triangles
	void RenderTestTriangle();
};

/*Convert from vector<GmpPolyhedron> to CGAL GmpPolyhedron*/
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Convert_vecGmpPoly_To_CGALGmpPoly : public CGAL::Modifier_base<HDS> {
public:
	Convert_vecGmpPoly_To_CGALGmpPoly(vector<GmpPolyhedron> vecGmpPolyIn) 
	{	
		vecGmpPoly=vecGmpPolyIn;
	}
	void operator()( HDS& hds) {
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		int iTotalPointSize=0;
		int iTotalFaceSize=0;
		for (unsigned int i=0;i<vecGmpPoly.size();i++)
		{
			iTotalPointSize=iTotalPointSize+vecGmpPoly.at(i).size_of_vertices();
			iTotalFaceSize=iTotalFaceSize+vecGmpPoly.at(i).size_of_facets();
		}
		B.begin_surface(iTotalPointSize,iTotalFaceSize);
		for (unsigned int i=0;i<vecGmpPoly.size();i++)
		{
			for (GmpVertex_iterator VerIter=vecGmpPoly.at(i).vertices_begin();VerIter!=vecGmpPoly.at(i).vertices_end();VerIter++)
			{
				B.add_vertex(VerIter->point());
			}
			
		}
		int iVerBaseInd=0;
		for (unsigned int i=0;i<vecGmpPoly.size();i++)
		{
			for (GmpFacet_iterator FaceIter=vecGmpPoly.at(i).facets_begin();FaceIter!=vecGmpPoly.at(i).facets_end();FaceIter++)
			{
				B.begin_facet();
				GmpHalfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
				do 
				{
					int iVerID=distance(vecGmpPoly.at(i).vertices_begin(),Hafc->vertex());
					B.add_vertex_to_facet(iVerID+iVerBaseInd);
					Hafc++;
				} while(Hafc!=FaceIter->facet_begin());
				B.end_facet();
			}
			iVerBaseInd=iVerBaseInd+vecGmpPoly.at(i).size_of_vertices();
		}
		B.end_surface();
	}
private:
	vector<GmpPolyhedron> vecGmpPoly;
};
/*Convert from vector<GmpPolyhedron> to CGAL GmpPolyhedron*/

/*Convert from array to CGAL GmpPolyhedron*/
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Convert_Array_To_CGALGmpPoly : public CGAL::Modifier_base<HDS> {
public:
	Convert_Array_To_CGALGmpPoly(const vector<Point_3> vecPointIn, const vector<vector<int>> vecFaceIn) 
	{	
		for (unsigned int i=0;i<vecPointIn.size();i++)
		{
			vecPoint.push_back(GmpPoint_3(vecPointIn.at(i).x(),vecPointIn.at(i).y(),vecPointIn.at(i).z()));
		}
		vecFace=vecFaceIn;
	}
	void operator()( HDS& hds) {
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		B.begin_surface(vecPoint.size(),vecFace.size());
		for (unsigned int i=0;i<vecPoint.size();i++)
		{
			B.add_vertex(vecPoint.at(i));
		}
		for (unsigned int i=0;i<vecFace.size();i++)
		{
			B.begin_facet();
			for (int j=0;j<3;j++)
			{
				B.add_vertex_to_facet(vecFace.at(i).at(j));
			}
			B.end_facet();
		}
		B.end_surface();
	}
private:
	vector<GmpPoint_3> vecPoint;
	vector<vector<int>> vecFace;
};
/*Convert from array to CGAL GmpPolyhedron*/

/*Convert from vector<KW_Mesh> to CGAL KW_Mesh*/
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Convert_vecKW_Mesh_To_KW_Mesh : public CGAL::Modifier_base<HDS> {
public:
	Convert_vecKW_Mesh_To_KW_Mesh(vector<KW_Mesh> vecPolyIn) 
	{	
		vecPoly=vecPolyIn;
	}
	void operator()( HDS& hds) {
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		int iTotalPointSize=0;
		int iTotalFaceSize=0;
		for (unsigned int i=0;i<vecPoly.size();i++)
		{
			iTotalPointSize=iTotalPointSize+vecPoly.at(i).size_of_vertices();
			iTotalFaceSize=iTotalFaceSize+vecPoly.at(i).size_of_facets();
		}
		B.begin_surface(iTotalPointSize,iTotalFaceSize);
		for (unsigned int i=0;i<vecPoly.size();i++)
		{
			for (Vertex_iterator VerIter=vecPoly.at(i).vertices_begin();VerIter!=vecPoly.at(i).vertices_end();VerIter++)
			{
				B.add_vertex(VerIter->point());
			}

		}
		int iVerBaseInd=0;
		for (unsigned int i=0;i<vecPoly.size();i++)
		{
			for (Facet_iterator FaceIter=vecPoly.at(i).facets_begin();FaceIter!=vecPoly.at(i).facets_end();FaceIter++)
			{
				B.begin_facet();
				Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
				do 
				{
					int iVerID=distance(vecPoly.at(i).vertices_begin(),Hafc->vertex());
					B.add_vertex_to_facet(iVerID+iVerBaseInd);
					Hafc++;
				} while(Hafc!=FaceIter->facet_begin());
				B.end_facet();
			}
			iVerBaseInd=iVerBaseInd+vecPoly.at(i).size_of_vertices();
		}
		B.end_surface();
	}
private:
	vector<KW_Mesh> vecPoly;
};
/*Convert from vector<KW_Mesh> to CGAL KW_Mesh*/


/*Convert from array to KW_Mesh*/
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Convert_Array_To_KW_Mesh : public CGAL::Modifier_base<HDS> {
public:
	Convert_Array_To_KW_Mesh(const vector<Point_3> vecPointIn, const vector<vector<int>> vecFaceIn) 
	{vecPoint=vecPointIn;vecFace=vecFaceIn;}
	void operator()( HDS& hds) {
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		B.begin_surface(vecPoint.size(),vecFace.size());
		for (unsigned int i=0;i<vecPoint.size();i++)
		{
			B.add_vertex(vecPoint.at(i));
		}
		for (unsigned int i=0;i<vecFace.size();i++)
		{
			B.begin_facet();
			for (unsigned int j=0;j<vecFace.at(i).size();j++)
			{
				B.add_vertex_to_facet(vecFace.at(i).at(j));
			}
			B.end_facet();
		}
		B.end_surface();
	}
private:
	vector<Point_3> vecPoint;
	vector<vector<int>> vecFace;
};
/*Convert from array to KW_Mesh*/

#endif
