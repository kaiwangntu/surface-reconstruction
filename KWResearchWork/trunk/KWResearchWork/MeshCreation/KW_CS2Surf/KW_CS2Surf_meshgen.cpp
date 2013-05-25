#include "StdAfx.h"
#include "KW_CS2Surf.h"
#include "../../CurveDeform.h"
#include "../../Math/DeTriangulator.h"
#include "../../Math/glu_tesselator.h"
#include "../../OBJHandle.h"

//#define  KW_TEST

void KW_CS2Surf::GenInitMesh()
{
	//test
	clock_t   start   =   clock();   

	//#pragma omp parallel 
	//{

	for (int i=0;i<this->iSSspacenum;i++)
	{
		//if empty subspace(combined into other subspaces),do not calculate
		if (this->vecSSspacefacenum.at(i)==0)
		{
			continue;
		}
		vector<Point_3> vecSubPoint;
		vector<vector<int>> vecSubSurf;
		//each face maintain a point vector
		vector<vector<Point_3>> vecvecFacePoint;
		bool bResult=GenSubMesh(i,vecSubPoint,vecSubSurf,vecvecFacePoint);
		if (bResult)
		{
			this->vecvecSubPoint.push_back(vecSubPoint);
			this->vecvecSubSurf.push_back(vecSubSurf);
			this->vecvecvecFacePoint.push_back(vecvecFacePoint);
			this->vecSubSurfSSId.push_back(i);
		}
		//test
		//if (this->vecvecSubPoint.size()==61)
		//if (i==61)
		//{
		//	for (unsigned int itest=0;itest<vecSubSurf.size();itest++)
		//	{
		//		this->vecTestTri.push_back(Triangle_3(vecSubPoint.at(vecSubSurf.at(itest).at(0)),vecSubPoint.at(vecSubSurf.at(itest).at(1)),
		//			vecSubPoint.at(vecSubSurf.at(itest).at(2))));
		//	}
		//	Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecSubPoint,vecSubSurf);
		//	KW_Mesh DirtyPolyh;
		//	DirtyPolyh.delegate(triangle);
		//	std::ofstream outf;
		//	outf.open("dirty.off");
		//	outf<<DirtyPolyh;
		//	outf.close();
		//	break;
		//}
	}

	//}

	//test
	clock_t   endt   =   clock();
	DBWindowWrite("union finished,taking: %d ms\n",endt - start);

	//test
#ifdef KW_TEST
	return;
#endif

	//stitch the submeshes together
	StitchMesh(this->vecvecSubPoint,this->vecvecSubSurf,this->InitPolyh);
}

bool KW_CS2Surf::GenSubMesh(int iSubSpaceId,vector<Point_3>& vecSubPoint,vector<vector<int>>& vecSubSurf,vector<vector<Point_3>>& vecvecFacePoint)
{
	//test
#ifdef KW_TEST
	if (iSubSpaceId!=1)
	{
		return false;
	}
#endif

	DBWindowWrite("build submesh in %d subspace...........................\n",iSubSpaceId);

	//collect the subspace info,to judge if the cylinder is totally inside the subspace and its shrinked version
	vector<vector<Triangle_3>> vecSSFaceTri,vecShrinkSSFaceTri;
	ShrinkCollectSSInfo(this->vecSSMesh.at(iSubSpaceId),vecSSFaceTri,vecShrinkSSFaceTri);

	//if num of faces containing pof ==1, then limit the length of the non-intersected cylinder
	//(faces on the same plane are considered as one)
	//such that the cylinder won't be too long and the result after smoothing is acceptable
	bool bIsolatedCylin=false;
	set<int> setPlnId;
	for (int i=0;i<this->vecSSspacefacenum.at(iSubSpaceId);i++)
	{
		//collect the POF on each face,calculate and fill in the PolyhedronFromPOF info for it
		int iFaceId=this->vecvecSSspace.at(iSubSpaceId).at(i);
		//if the face is on the bounding box or has been removed during subspace combination, don't calculate
		if (this->vecResortFace.at(iFaceId).bBoundaryFace
			|| this->vecResortFace.at(iFaceId).vecFaceVertex.empty())
		{
			continue;
		}
		//if no POF,continue
		if (this->vecPOF.at(iFaceId).PwhList3D.empty())
		{
			continue;
		}
		setPlnId.insert(this->vecResortFace.at(iFaceId).iFacePlaneID);
	}
	if (setPlnId.size()==1)
	{
		bIsolatedCylin=true;
	}

	//polygons who have edges on the bounding edges of the face
	//first: index of face in the subspace
	//second: index of polygon on this face
	vector<PolyhedronFromPOF> vecPFPOF;
	for (int i=0;i<this->vecSSspacefacenum.at(iSubSpaceId);i++)
	{
		//collect the POF on each face,calculate and fill in the PolyhedronFromPOF info for it
		int iFaceId=this->vecvecSSspace.at(iSubSpaceId).at(i);
		//if the face is on the bounding box or has been removed during subspace combination, don't calculate
		if (this->vecResortFace.at(iFaceId).bBoundaryFace
			|| this->vecResortFace.at(iFaceId).vecFaceVertex.empty())
		{
			continue;
		}
		PolyhedronFromPOF currentPFPOF;
		currentPFPOF.iFaceID=iFaceId;
		vector<Point_3> vecFacePoint;
		POFToPFPOF(iFaceId,iSubSpaceId,vecSSFaceTri,vecShrinkSSFaceTri,bIsolatedCylin,currentPFPOF,vecFacePoint);
		//do not record unless this face has curve on
		if (!currentPFPOF.vecPwh3D.empty())
		{
			vecPFPOF.push_back(currentPFPOF);
			vecvecFacePoint.push_back(vecFacePoint);
		}
	}
	//no polyhedrons collected in this subspace
	if (vecPFPOF.empty())
	{
		return false;
	}

	//test
#ifdef KW_TEST
	return false;
#endif

	//GmpPolyhedron GmpResult;
	KW_Mesh ResultMesh;
	ComputeUnionInSubspace(vecPFPOF,ResultMesh);

	//test
	ResultMesh.SetReserved(iSubSpaceId);
	OBJHandle::UnitizeCGALPolyhedron(ResultMesh,false,false);
	this->vecUnionPoly.push_back(ResultMesh);

	//test
	//return false;

	//test
	//std::ofstream outf;
	//outf.open("before.off");
	//outf<<ResultMesh;
	//outf.close();

	RemoveFaceTriangles(iSubSpaceId,ResultMesh,vecvecFacePoint,vecSubPoint,vecSubSurf);

	//test
	//for (unsigned int i=0;i<vecSubSurf.size();i++)
	//{
	//	vector<Point_3> vecTriPoint;
	//	for (unsigned int j=0;j<vecSubSurf.at(i).size();j++)
	//	{
	//		vecTriPoint.push_back(vecSubPoint.at(vecSubSurf.at(i).at(j)));
	//	}
	//	this->vecTestTri.push_back(Triangle_3(vecTriPoint.at(0),vecTriPoint.at(1),vecTriPoint.at(2)));
	//}

	//test
	//Convert_Array_To_CGALGmpPoly<GmpHalfedgeDS> testtriangle(vecSubPoint,vecSubSurf);
	//GmpPolyhedron gmptest;
	//gmptest.delegate(testtriangle);
	////test
	//for (GmpVertex_iterator VerIter=gmptest.vertices_begin();VerIter!=gmptest.vertices_end();VerIter++)
	//{
	//	if (VerIter->vertex_degree()==0)
	//	{
	//		DBWindowWrite("degree 0 vertex found!!\n");
	//	}
	//}

	return true;
}

void KW_CS2Surf::ShrinkCollectSSInfo(KW_Mesh& SSInOut,vector<vector<Triangle_3>>& vecSSFaceTri,vector<vector<Triangle_3>>& vecShrinkSSFaceTri)
{
	double dShrinkDist=5;
	//back up the shrinked vertex positions
	for (Vertex_iterator VerIter=SSInOut.vertices_begin();VerIter!=SSInOut.vertices_end();VerIter++)
	{
		VerIter->SetLRNewPos(VerIter->point()-VerIter->normal()*dShrinkDist);
	}

	//collect triangles on different planes
	vector<int> vecFaceId;
	for (Facet_iterator FaceIter=SSInOut.facets_begin();FaceIter!=SSInOut.facets_end();FaceIter++)
	{
		Vertex_handle Ver0=FaceIter->halfedge()->vertex();
		Vertex_handle Ver1=FaceIter->halfedge()->next()->vertex();
		Vertex_handle Ver2=FaceIter->halfedge()->next()->next()->vertex();

		Triangle_3 FaceTri(Ver0->point(),Ver1->point(),Ver2->point());
		//Triangle_3 ShrinkFaceTri(Ver0->point()-Ver0->normal()*dShrinkDist,Ver1->point()-Ver1->normal()*dShrinkDist,
		//	Ver2->point()-Ver2->normal()*dShrinkDist);
		Triangle_3 ShrinkFaceTri(Ver0->GetLRNewPos(),Ver1->GetLRNewPos(),Ver2->GetLRNewPos());

		vector<int>::iterator pFind=find(vecFaceId.begin(),vecFaceId.end(),FaceIter->GetReserved());
		if (pFind==vecFaceId.end())
		{
			vecFaceId.push_back(FaceIter->GetReserved());
			vector<Triangle_3> NewVecFaceTri,NewVecShrinkFaceTri;
			NewVecFaceTri.push_back(FaceTri);
			NewVecShrinkFaceTri.push_back(ShrinkFaceTri);
			vecSSFaceTri.push_back(NewVecFaceTri);
			vecShrinkSSFaceTri.push_back(NewVecShrinkFaceTri);
		}
		else
		{
			int iIndex=distance(vecFaceId.begin(),pFind);
			vecSSFaceTri.at(iIndex).push_back(FaceTri);
			vecShrinkSSFaceTri.at(iIndex).push_back(ShrinkFaceTri);
		}
	}
}

void KW_CS2Surf::POFToPFPOF(int iFaceId,int iSubSpaceId,vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri,
							bool bIsolatedCylin, PolyhedronFromPOF& InOutPFPOF,vector<Point_3>& vecFacePoint)
{
	ResortedFace currentFaceInfo=this->vecResortFace.at(iFaceId);
	//decide the height vector pointing to the inside of the subspace
	Vector_3 HeightVec=CGAL::NULL_VECTOR;
	if (iSubSpaceId==currentFaceInfo.vecSubspaceId.front())
	{
		HeightVec=currentFaceInfo.vecHeightVect.front();
	}
	else if (iSubSpaceId==currentFaceInfo.vecSubspaceId.back())
	{
		HeightVec=currentFaceInfo.vecHeightVect.back();
	}
	//collect the polyhedrons
	//quite time-consuming when terminating the program in debug mode
	Pwh_list_2::iterator PwhList2Iter=this->vecPOF.at(iFaceId).PwhList2D.begin();
	for (Pwh_list_3::iterator PwhList3Iter=this->vecPOF.at(iFaceId).PwhList3D.begin();
		PwhList3Iter!=this->vecPOF.at(iFaceId).PwhList3D.end();PwhList3Iter++,PwhList2Iter++)
	{
		Polygon_with_holes_3 currentPwh3=*PwhList3Iter;
		Polygon_with_holes_2 currentPwh2=*PwhList2Iter;
		//////////////////////////////////////////////////////////////////
		//lower the precision&record these points
		//test
		//FILE* pFile=fopen("bound.txt","w");
		for (unsigned int j=0;j<currentPwh3.outer_boundary.size();j++)
		{
			//float fX=currentPwh3.outer_boundary.at(j).x();
			//float fY=currentPwh3.outer_boundary.at(j).y();
			//float fZ=currentPwh3.outer_boundary.at(j).z();
			//currentPwh3.outer_boundary.at(j)=Point_3(fX,fY,fZ);
			vecFacePoint.push_back(currentPwh3.outer_boundary.at(j));
			//fprintf(pFile,"%f %f %f\n",currentPwh3.outer_boundary.at(j).x(),currentPwh3.outer_boundary.at(j).y(),
			//	currentPwh3.outer_boundary.at(j).z());
		}
		//fclose(pFile);
		for (unsigned int j=0;j<currentPwh3.inner_hole.size();j++)
		{
			for (unsigned int k=0;k<currentPwh3.inner_hole.at(j).size();k++)
			{
				//float fX=currentPwh3.inner_hole.at(j).at(k).x();
				//float fY=currentPwh3.inner_hole.at(j).at(k).y();
				//float fZ=currentPwh3.inner_hole.at(j).at(k).z();
				//currentPwh3.inner_hole.at(j).at(k)=Point_3(fX,fY,fZ);
				vecFacePoint.push_back(currentPwh3.inner_hole.at(j).at(k));
			}
		}
		
		//////////////////////////////////////////////////////////////////
		//perturb the outer boundary of the extruded part
		//vector<Point_3> PertOutBnd;
		//bool bPert=PerturbOneOutBound(currentFaceInfo,currentPwh3,HeightVec,PertOutBnd,iSubSpaceId);
		//PerturbOneOutBound(currentFaceInfo,currentPwh3,HeightVec,PertOutBnd,iSubSpaceId);
		Polygon_with_holes_3 OutBnd;
		Point_3 ExtruCenter;
		bool bPert=GetOutBound(iSubSpaceId,currentFaceInfo,iFaceId,vecSSFaceTri,vecShrinkSSFaceTri,bIsolatedCylin,currentPwh3,HeightVec,OutBnd,ExtruCenter);

		//test
#ifdef KW_TEST
//		return;
#endif
		//if (OutBnd.outer_boundary.empty())
		//{
		//	return;
		//}
		//continue;

		//get the 2d version of the extruded face,for computing the triangulation(its triangulation result may be different
		//from that of the POF)
		Plane_3 plane=this->vecTempCN.at(currentFaceInfo.iFacePlaneID).plane;
		Polygon_with_holes_2 OutBnd2d;
		Polygon_2 OutBnd2dOutBoundary;
		GeometryAlgorithm::PlanarPolygonToXOY(OutBnd.outer_boundary,OutBnd2dOutBoundary,plane);
		OutBnd2d.outer_boundary()=OutBnd2dOutBoundary;
		for (unsigned int i=0;i<OutBnd.inner_hole.size();i++)
		{
			Polygon_2 OutBnd2dInnerHole;
			GeometryAlgorithm::PlanarPolygonToXOY(OutBnd.inner_hole.at(i),OutBnd2dInnerHole,plane);
			OutBnd2d.add_hole(OutBnd2dInnerHole);
		}

		//build cylinder
		KW_Mesh OutPolyh;
		GetOnePolyhFromPwh3(currentPwh3,currentPwh2,OutBnd,OutBnd2d,HeightVec,OutPolyh);
		//record the id of the subspace of this cylinder
		OutPolyh.SetReserved(iSubSpaceId);
		//record the center of the extruded POF
		OutPolyh.SetExtruCenter(ExtruCenter);
		//record in InOutPFPOF
		InOutPFPOF.vecPwh3D.push_back(currentPwh3);
		//InOutPFPOF.vecPwh2D.push_back(currentPwh2);
		InOutPFPOF.vecPwhCylinder.push_back(OutPolyh);

		//test
		OBJHandle::UnitizeCGALPolyhedron(OutPolyh,false,false);
		this->vecSinglePoly.push_back(OutPolyh);
	}
}

bool KW_CS2Surf::GetOutBound(int iSubSpaceId,ResortedFace FaceInfo,int iFaceId,vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri, 
							 bool bIsolatedCylin,Polygon_with_holes_3 Pwh3DIn,Vector_3 HeightVec,Polygon_with_holes_3& Pwh3DOut,Point_3& ExtruCenter)
{
	//test
#ifdef KW_TEST
	//if (iFaceId!=33)
	//{
	//	return false;
	//}
#endif

	double NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=2.1;//1.4;//1.2;//1;//0.5
	double INTSC_POLY_EXTRU_HEIGHT=200;

	//get the center of the CS
	Point_3 CSCenter=CGAL::centroid(Pwh3DIn.outer_boundary.begin(),Pwh3DIn.outer_boundary.end());
	int iLenStep=10;//10;
	int iScaleStep=10;//10;//5;

	////////////kw debug test
	if (this->CtrName=="teacup.contour")
	{
		//for teacup example
		if (iSubSpaceId==1)
		{
			if (iFaceId==19)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.1;
			}
		}
		else if (iSubSpaceId==3)
		{
			if (iFaceId==19)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
		else if (iSubSpaceId==14)
		{
			if (iFaceId==71)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.1;
			}
		}
		else if (iSubSpaceId==15)
		{
			if (iFaceId==71)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
		else if (iSubSpaceId==26)
		{
			NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.1;
		}
	}
	else if (this->CtrName=="BE.contour")
	{
		//only for B->E example
		if (iSubSpaceId==0)
		{
			if (iFaceId==10)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
			else if (iFaceId==34)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
		}
		else if (iSubSpaceId==1)
		{
			if (iFaceId==35)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.6;
			}
			else if (iFaceId==10)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.8;//0.3
			}
			else if (iFaceId==15)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=1.4;
			}
		}
		else if (iSubSpaceId==3)
		{
			if (iFaceId==20)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
			else if (iFaceId==37)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
		}
		else if (iSubSpaceId==4)
		{
			if (iFaceId==34)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
			else if (iFaceId==27)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
		else if (iSubSpaceId==5)
		{
			if (iFaceId==35)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.55;
			}
		}
		else if (iSubSpaceId==7)
		{
			if (iFaceId==37)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
			else if (iFaceId==33)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
	}
	else if (this->CtrName=="FP.contour")
	{
		//only for f->p example
		if (iSubSpaceId==0)
		{
			if (iFaceId==10)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
			else if (iFaceId==34)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
		}
		else if (iSubSpaceId==1)
		{
			if (iFaceId==35)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.6;//0.7
			}
			else if (iFaceId==15)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.8;
			}
			else 
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
		else if (iSubSpaceId==4)
		{
			if (iFaceId==34)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.7;
			}
			else if (iFaceId==27)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.3;
			}
		}
		else if (iSubSpaceId==5)
		{
			if (iFaceId==35)
			{
				NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.5;//0.4
			}
		}
		else if (iSubSpaceId==7 || iSubSpaceId==3)
		{
			NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=0.8;//0.4
		}
	}
	////////////kw debug test




	if (Pwh3DIn.bIntersectSSFace)
	{
		DBWindowWrite("Pwh intersects face...\n");
		//collect the id of neighbor faces first
		set<int> setNbFaceId;
		for (Facet_iterator FaceIter=this->vecSSMesh.at(iSubSpaceId).facets_begin();FaceIter!=this->vecSSMesh.at(iSubSpaceId).facets_end();FaceIter++)
		{
			if (FaceIter->GetReserved()==iFaceId)
			{
				Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
				do 
				{
					if (Hafc->opposite()->facet()->GetReserved()!=iFaceId)
					{
						//also consider if the original Pwh intersects this face
						//if not,proper cylinder can be obtained by shrinking the height first


						//also consider the dihedral angle between two planes
						//if quite small,the cylinder needs to be shrinked first
						Vector_3 Norm0=-FaceIter->normal();
						Vector_3 Norm1=-Hafc->opposite()->facet()->normal();
						double dAngle=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(Norm0,Norm1);
						//test
						//if (dAngle<160)
						//{
							setNbFaceId.insert(Hafc->opposite()->facet()->GetReserved());
						//}
					}
					Hafc++;
				} while(Hafc!=FaceIter->facet_begin());
			}
		}

		//shrink the length of the cylinder
		for (int iLengh=iLenStep;iLengh>0;iLengh--)
		{
			HeightVec=HeightVec/sqrt(HeightVec.squared_length());
			HeightVec=HeightVec*INTSC_POLY_EXTRU_HEIGHT;//150;//600;//900;//1000;
			Point_3 TransCenter=CSCenter+HeightVec*(double)iLengh/(double)iLenStep;
			//if the center of the extruded pwh is outside the ss,then shrink the length without further adjustment
			if (!JudgePointInsideSSStrict(TransCenter,vecShrinkSSFaceTri))
			{
				continue;
			}
			//compute the translated&scaled extruded Pwh 
			Polygon_with_holes_3 ExtrudePwh=Pwh3DIn;

			//test
#ifdef KW_TEST
			//this->vecTestPoint=Pwh3DIn.outer_boundary;
			//this->vecTestPoint.push_back(TransCenter);
#endif

			//set the flags for all points
			for (unsigned int i=0;i<ExtrudePwh.outer_boundary.size();i++)
			{
				ExtrudePwh.outer_bound_flag.push_back(CGAL::ON_UNBOUNDED_SIDE);
			}
			for (unsigned int i=0;i<ExtrudePwh.inner_hole.size();i++)
			{
				vector<int> vecCurrFlag;
				for (unsigned int j=0;j<ExtrudePwh.inner_hole.at(i).size();j++)
				{
					vecCurrFlag.push_back(CGAL::ON_UNBOUNDED_SIDE);
				}
				ExtrudePwh.inner_hole_flag.push_back(vecCurrFlag);
			}

			//extrude all boundary points first
			for (unsigned int i=0;i<Pwh3DIn.outer_boundary.size();i++)
			{
				Point_3 TransPoint=Pwh3DIn.outer_boundary.at(i)+HeightVec*(double)iLengh/(double)iLenStep;
				ExtrudePwh.outer_boundary.at(i)=TransPoint;
			}
			bool bInside=CheckCylinderInsideSS(vecSSFaceTri,vecShrinkSSFaceTri,ExtrudePwh);
			//if the extruded points are all inside the subspace,continue with the holes;
			//otherwise,compute the new extruded points
			if (!bInside)
			{
				//check if the segment connecting the TransCenter&extruded boundary point intersects with
				//non-neighbor facets of iFaceId,if yes, shrink the length,until all these segments intersect
				//only with the neighbor faces
				bool bIntSctNonNb=false;
				//vector(with max distance to intersected edge line) from the outside point to the intersected face
				vector<Vector_3> vecMaxToFaceVect;
				for (unsigned int i=0;i<setNbFaceId.size();i++)
				{
					vecMaxToFaceVect.push_back(CGAL::NULL_VECTOR);
				}
				for (unsigned int i=0;i<ExtrudePwh.outer_boundary.size();i++)
				{
					//only judge the point outside the subspace
					if (ExtrudePwh.outer_bound_flag.at(i)==CGAL::ON_BOUNDED_SIDE)
					{
						continue;
					}
					else
					{
						//get the intersection point between the segment connecting the extruded point and center & the subspace
						//get the vectors with max distances from the outside points to each intersected neighbor face
						Segment_3 ConnectSeg(ExtrudePwh.outer_boundary.at(i),TransCenter);
						for (Facet_iterator FaceIter=this->vecSSMesh.at(iSubSpaceId).facets_begin();FaceIter!=this->vecSSMesh.at(iSubSpaceId).facets_end();FaceIter++)
						{
							//ignore the current face itself
							if (FaceIter->GetReserved()==iFaceId)
							{
								continue;
							}
							Triangle_3 currTri(FaceIter->halfedge()->vertex()->GetLRNewPos(),FaceIter->halfedge()->next()->vertex()->GetLRNewPos(),
								FaceIter->halfedge()->next()->next()->vertex()->GetLRNewPos());
							if (GeometryAlgorithm::CGAL_do_intersect(ConnectSeg,currTri))
							{
								set<int>::iterator pFind=setNbFaceId.find(FaceIter->GetReserved());
								if (pFind==setNbFaceId.end())
								{
									bIntSctNonNb=true;
									break;
								}
								else
								{
									//plane that the extruded boundary points lie on
									Plane_3 ExtrudedFacePlane(TransCenter,HeightVec*(double)iLengh/(double)iLenStep);
									//compute the intersection lines between the plane of the extruded points and the neighbor faces
									CGAL::Object result = CGAL::intersection(ExtrudedFacePlane,currTri);
									Segment_3 SegResult;
									//Point_3 TempPnt;Triangle_3 TempTri;
									assert(CGAL::assign(SegResult, result));
									Line_3 IntSctLine=SegResult.supporting_line();
									Point_3 ProjPoint=IntSctLine.projection(ExtrudePwh.outer_boundary.at(i));
									double dDist=sqrt(CGAL::squared_distance(ExtrudePwh.outer_boundary.at(i),ProjPoint));
									int iNbFaceIndex=distance(setNbFaceId.begin(),pFind);
									double dSavedVectLen=sqrt(vecMaxToFaceVect.at(iNbFaceIndex).squared_length());
									if (dDist>dSavedVectLen)
									{
										Vector_3 TempVect(ExtrudePwh.outer_boundary.at(i),ProjPoint);
										TempVect=TempVect/sqrt(TempVect.squared_length());
										vecMaxToFaceVect.at(iNbFaceIndex)=TempVect*dDist;
									}
								}
							}
						}
					}
					if (bIntSctNonNb)
					{
						break;
					}
				}
				if (bIntSctNonNb)//continue to shrink the cylinder length
				{
					continue;
				}
				else
				{
					//iteratively move each boundary point along the composite vector, 
					//if the num of points outside the ss increases,then break & put all outside points inwards by scaling
					Vector_3 CompVect=CGAL::NULL_VECTOR;
					for (unsigned int i=0;i<vecMaxToFaceVect.size();i++)
					{
						CompVect=CompVect+vecMaxToFaceVect.at(i);
					}
					int iLastOutPoinNum=ExtrudePwh.outer_boundary.size();
					vector<Point_3> vecLastPointPos=ExtrudePwh.outer_boundary;
					vector<int> vecLastInOutflag=ExtrudePwh.outer_bound_flag;
					int iMoveStep=5;
					for (int iMove=0;iMove<=iMoveStep;iMove++)
					{
						int iCurrOutPointNum=0;
						for (unsigned int i=0;i<ExtrudePwh.outer_boundary.size();i++)
						{
							ExtrudePwh.outer_boundary.at(i)=ExtrudePwh.outer_boundary.at(i)+CompVect*(double)iMove/(double)iMoveStep;
							if (JudgePointInsideSS(ExtrudePwh.outer_boundary.at(i),vecShrinkSSFaceTri))
							{
								ExtrudePwh.outer_bound_flag.at(i)=CGAL::ON_BOUNDED_SIDE;
							}
							else
							{
								ExtrudePwh.outer_bound_flag.at(i)=CGAL::ON_UNBOUNDED_SIDE;
								iCurrOutPointNum++;
							}
						}
						//make sure the extruded center is strictly inside the subspace
						Point_3 NewTransCenter=CGAL::centroid(ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end());
						bool bCtrInSS=JudgePointInsideSSStrict(NewTransCenter,vecShrinkSSFaceTri);
#ifdef KW_TEST
//	this->vecTestPoint.push_back(NewTransCenter);
#endif

						if (iCurrOutPointNum>iLastOutPoinNum || iLastOutPoinNum==0 || !bCtrInSS)
						{
							ExtrudePwh.outer_boundary=vecLastPointPos;
							ExtrudePwh.outer_bound_flag=vecLastInOutflag;
							break;
						}
						else
						{
							iLastOutPoinNum=iCurrOutPointNum;
							vecLastPointPos=ExtrudePwh.outer_boundary;
							vecLastInOutflag=ExtrudePwh.outer_bound_flag;
						}
					}
#ifdef KW_TEST
	//this->vecTestPoint.insert(this->vecTestPoint.end(),ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end());
	//this->vecTestPoint.push_back(CGAL::centroid(ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end()));
#endif

					//only scale points outside the ss
					for (unsigned int i=0;i<ExtrudePwh.outer_boundary.size();i++)
					{
						//if the boundary point is already inside,keep fixed
						if (ExtrudePwh.outer_bound_flag.at(i)==CGAL::ON_BOUNDED_SIDE)
						{
							continue;
						}
						//else, move towards the extruded center
						else
						{
							//scale
							Vector_3 VectPointToCsCenter=CSCenter-Pwh3DIn.outer_boundary.at(i);
							for (int iScale=0;iScale<iScaleStep;iScale++)
							{
								Point_3 ScalePoint=ExtrudePwh.outer_boundary.at(i)+VectPointToCsCenter*(float)iScale/(float)iScaleStep;
								if (JudgePointInsideSS(ScalePoint,vecShrinkSSFaceTri))
								{
									ExtrudePwh.outer_boundary.at(i)=ScalePoint;
									ExtrudePwh.outer_bound_flag.at(i)=CGAL::ON_BOUNDED_SIDE;
									break;
								}
#ifdef KW_TEST
//	this->vecTestPoint.push_back(ScalePoint);
#endif
							}
						}
					}
					//test
					assert(CheckCylinderInsideSS(vecSSFaceTri,vecShrinkSSFaceTri,ExtrudePwh));
				}
			}

			//this->vecTestPoint.insert(this->vecTestPoint.end(),ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end());

			//translate the holes towards the center if they lie outside the subspace
			//if not succeed, rescale the radius & shrink the length
			bool bInnerHoleDone=true;
			for (int iScale=0;iScale<iScaleStep;iScale++)
			{
				for (unsigned int i=0;i<Pwh3DIn.inner_hole.size();i++)
				{
					bInnerHoleDone=false;
					//move each inner hole towards the center of the cs
					Point_3 InnerHoleCenter=CGAL::centroid(Pwh3DIn.inner_hole.at(i).begin(),Pwh3DIn.inner_hole.at(i).end());
					Vector_3 HoleCenterToCSCenter=CSCenter-InnerHoleCenter;
					for (unsigned int j=0;j<Pwh3DIn.inner_hole.at(i).size();j++)
					{
						//scale
						//Vector_3 PointToCsCenterVect=CSCenter-Pwh3DIn.inner_hole.at(i).at(j);
						//Point_3 ScalePoint=Pwh3DIn.inner_hole.at(i).at(j)+PointToCsCenterVect*iScale/(float)iScaleStep;
						Point_3 ScalePoint=Pwh3DIn.inner_hole.at(i).at(j)+HoleCenterToCSCenter*iScale/(float)iScaleStep;
						//translate
						Point_3 TransPoint=ScalePoint;//+CstoSSCenterVect*(double)iCenter/(double)iMoveStep;
						//extrude
						HeightVec=HeightVec/sqrt(HeightVec.squared_length());
						HeightVec=HeightVec*INTSC_POLY_EXTRU_HEIGHT;//150;//600;//900;//1000;
						TransPoint=TransPoint+HeightVec*(double)iLengh/(double)iLenStep;//*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO;
						ExtrudePwh.inner_hole.at(i).at(j)=TransPoint;
					}
				}
				//check intersection
				bool bInside=CheckCylinderInsideSS(vecSSFaceTri,vecShrinkSSFaceTri,ExtrudePwh);
				if (bInside)
				{
					bInnerHoleDone=true;
					break;
				}
			}
			if (bInnerHoleDone==true)
			{
				Pwh3DOut=ExtrudePwh;
				break;
			}
		}
	}
	else
	{
		DBWindowWrite("Pwh does NOT intersect face...\n");
		
		//if isolated cylidners, then limit the length
		if (bIsolatedCylin)
		{
			DBWindowWrite("isolated cylinder...\n");
			HeightVec=HeightVec/sqrt(HeightVec.squared_length());
			HeightVec=HeightVec*600;
		}

#ifdef KW_TEST
//		this->vecTestPoint.push_back(CSCenter);
#endif

		//shrink the length of the cylinder
		for (int iLengh=iLenStep;iLengh>0;iLengh--)
		{
			//if the center of the extruded pwh is outside the ss,then shrink the length without scaling
			Point_3 TransCenter=CSCenter+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*(double)iLengh/(double)iLenStep;

#ifdef KW_TEST
//			this->vecTestPoint.push_back(TransCenter);
#endif

			//if (!JudgePointInsideSS(TransCenter,vecShrinkSSFaceTri))
			if (!JudgePointInsideSSStrict(TransCenter,vecShrinkSSFaceTri))
			{
				continue;
			}
			//compute the translated&scaled extruded Pwh 
			Polygon_with_holes_3 ExtrudePwh=Pwh3DIn;
			//set the flags for all points
			for (unsigned int i=0;i<ExtrudePwh.outer_boundary.size();i++)
			{
				ExtrudePwh.outer_bound_flag.push_back(CGAL::ON_UNBOUNDED_SIDE);
			}
			for (unsigned int i=0;i<ExtrudePwh.inner_hole.size();i++)
			{
				vector<int> vecCurrFlag;
				for (unsigned int j=0;j<ExtrudePwh.inner_hole.at(i).size();j++)
				{
					vecCurrFlag.push_back(CGAL::ON_UNBOUNDED_SIDE);
				}
				ExtrudePwh.inner_hole_flag.push_back(vecCurrFlag);
			}

			//rescale the radius until all vertices are inside the subspace
			//for (int iScale=0;iScale<iScaleStep;iScale++)
			for (int iScale=0;iScale<iScaleStep/2;iScale++)
			{
				for (unsigned int i=0;i<Pwh3DIn.outer_boundary.size();i++)
				{
					//scale
					Vector_3 PointToCsCenterVect=CSCenter-Pwh3DIn.outer_boundary.at(i);
					Point_3 ScalePoint=Pwh3DIn.outer_boundary.at(i)+PointToCsCenterVect*(float)iScale/(float)iScaleStep;
					//test see
					//DBWindowWrite("length of vector to shrink: %f\n",sqrt(PointToCsCenterVect.squared_length())*(float)iScale/(float)iScaleStep);
					//translate
					Point_3 TransPoint=ScalePoint;//+CstoSSCenterVect*(double)iCenter/(double)iMoveStep;
					TransPoint=TransPoint+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*(double)iLengh/(double)iLenStep;//NON_INTSC_POLY_EXTRU_HEIGHT_RATIO;
					ExtrudePwh.outer_boundary.at(i)=TransPoint;
				}
				for (unsigned int i=0;i<Pwh3DIn.inner_hole.size();i++)
				{
					//move each inner hole towards the center of the cs
					Point_3 InnerHoleCenter=CGAL::centroid(Pwh3DIn.inner_hole.at(i).begin(),Pwh3DIn.inner_hole.at(i).end());
					Vector_3 HoleCenterToCSCenter=CSCenter-InnerHoleCenter;
					for (unsigned int j=0;j<Pwh3DIn.inner_hole.at(i).size();j++)
					{
						//scale
						//Vector_3 PointToCsCenterVect=CSCenter-Pwh3DIn.inner_hole.at(i).at(j);
						//Point_3 ScalePoint=Pwh3DIn.inner_hole.at(i).at(j)+PointToCsCenterVect*iScale/(float)iScaleStep;
						Point_3 ScalePoint=Pwh3DIn.inner_hole.at(i).at(j)+HoleCenterToCSCenter*iScale/(float)iScaleStep;
						//translate
						Point_3 TransPoint=ScalePoint;//+CstoSSCenterVect*(double)iCenter/(double)iMoveStep;
						//extrude
						TransPoint=TransPoint+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*(double)iLengh/(double)iLenStep;//*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO;
						ExtrudePwh.inner_hole.at(i).at(j)=TransPoint;
					}
				}
				//check intersection
				bool bInside=CheckCylinderInsideSS(vecSSFaceTri,vecShrinkSSFaceTri,ExtrudePwh);
				//test
				//if (iScale==1)
				//{
				//	this->vecTestPoint.insert(this->vecTestPoint.end(),ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end());
				//	this->vecTestPoint.push_back(CSCenter);
				//	break;
				//}
				if (bInside)
				{
					Pwh3DOut=ExtrudePwh;
					break;
				}
			}
			if (!Pwh3DOut.outer_boundary.empty())
			{
				break;
			}
		}
	}

	//test
	//assert(!Pwh3DOut.outer_boundary.empty());

	if (!Pwh3DOut.outer_boundary.empty())
	{
		ExtruCenter=CGAL::centroid(Pwh3DOut.outer_boundary.begin(),Pwh3DOut.outer_boundary.end());
	}

	//test
	//this->vecTestPoint.insert(this->vecTestPoint.end(),Pwh3DIn.outer_boundary.begin(),Pwh3DIn.outer_boundary.end());
	////this->vecTestPoint.push_back(CSCenter);
	//for (unsigned int i=0;i<Pwh3DOut.outer_boundary.size();i++)
	//{
	//	this->vecTestPoint.push_back(Pwh3DOut.outer_boundary.at(i));
	//}

	if (Pwh3DIn.bIntersectSSFace)
	{
		return true;
	}
	return false;
}

bool KW_CS2Surf::CheckCylinderInsideSS(vector<vector<Triangle_3>> vecSSFaceTri,vector<vector<Triangle_3>> vecShrinkSSFaceTri,Polygon_with_holes_3& PwhExtru)
{
	bool bAllInside=true;

	for (unsigned int i=0;i<PwhExtru.outer_boundary.size();i++)
	{
		if (PwhExtru.outer_bound_flag.at(i)==CGAL::ON_BOUNDED_SIDE)//inside ss,do not check
		{
			continue;
		}
		else
		{
			if (//JudgePointInsideSS(PwhExtru.outer_boundary.at(i),vecSSFaceTri) &&
				JudgePointInsideSS(PwhExtru.outer_boundary.at(i),vecShrinkSSFaceTri))
			{
				PwhExtru.outer_bound_flag.at(i)=CGAL::ON_BOUNDED_SIDE;
			}
			else
			{
				bAllInside=false;
			}
		}
	}
	for (unsigned int i=0;i<PwhExtru.inner_hole.size();i++)
	{
		for (unsigned int j=0;j<PwhExtru.inner_hole.at(i).size();j++)
		{
			if (PwhExtru.inner_hole_flag.at(i).at(j)==CGAL::ON_BOUNDED_SIDE)
			{
				continue;
			}
			else
			{
				if (//JudgePointInsideSS(PwhExtru.inner_hole.at(i).at(j),vecSSFaceTri) && 
					JudgePointInsideSS(PwhExtru.outer_boundary.at(i),vecShrinkSSFaceTri))
				{
					PwhExtru.inner_hole_flag.at(i).at(j)=CGAL::ON_BOUNDED_SIDE;
				}
				else
				{
					bAllInside=false;
				}
			}
		}
	}
	return bAllInside;
}

bool KW_CS2Surf::JudgePointInsideSS(Point_3 PointToJudge,vector<vector<Triangle_3>> vecSSFaceTri)
{
	//first, judge if a point is inside the subspace
	//get an arbitrary point
	Point_3 ArbitPoint=CGAL::ORIGIN;
	if (PointToJudge==ArbitPoint)
	{
		ArbitPoint=Point_3(0,0,1);
	}
	Ray_3 ArbitRay(PointToJudge,ArbitPoint);

	//compute the total intersection point between the ray and subspace
	int iTotalPoint=0;
	for (unsigned int i=0;i<vecSSFaceTri.size();i++)
	{
		for (unsigned int j=0;j<vecSSFaceTri.at(i).size();j++)
		{
			if (GeometryAlgorithm::CGAL_do_intersect(ArbitRay,vecSSFaceTri.at(i).at(j)))
			{
				iTotalPoint++;
				break;
			}
		}
	}
	//even number,outside
	if (iTotalPoint%2==0)
	{
		return false;
	}
	return true;
}

bool KW_CS2Surf::JudgePointInsideSSStrict(Point_3 PointToJudge,vector<vector<Triangle_3>> vecSSFaceTri)
{
	//first, judge inside or not
	if (!JudgePointInsideSS(PointToJudge,vecSSFaceTri))
	{
		return false;
	}
	//then, judge the distance
	double dThreDist=5;
	for (unsigned int i=0;i<vecSSFaceTri.size();i++)
	{
		//project the point onto the plane first
		Plane_3 TriPlane=vecSSFaceTri.at(i).front().supporting_plane();
		Point_3 ProjPoint=TriPlane.projection(PointToJudge);
		Vector_3 ConnectVect(PointToJudge,ProjPoint);
		//extend the projection point a little bit to avoid numerical issue
		Vector_3 NormalizedVect=ConnectVect/sqrt(ConnectVect.squared_length());
		Point_3 ExtPoint=PointToJudge+ConnectVect+NormalizedVect*dThreDist;
		Segment_3 JudgeSeg(PointToJudge,ExtPoint);
		double dPointToProjDist=CGAL::squared_distance(PointToJudge,ProjPoint);
		if (dPointToProjDist<50)
		{
			for (unsigned int j=0;j<vecSSFaceTri.at(i).size();j++)
			{
				if (GeometryAlgorithm::CGAL_do_intersect(JudgeSeg,vecSSFaceTri.at(i).at(j)))
				{
					return false;
				}
			}
		}
	}
	return true;
}

//bool KW_CS2Surf::GetOutBound(int iSubSpaceId,ResortedFace FaceInfo,int iFaceId,Polygon_with_holes_3 Pwh3DIn,Vector_3 HeightVec,Polygon_with_holes_3& Pwh3DOut,Point_3& ExtruCenter)
//{
//	float INTSC_POLY_EXTRU_HEIGHT_RATIO=0.25;//0.5;//0.125
//	float NON_INTSC_POLY_EXTRU_HEIGHT_RATIO=2.1;//1.4;//1.2;//1;//0.5
//
//	//get edges of the face,and the corresponding width vector of them
//	vector<Segment_3> vecFaceEdge;
//	for (unsigned int i=0;i<FaceInfo.vecFaceVertex.size();i++)
//	{
//		Segment_3 currentSeg(FaceInfo.vecFaceVertex.at(i),FaceInfo.vecFaceVertex.at((i+1)%FaceInfo.vecFaceVertex.size()));
//		vecFaceEdge.push_back(currentSeg);
//	}
//
//	//get the center of the SS face
//	Point_3 SSFaceCenter=CGAL::centroid(FaceInfo.vecFaceVertex.begin(),FaceInfo.vecFaceVertex.end());
//	//get the center of the CS
//	Point_3 CSCenter=CGAL::centroid(Pwh3DIn.outer_boundary.begin(),Pwh3DIn.outer_boundary.end());
//	Vector_3 CstoSSCenterVect=SSFaceCenter-CSCenter;
//	const int iLenStep=10;
//	const int iMoveStep=5;
//	const int iScaleStep=5;
//	//shrink the length of the cylinder
//	for (int iLengh=iLenStep;iLengh>0;iLengh--)
//	{
//		//translate the center
//		for (int iCenter=0;iCenter<=iMoveStep;iCenter++)
//		{
//			//compute the translated&scaled extruded Pwh 
//			Polygon_with_holes_3 ExtrudePwh;
//			//rescale the radius
//			for (int iScale=0;iScale<iScaleStep;iScale++)
//			{
//				ExtrudePwh.outer_boundary.clear();
//				ExtrudePwh.inner_hole.clear();
//				for (unsigned int i=0;i<Pwh3DIn.outer_boundary.size();i++)
//				{
//					//scale
//					Vector_3 PointToCsCenterVect=CSCenter-Pwh3DIn.outer_boundary.at(i);
//					Point_3 ScalePoint=Pwh3DIn.outer_boundary.at(i)+PointToCsCenterVect*(float)iScale/(float)iScaleStep;
//					//translate
//					Point_3 TransPoint=ScalePoint+CstoSSCenterVect*(double)iCenter/(double)iMoveStep;
//					//extrude
//					if (Pwh3DIn.bIntersectSSFace)//input cs intersects with the ss face
//					{
//						//test
//						HeightVec=HeightVec/sqrt(HeightVec.squared_length());
//						HeightVec=HeightVec*1000;//600;//900;//1000;
//						TransPoint=TransPoint+HeightVec*INTSC_POLY_EXTRU_HEIGHT_RATIO;
//					}
//					else
//					{
//						TransPoint=TransPoint+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*(double)iLengh/(double)iLenStep;//NON_INTSC_POLY_EXTRU_HEIGHT_RATIO;
//					}
//					ExtrudePwh.outer_boundary.push_back(TransPoint);
//				}
//				for (unsigned int i=0;i<Pwh3DIn.inner_hole.size();i++)
//				{
//					vector<Point_3> NewHole;
//					ExtrudePwh.inner_hole.push_back(NewHole);
//					for (unsigned int j=0;j<Pwh3DIn.inner_hole.at(i).size();j++)
//					{
//						//scale
//						Vector_3 PointToCsCenterVect=CSCenter-Pwh3DIn.inner_hole.at(i).at(j);
//						Point_3 ScalePoint=Pwh3DIn.inner_hole.at(i).at(j)+PointToCsCenterVect*iScale/(float)iScaleStep;
//						//translate
//						Point_3 TransPoint=ScalePoint+CstoSSCenterVect*(double)iCenter/(double)iMoveStep;
//						//extrude
//						if (Pwh3DIn.bIntersectSSFace)//input cs intersects with the ss face
//						{
//							//test
//							HeightVec=HeightVec/sqrt(HeightVec.squared_length());
//							HeightVec=HeightVec*1000;
//							TransPoint=TransPoint+HeightVec*INTSC_POLY_EXTRU_HEIGHT_RATIO;
//						}
//						else
//						{
//							TransPoint=TransPoint+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*(double)iLengh/(double)iLenStep;//*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO;
//						}
//						ExtrudePwh.inner_hole.back().push_back(TransPoint);
//					}
//				}
//				//check intersection
//				bool bIntersect=CheckCylinderSSIntersect(iFaceId,iSubSpaceId,Pwh3DIn,ExtrudePwh);
//				//test
//				//if (iScale==1)
//				//{
//				//	this->vecTestPoint.insert(this->vecTestPoint.end(),ExtrudePwh.outer_boundary.begin(),ExtrudePwh.outer_boundary.end());
//				//	this->vecTestPoint.push_back(CSCenter);
//				//	break;
//				//}
//				if (!bIntersect)
//				{
//					Pwh3DOut=ExtrudePwh;
//					break;
//				}
//			}
//			if (!Pwh3DOut.outer_boundary.empty())
//			{
//				break;
//			}
//		}
//		if (!Pwh3DOut.outer_boundary.empty())
//		{
//			break;
//		}
//	}
//	if (Pwh3DIn.bIntersectSSFace)
//	{
//		DBWindowWrite("Pwh intersects face...\n");
//	}
//	else
//	{
//		DBWindowWrite("Pwh does NOT intersects face...\n");
//	}
//	assert(!Pwh3DOut.outer_boundary.empty());
//
//	if (!Pwh3DOut.outer_boundary.empty())
//	{
//		ExtruCenter=CGAL::centroid(Pwh3DOut.outer_boundary.begin(),Pwh3DOut.outer_boundary.end());
//	}
//
//	//test
//	//for (unsigned int i=0;i<Pwh3DOut.outer_boundary.size();i++)
//	//{
//	//	this->vecTestPoint.push_back(Pwh3DOut.outer_boundary.at(i));
//	//}
//
//	if (Pwh3DIn.bIntersectSSFace)
//	{
//		return true;
//	}
//	return false;
//}
//
//bool KW_CS2Surf::CheckCylinderSSIntersect(int iFaceId,int iSubSpaceId,Polygon_with_holes_3 PwhCS,Polygon_with_holes_3 PwhExtru)
//{
//	//intersect or not between each edge vector of the extruded polygons and the subspace faces
//	for (Facet_iterator FaceIter=this->vecSSMesh.at(iSubSpaceId).facets_begin();FaceIter!=this->vecSSMesh.at(iSubSpaceId).facets_end();FaceIter++)
//	{
//		if (FaceIter->GetReserved()==iFaceId)
//		{
//			continue;
//		}
//		Triangle_3 FaceTri(FaceIter->halfedge()->vertex()->point(),
//			FaceIter->halfedge()->next()->vertex()->point(),
//			FaceIter->halfedge()->next()->next()->vertex()->point());
//		for (unsigned int j=0;j<PwhExtru.outer_boundary.size();j++)
//		{
//			Segment_3 Seg(PwhExtru.outer_boundary.at(j),PwhExtru.outer_boundary.at((j+1)%PwhExtru.outer_boundary.size()));
//			if (GeometryAlgorithm::CGAL_do_intersect(Seg,FaceTri))
//			{
//				return true;
//			}
//		}
//		//to avoid numerical issues,also let the distance between the points and face as far as possible
//		for (unsigned int j=0;j<PwhExtru.outer_boundary.size();j++)
//		{
//			double dDist=CGAL::squared_distance(PwhExtru.outer_boundary.at(j),FaceTri.supporting_plane());
//			if (dDist<100)
//			{
//				return true;
//			}
//		}
//	}
//	//two possibilities: all the extruded points are totally inside/outside the subspace
//	//intersect each vector connecting the old and new Pwh and the subspace faces
//	for (Facet_iterator FaceIter=this->vecSSMesh.at(iSubSpaceId).facets_begin();FaceIter!=this->vecSSMesh.at(iSubSpaceId).facets_end();FaceIter++)
//	{
//		if (FaceIter->GetReserved()==iFaceId)
//		{
//			continue;
//		}
//		Triangle_3 FaceTri(FaceIter->halfedge()->vertex()->point(),
//			FaceIter->halfedge()->next()->vertex()->point(),
//			FaceIter->halfedge()->next()->next()->vertex()->point());
//		for (unsigned int j=0;j<PwhExtru.outer_boundary.size();j++)
//		{
//			//if the original point is on the face,don't involve in the calculation
//			double dDistSource=CGAL::squared_distance(PwhCS.outer_boundary.at(j),FaceTri.supporting_plane());
//			if (dDistSource<5)//1
//			{
//				continue;
//			}
//			Segment_3 Seg(PwhCS.outer_boundary.at(j),PwhExtru.outer_boundary.at(j));
//			if (GeometryAlgorithm::CGAL_do_intersect(Seg,FaceTri))
//			{
//				return true;
//			}
//		}
//	}
//	return false;
//}

void KW_CS2Surf::GetOnePolyhFromPwh3(Polygon_with_holes_3 CSPwh3,Polygon_with_holes_2 CSPwh2,Polygon_with_holes_3 ExtruPwh3,Polygon_with_holes_2 ExtruPwh2, 
									 Vector_3 HeightVec,KW_Mesh& OutPolyh)
{
	//compute the total point number first
	int iTotalPointNum=CSPwh3.outer_boundary.size();
	for (unsigned int i=0;i<CSPwh3.inner_hole.size();i++)
	{
		iTotalPointNum=iTotalPointNum+CSPwh3.inner_hole.at(i).size();
	}
	//collect all old and new points,compute the side faces
	vector<Point_3> vecOldPoint,vecNewPoint;
	//collect outer boundary points
	for (unsigned int i=0;i<CSPwh3.outer_boundary.size();i++)
	{
		vecOldPoint.push_back(CSPwh3.outer_boundary.at(i));
	}
	vecNewPoint=ExtruPwh3.outer_boundary;

	int iStartId=0;
	vector<vector<int>> vecSideTri;
	//build side triangles for outer boundary
	BuildSideFaces(iStartId,CSPwh3.outer_boundary.size(),iTotalPointNum,vecSideTri);
	iStartId=iStartId+CSPwh3.outer_boundary.size();
	//collect inner holes points
	for (unsigned int i=0;i<CSPwh3.inner_hole.size();i++)
	{
		for (unsigned int j=0;j<CSPwh3.inner_hole.at(i).size();j++)
		{
			vecOldPoint.push_back(CSPwh3.inner_hole.at(i).at(j));
			Point_3 NewPoint=ExtruPwh3.inner_hole.at(i).at(j);
			vecNewPoint.push_back(NewPoint);
		}
		BuildSideFaces(iStartId,CSPwh3.inner_hole.at(i).size(),iTotalPointNum,vecSideTri);
		iStartId=iStartId+CSPwh3.inner_hole.at(i).size();
	}

	vector<vector<int>> vecTopTri;
	vector<vector<int>> vecBotTri;
	//build bottom and top triangles
	//BuildTopBotFaces(CSPwh2,vecTopTri,vecBotTri);
	BuildTopBotFaces(CSPwh2,vecBotTri);
	//test
	BuildTopBotFaces(ExtruPwh2,vecTopTri,false);

	//convert to KW_MESH
	vector<Point_3> vecTotalPoint=vecOldPoint;
	vecTotalPoint.insert(vecTotalPoint.end(),vecNewPoint.begin(),vecNewPoint.end());
	//this->vecTestPoint.insert(this->vecTestPoint.end(),vecTotalPoint.begin(),vecTotalPoint.end());

	vector<vector<int>> vecTotalTri=vecSideTri;
	vecTotalTri.insert(vecTotalTri.end(),vecTopTri.begin(),vecTopTri.end());
	vecTotalTri.insert(vecTotalTri.end(),vecBotTri.begin(),vecBotTri.end());

	Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecTotalPoint,vecTotalTri);
	OutPolyh.delegate(triangle);

	//mark which vertices are extruded (the latter half) and which belongs to the original POF (the former half)
	int iCount=0;
	for (Vertex_iterator VerIter=OutPolyh.vertices_begin();VerIter!=OutPolyh.vertices_end();VerIter++)
	{
		if (iCount<(int)OutPolyh.size_of_vertices()/2)
		{
			VerIter->SetReserved(0);//original POF
		}
		else
		{
			VerIter->SetReserved(1);//extruded
		}
		iCount++;
	}

	//since HeightVec is the vector orthogonal to the face of the subspace and points to the inside of the subspace
	//so the normals of the bottom faces of the polyhedron should be opposite to HeightVec
	//to ensure that the faces of the polyhedron point outward
	//get the last face of the polyhedron(one bottom face)
	Facet_iterator FaceIter=OutPolyh.facets_end();
	FaceIter--;
	//compute its normal
	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	vector<Vector_3> vecEdgeVect;
	for (int i=0;i<2;i++,Hafc++)
	{
		Point_3 CurrVer=Hafc->vertex()->point();
		Point_3 PrevVer=Hafc->opposite()->vertex()->point();
		Vector_3 CurrVect(PrevVer,CurrVer);
		vecEdgeVect.push_back(CurrVect);
	}
	Vector_3 FaceNorm=CGAL::cross_product(vecEdgeVect.front(),vecEdgeVect.back());
	//compute the angle between the polyhedron face normal and HeightVec
	double dAngle=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(FaceNorm,HeightVec);
	if (dAngle<=90)//the normal of the triangle points to the inside of the subspace,so invert(the angle acturally should equal to 0)
	{
		OutPolyh.inside_out();
	}
}

void KW_CS2Surf::BuildSideFaces(int iStartId,int iEdgeSize,int iTotalPointNum,vector<vector<int>>& vecSideTri)
{
	for (int i=iStartId;i<iStartId+iEdgeSize;i++)
	{
		int iFaceVerId[4];
		iFaceVerId[0]=i;
		iFaceVerId[1]=iFaceVerId[0]+iTotalPointNum;
		iFaceVerId[3]=(iFaceVerId[0]+1-iStartId)%iEdgeSize+iStartId;
		iFaceVerId[2]=iFaceVerId[3]+iTotalPointNum;
		vector<int> FirstTri,SecondTri;
		FirstTri.push_back(iFaceVerId[0]);FirstTri.push_back(iFaceVerId[1]);FirstTri.push_back(iFaceVerId[2]);
		SecondTri.push_back(iFaceVerId[0]);SecondTri.push_back(iFaceVerId[2]);SecondTri.push_back(iFaceVerId[3]);
		vecSideTri.push_back(FirstTri);
		vecSideTri.push_back(SecondTri);
	}
}

void KW_CS2Surf::BuildTopBotFaces(Polygon_with_holes_2 InPwh2,vector<vector<int>>& vecOutTri,bool bBottom/* =true */)
{
	vector<Point_2> vecTotalPoint;
	//list suggesting the id of the start and end point of each segment
	vector<int> vecSegList;


	//DBWindowWrite("face outter polygon has %d vertices\n",InPwh2.outer_boundary().size());
	//DBWindowWrite("face polygon has %d holes\n",InPwh2.number_of_holes());

	//test
	//FILE* pFile=fopen("Poly.txt","w");
	for (Vertex_iterator_2 VerIter=InPwh2.outer_boundary().vertices_begin();VerIter!=InPwh2.outer_boundary().vertices_end();VerIter++)
	{
		vecTotalPoint.push_back(*VerIter);
		//fprintf(pFile,"%f %f\n",(*VerIter).x(),(*VerIter).y());
	}
	//fclose(pFile);
	for (unsigned int i=0;i<InPwh2.outer_boundary().size();i++)
	{
		vecSegList.push_back(i);
		vecSegList.push_back((i+1)%InPwh2.outer_boundary().size());
	}

	//for calculating the triangulation
	vector<Point_2> vecPointInHole;
	for (Hole_const_iterator_2 HoleIter=InPwh2.holes_begin();HoleIter!=InPwh2.holes_end();HoleIter++)
	{
		for (unsigned int i=0;i<(*HoleIter).size();i++)
		{
			vecSegList.push_back(i+vecTotalPoint.size());
			vecSegList.push_back((i+1)%(*HoleIter).size()+vecTotalPoint.size());
		}

		//test
		//FILE* pFile=fopen("PolyIn.txt","w");
		for (Vertex_iterator_2 VerIter=(*HoleIter).vertices_begin();VerIter!=(*HoleIter).vertices_end();VerIter++)
		{
			vecTotalPoint.push_back(*VerIter);
			//fprintf(pFile,"%f %f\n",(*VerIter).x(),(*VerIter).y());
		}
		//fclose(pFile);

		Point_2 ResultPoint;
		if (GeometryAlgorithm::GetArbiPointInPolygon((*HoleIter),ResultPoint))
		{
			vecPointInHole.push_back(ResultPoint);
		}
		else
		{
			DBWindowWrite("Error!can NOT find a point inside the polygon");
		}
	}

	//compute the triangulation
	triangulateio Input,Output,Mid;

	Input.numberofpoints=vecTotalPoint.size();
	Input.pointlist=new REAL[2*Input.numberofpoints];
	for (unsigned int i=0;i<vecTotalPoint.size();i++)
	{
		Input.pointlist[2*i]=vecTotalPoint.at(i).x();
		Input.pointlist[2*i+1]=vecTotalPoint.at(i).y();
	}
	Input.pointmarkerlist=(int*)NULL;
	Input.numberofpointattributes=0;

	Input.numberofsegments=vecSegList.size()/2;
	Input.segmentlist=new int[2*Input.numberofsegments];
	for (unsigned int i=0;i<vecSegList.size();i++)
	{
		Input.segmentlist[i]=vecSegList.at(i);
	}
	Input.segmentmarkerlist=(int*)NULL;

	Input.numberofholes=InPwh2.number_of_holes();
	Input.holelist=new REAL[Input.numberofholes*2];
	for (unsigned int i=0;i<vecPointInHole.size();i++)
	{
		Input.holelist[2*i]=vecPointInHole.at(i).x();
		Input.holelist[2*i+1]=vecPointInHole.at(i).y();
	}

	Input.numberofregions=0;


	Output.pointlist=(REAL*)NULL;
	Output.pointmarkerlist=(int*)NULL;
	Output.trianglelist=(int*)NULL;
	Output.segmentlist=(int*)NULL;
	Output.segmentmarkerlist=(int*)NULL;


	triangulate("zQp", &Input, &Output, &Mid);

	assert(Output.numberofpoints==Input.numberofpoints);

	//compute the triangles,formed from the original points on face
	for (int i=0;i<Output.numberoftriangles;i++)
	{
		if (bBottom)//bottom face,just copy the data
		{
			vector<int> currentBot;
			for (int j=0;j<Output.numberofcorners;j++)
			{
				currentBot.push_back(Output.trianglelist[i*Output.numberofcorners+j]);
			}
			vecOutTri.push_back(currentBot);
		}
		else//top face
		{
			//copy the data,be careful of the vertex index
			vector<int> currentTop;
			for (int j=0;j<Output.numberofcorners;j++)
			{
				currentTop.push_back(Output.trianglelist[i*Output.numberofcorners+j]+vecTotalPoint.size());
			}
			vecOutTri.push_back(currentTop);
		}
	}
	if (!bBottom)//invert the orientation of each top triangles
	{
		for (unsigned int i=0;i<vecOutTri.size();i++)
		{
			int iTemp=vecOutTri.at(i).at(0);
			vecOutTri.at(i).at(0)=vecOutTri.at(i).at(1);
			vecOutTri.at(i).at(1)=iTemp;
		}
	}

	delete [] Input.pointlist;
	delete [] Input.pointmarkerlist;
	delete [] Input.segmentlist;
	delete [] Input.holelist;

	delete [] Output.pointlist;
	delete [] Output.trianglelist;
	delete [] Output.segmentlist;
	delete [] Output.segmentmarkerlist;
}

void KW_CS2Surf::ComputeUnionInSubspace(vector<PolyhedronFromPOF> vecPFPOF,KW_Mesh& ResultPolyh)
{
	//for each face,combine the cylinders whose faces do NOT have edges on the bounding face edges
	//by manipulating the data structure instead of using boolean operation (since they never intersect),to speed up the calculation
	vector<KW_Mesh> vecCombinedCylinder;
	for (unsigned int i=0;i<vecPFPOF.size();i++)
	{
		for (unsigned int j=0;j<vecPFPOF.at(i).vecPwhCylinder.size();j++)
		{
			vecCombinedCylinder.push_back(vecPFPOF.at(i).vecPwhCylinder.at(j));
		}
	}

	//Nef_polyhedron NefResult(vecCombinedCylinder.front());
	if (vecCombinedCylinder.size()==1)
	{
		ResultPolyh=vecCombinedCylinder.front();
		return;
	}

	clock_t   start   =   clock();   
	DBWindowWrite( "CSG compute begins...\n");

	vector<CarveVertex> vecCarveVer0;
	vector<CarveFace> vecCarveFace0;
	CGALKW_MeshToCarveArray(vecCombinedCylinder.front(),vecCarveVer0,vecCarveFace0);
	CarvePoly* pCarveFinal=new CarvePoly(vecCarveFace0,vecCarveVer0);

	for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	{
		vector<CarveVertex> vecCarveVerNew;
		vector<CarveFace> vecCarveFaceNew;
		CGALKW_MeshToCarveArray(vecCombinedCylinder.at(i),vecCarveVerNew,vecCarveFaceNew);

		clock_t   start   =   clock();   
		DBWindowWrite("to use carve csg union...\n");

		CarvePoly* pCarveNew=new CarvePoly(vecCarveFaceNew,vecCarveVerNew);

		//test
#ifdef KW_TEST
		std::ofstream outfFinal;
		outfFinal.open("0.obj");
		writeOBJ(outfFinal, pCarveFinal);//std::cout
		outfFinal.close();

		//test
		std::ofstream outfNew;
		outfNew.open("1.obj");
		writeOBJ(outfNew, pCarveNew);//std::cout
		outfNew.close();
#endif

		try 
		{
			pCarveFinal = carve::csg::CSG().compute(pCarveFinal,pCarveNew,carve::csg::CSG::UNION, NULL,carve::csg::CSG::CLASSIFY_NORMAL);//carve::csg::CSG::CLASSIFY_EDGE carve::csg::CSG::CLASSIFY_NORMAL
		} 
		catch (carve::exception e) 
		{
			DBWindowWrite("error in computing union!!!!!!!!!!!!!!\n");
		}

		clock_t   end   =   clock();   
		DBWindowWrite("used carve csg union, time: %d ms\n",end-start);

		//test
#ifdef KW_TEST
		std::ofstream outfResult;
		outfResult.open("result.obj");
		writeOBJ(outfResult, pCarveFinal);//std::cout
		outfResult.close();
#endif

		delete pCarveNew;pCarveNew=NULL;
	}

	//test
	//std::ofstream outf;
	//outf.open("carve csg out.obj");
	//writeOBJ(outf, pCarveFinal);//std::cout
	//outf.close();

	CarvePolyToCGALKW_Mesh(pCarveFinal,ResultPolyh);

	delete pCarveFinal;pCarveFinal=NULL;

	//test
	//FILE* pFile=fopen("triangle.txt","w");
	//for (Facet_iterator FaceIter=ResultPolyh.facets_begin();FaceIter!=ResultPolyh.facets_end();FaceIter++)
	//{
	//	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	//	vector<Point_3> vecPoint;
	//	do 
	//	{
	//		vecPoint.push_back(Hafc->vertex()->point());
	//		Hafc++;
	//	} while(Hafc!=FaceIter->facet_begin());
	//	Triangle_3 currTri(vecPoint.at(0),vecPoint.at(1),vecPoint.at(2));
	//	if (currTri.squared_area()<1)
	//	{
	//		this->vecTestTri.push_back(currTri);
	//		fprintf(pFile,"area: %f\n",currTri.squared_area());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(0).x(),currTri.vertex(0).y(),currTri.vertex(0).z());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(1).x(),currTri.vertex(1).y(),currTri.vertex(1).z());
	//		fprintf(pFile,"%f %f %f\n\n",currTri.vertex(2).x(),currTri.vertex(2).y(),currTri.vertex(2).z());
	//	}
	//}
	//fclose(pFile);

	//test
	//std::ofstream outf;
	//outf.open("0.off");
	//outf<<vecCombinedCylinder.front();
	//outf.close();

	//for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	//{
		//test
		//std::ofstream outf;
		//outf.open("1.off");
		//outf<<vecCombinedCylinder.at(i);
		//outf.close();

		//Nef_polyhedron NefCurrent(vecCombinedCylinder.at(i));
		//NefResult=NefResult+NefCurrent;
		//if (!NefResult.is_simple())
		//{
		//	DBWindowWrite("Nef Poly union error\n");
		//}
	//}
	//NefResult.convert_to_Polyhedron(ResultPolyh);

	//test
	//std::ofstream outf;
	//outf.open("out.off");
	//outf<<ResultPolyh;
	//outf.close();

	clock_t   endt   =   clock();
	DBWindowWrite("CSG compute  finished,taking: %d ms,%d poly involved\n",endt - start,vecCombinedCylinder.size());

	//test
	//this->vecTestGmpPoly.push_back(ResultPolyh);
}

void KW_CS2Surf::ComputeUnionInSubspace(vector<KW_Mesh> vecSinglePoly,KW_Mesh& ResultPolyh)
{
	vector<KW_Mesh> vecCombinedCylinder=vecSinglePoly;
	//Nef_polyhedron NefResult(vecCombinedCylinder.front());
	if (vecCombinedCylinder.size()==1)
	{
		ResultPolyh=vecCombinedCylinder.front();
		return;
	}

	clock_t   start   =   clock();   
	DBWindowWrite( "CSG compute begins...\n");

	vector<CarveVertex> vecCarveVer0;
	vector<CarveFace> vecCarveFace0;
	CGALKW_MeshToCarveArray(vecCombinedCylinder.front(),vecCarveVer0,vecCarveFace0);
	CarvePoly* pCarveFinal=new CarvePoly(vecCarveFace0,vecCarveVer0);

	for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	{
		vector<CarveVertex> vecCarveVerNew;
		vector<CarveFace> vecCarveFaceNew;
		CGALKW_MeshToCarveArray(vecCombinedCylinder.at(i),vecCarveVerNew,vecCarveFaceNew);

		clock_t   start   =   clock();   
		DBWindowWrite("to use carve csg union...\n");

		CarvePoly* pCarveNew=new CarvePoly(vecCarveFaceNew,vecCarveVerNew);

		////test
#ifdef KW_TEST
		std::ofstream outfFinal;
		outfFinal.open("0.obj");
		writeOBJ(outfFinal, pCarveFinal);//std::cout
		outfFinal.close();

		////test
		std::ofstream outfNew;
		outfNew.open("1.obj");
		writeOBJ(outfNew, pCarveNew);//std::cout
		outfNew.close();
#endif

		try 
		{
			pCarveFinal = carve::csg::CSG().compute(pCarveFinal,pCarveNew,carve::csg::CSG::UNION, NULL,carve::csg::CSG::CLASSIFY_NORMAL);//carve::csg::CSG::CLASSIFY_EDGE carve::csg::CSG::CLASSIFY_NORMAL
		} 
		catch (carve::exception e) 
		{
			DBWindowWrite("error in computing union!!!!!!!!!!!!!!\n");
		}

		clock_t   end   =   clock();   
		DBWindowWrite("used carve csg union, time: %d ms\n",end-start);

		////test
#ifdef KW_TEST
		std::ofstream outfResult;
		outfResult.open("result.obj");
		writeOBJ(outfResult, pCarveFinal);//std::cout
		outfResult.close();
#endif

		delete pCarveNew;pCarveNew=NULL;
	}

	//test
	//std::ofstream outf;
	//outf.open("carve csg out.obj");
	//writeOBJ(outf, pCarveFinal);//std::cout
	//outf.close();

	CarvePolyToCGALKW_Mesh(pCarveFinal,ResultPolyh);

	delete pCarveFinal;pCarveFinal=NULL;

	//test
	//FILE* pFile=fopen("triangle.txt","w");
	//for (Facet_iterator FaceIter=ResultPolyh.facets_begin();FaceIter!=ResultPolyh.facets_end();FaceIter++)
	//{
	//	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	//	vector<Point_3> vecPoint;
	//	do 
	//	{
	//		vecPoint.push_back(Hafc->vertex()->point());
	//		Hafc++;
	//	} while(Hafc!=FaceIter->facet_begin());
	//	Triangle_3 currTri(vecPoint.at(0),vecPoint.at(1),vecPoint.at(2));
	//	if (currTri.squared_area()<1)
	//	{
	//		this->vecTestTri.push_back(currTri);
	//		fprintf(pFile,"area: %f\n",currTri.squared_area());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(0).x(),currTri.vertex(0).y(),currTri.vertex(0).z());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(1).x(),currTri.vertex(1).y(),currTri.vertex(1).z());
	//		fprintf(pFile,"%f %f %f\n\n",currTri.vertex(2).x(),currTri.vertex(2).y(),currTri.vertex(2).z());
	//	}
	//}
	//fclose(pFile);

	//test
	//std::ofstream outf;
	//outf.open("0.off");
	//outf<<vecCombinedCylinder.front();
	//outf.close();

	//for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	//{
	//test
	//std::ofstream outf;
	//outf.open("1.off");
	//outf<<vecCombinedCylinder.at(i);
	//outf.close();

	//Nef_polyhedron NefCurrent(vecCombinedCylinder.at(i));
	//NefResult=NefResult+NefCurrent;
	//if (!NefResult.is_simple())
	//{
	//	DBWindowWrite("Nef Poly union error\n");
	//}
	//}
	//NefResult.convert_to_Polyhedron(ResultPolyh);

	//test
	//std::ofstream outf;
	//outf.open("out.off");
	//outf<<ResultPolyh;
	//outf.close();

	clock_t   endt   =   clock();
	DBWindowWrite("CSG compute  finished,taking: %d ms,%d poly involved\n",endt - start,vecCombinedCylinder.size());

	//test
	//this->vecTestGmpPoly.push_back(ResultPolyh);
}

void KW_CS2Surf::ComputeUnionInSubspace(vector<KW_Mesh> vecSplitPoly,vector<KW_Mesh> vecOtherPoly,KW_Mesh& ResultPolyh)
{
	KW_Mesh CombineResult;
	Convert_vecKW_Mesh_To_KW_Mesh<HalfedgeDS> triangle(vecSplitPoly);
	CombineResult.delegate(triangle);
	vecOtherPoly.push_back(CombineResult);

	vector<KW_Mesh> vecCombinedCylinder=vecOtherPoly;
	//Nef_polyhedron NefResult(vecCombinedCylinder.front());
	if (vecCombinedCylinder.size()==1)
	{
		ResultPolyh=vecCombinedCylinder.front();
		return;
	}

	clock_t   start   =   clock();   
	DBWindowWrite( "CSG compute begins...\n");

	vector<CarveVertex> vecCarveVer0;
	vector<CarveFace> vecCarveFace0;
	CGALKW_MeshToCarveArray(vecCombinedCylinder.front(),vecCarveVer0,vecCarveFace0);
	CarvePoly* pCarveFinal=new CarvePoly(vecCarveFace0,vecCarveVer0);

	for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	{
		vector<CarveVertex> vecCarveVerNew;
		vector<CarveFace> vecCarveFaceNew;
		CGALKW_MeshToCarveArray(vecCombinedCylinder.at(i),vecCarveVerNew,vecCarveFaceNew);

		clock_t   start   =   clock();   
		DBWindowWrite("to use carve csg union...\n");

		CarvePoly* pCarveNew=new CarvePoly(vecCarveFaceNew,vecCarveVerNew);

		////test
		//std::ofstream outfFinal;
		//outfFinal.open("0.obj");
		//writeOBJ(outfFinal, pCarveFinal);//std::cout
		//outfFinal.close();

		////test
		//std::ofstream outfNew;
		//outfNew.open("1.obj");
		//writeOBJ(outfNew, pCarveNew);//std::cout
		//outfNew.close();

		try 
		{
			pCarveFinal = carve::csg::CSG().compute(pCarveFinal,pCarveNew,carve::csg::CSG::UNION, NULL,carve::csg::CSG::CLASSIFY_NORMAL);//carve::csg::CSG::CLASSIFY_EDGE carve::csg::CSG::CLASSIFY_NORMAL
		} 
		catch (carve::exception e) 
		{
			DBWindowWrite("error in computing union!!!!!!!!!!!!!!\n");
		}

		clock_t   end   =   clock();   
		DBWindowWrite("used carve csg union, time: %d ms\n",end-start);

		////test
		//std::ofstream outfResult;
		//outfResult.open("result.obj");
		//writeOBJ(outfResult, pCarveFinal);//std::cout
		//outfResult.close();

		delete pCarveNew;pCarveNew=NULL;
	}

	//test
	//std::ofstream outf;
	//outf.open("carve csg out.obj");
	//writeOBJ(outf, pCarveFinal);//std::cout
	//outf.close();

	CarvePolyToCGALKW_Mesh(pCarveFinal,ResultPolyh);

	delete pCarveFinal;pCarveFinal=NULL;

	//test
	//FILE* pFile=fopen("triangle.txt","w");
	//for (Facet_iterator FaceIter=ResultPolyh.facets_begin();FaceIter!=ResultPolyh.facets_end();FaceIter++)
	//{
	//	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	//	vector<Point_3> vecPoint;
	//	do 
	//	{
	//		vecPoint.push_back(Hafc->vertex()->point());
	//		Hafc++;
	//	} while(Hafc!=FaceIter->facet_begin());
	//	Triangle_3 currTri(vecPoint.at(0),vecPoint.at(1),vecPoint.at(2));
	//	if (currTri.squared_area()<1)
	//	{
	//		this->vecTestTri.push_back(currTri);
	//		fprintf(pFile,"area: %f\n",currTri.squared_area());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(0).x(),currTri.vertex(0).y(),currTri.vertex(0).z());
	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(1).x(),currTri.vertex(1).y(),currTri.vertex(1).z());
	//		fprintf(pFile,"%f %f %f\n\n",currTri.vertex(2).x(),currTri.vertex(2).y(),currTri.vertex(2).z());
	//	}
	//}
	//fclose(pFile);

	//test
	//std::ofstream outf;
	//outf.open("0.off");
	//outf<<vecCombinedCylinder.front();
	//outf.close();

	//for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
	//{
	//test
	//std::ofstream outf;
	//outf.open("1.off");
	//outf<<vecCombinedCylinder.at(i);
	//outf.close();

	//Nef_polyhedron NefCurrent(vecCombinedCylinder.at(i));
	//NefResult=NefResult+NefCurrent;
	//if (!NefResult.is_simple())
	//{
	//	DBWindowWrite("Nef Poly union error\n");
	//}
	//}
	//NefResult.convert_to_Polyhedron(ResultPolyh);

	//test
	//std::ofstream outf;
	//outf.open("out.off");
	//outf<<ResultPolyh;
	//outf.close();

	clock_t   endt   =   clock();
	DBWindowWrite("CSG compute  finished,taking: %d ms,%d poly involved\n",endt - start,vecCombinedCylinder.size());

	//test
	//this->vecTestGmpPoly.push_back(ResultPolyh);
}

void KW_CS2Surf::CGALKW_MeshToCarveArray(KW_Mesh PolyIn, std::vector<CarveVertex> &verts, std::vector<CarveFace> &faces)
{
	verts.clear();
	faces.clear();
	verts.reserve(PolyIn.size_of_vertices());
	faces.reserve(PolyIn.size_of_facets());

	for (Vertex_iterator VerIter=PolyIn.vertices_begin();VerIter!=PolyIn.vertices_end();VerIter++)
	{
		verts.push_back(CarveVertex(carve::geom::VECTOR(VerIter->point().x(),VerIter->point().y(),VerIter->point().z())));
	}
	for (Facet_iterator FaIter=PolyIn.facets_begin();FaIter!=PolyIn.facets_end();FaIter++)
	{
		vector<int> vecVerInd;
		Halfedge_around_facet_circulator Hafc=FaIter->facet_begin();
		do 
		{
			vecVerInd.push_back(distance(PolyIn.vertices_begin(),Hafc->vertex()));
			Hafc++;
		} while(Hafc!=FaIter->facet_begin());
		faces.push_back(CarveFace(&verts[vecVerInd.at(0)], &verts[vecVerInd.at(1)], &verts[vecVerInd.at(2)]));
	}
}

void KW_CS2Surf::CarvePolyToCGALKW_Mesh(CarvePoly* pPolyIn, KW_Mesh& PolyOut)
{
	PolyOut.clear();
	vector<Point_3> vecPoint;
	for (unsigned int i=0;i<pPolyIn->vertices.size();i++)
	{
		CarveVertex Ver=pPolyIn->vertices.at(i);
		vecPoint.push_back(Point_3(Ver.v.x,Ver.v.y,Ver.v.z));
	}
	//since the resulting mesh calculated using Carve CSG may not be a triangle mesh
	//so need to triangulate the polygonal faces(has more than 3 vertices)
	vector<vector<int>> vecFace;
	
	for (unsigned int i=0;i<pPolyIn->faces.size();i++)
	{
		CarveFace face=pPolyIn->faces.at(i);
		//use glu tesselator to triangulate
		vector<Point_3> vecTriPoint;
		vector<int> vecTriVerInd;
		for(unsigned int j=0;j<face.nVertices();j++) 
		{
			vecTriPoint.push_back(Point_3(face.vertex(j)->v.x,face.vertex(j)->v.y,face.vertex(j)->v.z));
		}
		glu_tesselator::tesselate(vecTriPoint,vecTriVerInd);
		if (vecTriVerInd.size()%3!=0)
		{
			DBWindowWrite("tesselation after CSG has error!\n");
			continue;
		}
		for (unsigned int j=0;j<vecTriVerInd.size();j=j+3)
		{
			int iInd0=pPolyIn->vertexToIndex_fast(face.vertex(vecTriVerInd.at(j)));
			int iInd1=pPolyIn->vertexToIndex_fast(face.vertex(vecTriVerInd.at(j+1)));
			int iInd2=pPolyIn->vertexToIndex_fast(face.vertex(vecTriVerInd.at(j+2)));
			vector<int> currentFace;
			currentFace.push_back(iInd0);currentFace.push_back(iInd1);currentFace.push_back(iInd2);
			vecFace.push_back(currentFace);
		}
		//int iInd0=pPolyIn->vertexToIndex_fast(face.vertex(0));
		//for(unsigned int j=1;j<face.nVertices();j++) 
		//{
		//	int iInd1=pPolyIn->vertexToIndex_fast(face.vertex(j));
		//	int iInd2=pPolyIn->vertexToIndex_fast(face.vertex(j+1));
		//	vector<int> currentFace;
		//	currentFace.push_back(iInd0);currentFace.push_back(iInd1);currentFace.push_back(iInd2);
		//	vecFace.push_back(currentFace);
		//	if (j+1==face.nVertices()-1)
		//	{
		//		break;
		//	}
		//}
	}

	//test
	//FILE* pFile=fopen("tempTri.txt","w");
	//for (unsigned int i=0;i<vecFace.size();i++)
	//{
	//	fprintf(pFile,"%d %d %d\n",vecFace.at(i).at(0)+1,vecFace.at(i).at(1)+1,vecFace.at(i).at(2)+1);
	//}
	//fclose(pFile);


	Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecPoint,vecFace);
	PolyOut.delegate(triangle);

	//test
	//std::ofstream outf;
	//outf.open("error check.off");
	//outf<<PolyOut;
	//outf.close();
}

void KW_CS2Surf::RemoveFaceTriangles(int iSubSpaceId,KW_Mesh PolyhIn,vector<vector<Point_3>> vecvecFacePoint,vector<Point_3>& vecSubPoint,vector<vector<int>>& vecSubSurf)
{
	//collect the info of the polyhedron
	vecSubPoint.clear();
	for (Vertex_iterator VerIter=PolyhIn.vertices_begin();VerIter!=PolyhIn.vertices_end();VerIter++)
	{
		vecSubPoint.push_back(Point_3(VerIter->point().x(),VerIter->point().y(),VerIter->point().z()));
	}
	vecSubSurf.clear();
	for (Facet_iterator FaceIter=PolyhIn.facets_begin();FaceIter!=PolyhIn.facets_end();FaceIter++)
	{
		vector<int> currentTri;
		Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
		do 
		{
			int iIndex=distance(PolyhIn.vertices_begin(),Hafc->vertex());
			currentTri.push_back(iIndex);
			Hafc++;
		} while(Hafc!=FaceIter->facet_begin());
		vecSubSurf.push_back(currentTri);
	}
	
	//DBWindowWrite("before removing,ver num: %d,face num: %d\n",vecSubPoint.size(),vecSubSurf.size());

	//collect the triangles on the faces and record them
	////it is assumed that the vertices of the polyhedron which lie on the bounding faces of the subspace
	////remain exactly still during the union calculation process
	set<int> setFaceToRemove;
	//for (unsigned int i=0;i<vecSubSurf.size();i++)
	//{
	//	for (unsigned int j=0;j<vecvecFacePoint.size();j++)
	//	{
	//		vector<Point_3> vecFacePoint=vecvecFacePoint.at(j);
	//		vector<Point_3>::iterator pFindPoint0=find(vecFacePoint.begin(),vecFacePoint.end(),vecSubPoint.at(vecSubSurf.at(i).at(0)));
	//		vector<Point_3>::iterator pFindPoint1=find(vecFacePoint.begin(),vecFacePoint.end(),vecSubPoint.at(vecSubSurf.at(i).at(1)));
	//		vector<Point_3>::iterator pFindPoint2=find(vecFacePoint.begin(),vecFacePoint.end(),vecSubPoint.at(vecSubSurf.at(i).at(2)));
	//		if (pFindPoint0!=vecFacePoint.end() && pFindPoint1!=vecFacePoint.end() && pFindPoint2!=vecFacePoint.end())
	//		{
	//			setFaceToRemove.insert(i);
	//		}
	//	}
	//}

	//old method: using the distance between point and bounding face to judge whether a triangle is on the bounding face or not
	//use the old method again to ensure the faces are deleted
	//set<int> setFaceToRemove;
	for (int i=0;i<this->vecSSspacefacenum.at(iSubSpaceId);i++)
	{
		//check each face of the subspace
		int iFaceId=this->vecvecSSspace.at(iSubSpaceId).at(i);
		if (this->vecResortFace.at(iFaceId).bBoundaryFace)
		{
			continue;
		}
		int iPlaneId=this->vecResortFace.at(iFaceId).iFacePlaneID;
		Plane_3 FacePlane=this->vecTempCN.at(iPlaneId).plane;
		//check each triangle, if it is on this face,then delete it
		for (unsigned int j=0;j<vecSubSurf.size();j++)
		{
			//due to numerical issues, cannot use the "has_on" method of CGAL plane_3
			double dDistance0=CGAL::squared_distance(vecSubPoint.at(vecSubSurf.at(j).at(0)),FacePlane);
			double dDistance1=CGAL::squared_distance(vecSubPoint.at(vecSubSurf.at(j).at(1)),FacePlane);
			double dDistance2=CGAL::squared_distance(vecSubPoint.at(vecSubSurf.at(j).at(2)),FacePlane);
			if (dDistance0<5 && dDistance1<5 && dDistance2<5)
			{
				//DBWindowWrite("distance to plane: %f %f %f\n",dDistance0,dDistance1,dDistance2);
				setFaceToRemove.insert(j);
			}
		}
	}

	//DBWindowWrite("%d triangles should be removed\n",setFaceToRemove.size());

	//delete the triangles on the faces
	vector<vector<int>> vecNewSubSurf;
	for (unsigned int i=0;i<vecSubSurf.size();i++)
	{
		if (setFaceToRemove.size()!=0)
		{
			set<int>::iterator SetIter=find(setFaceToRemove.begin(),setFaceToRemove.end(),i);
			//this face need to be remove,delete its index from setFaceToRemove to speed up calculation
			if (SetIter!=setFaceToRemove.end())
			{
				setFaceToRemove.erase(SetIter);
				continue;
			}
		}
		vecNewSubSurf.push_back(vecSubSurf.at(i));
	}
	vecSubSurf=vecNewSubSurf;

	//attention: do NOT delete the isolated vertices and resort the reserved vertices here!
	//otherwise the indices in triangles will become invalide
	//DBWindowWrite("after removing,ver num: %d,face num: %d\n",vecSubPoint.size(),vecSubSurf.size());

	//remove the isolated points 
	set<int> setValidVerInd;
	for (unsigned int i=0;i<vecSubSurf.size();i++)
	{
		for (unsigned int j=0;j<vecSubSurf.at(i).size();j++)
		{
			setValidVerInd.insert(vecSubSurf.at(i).at(j));
		}
	}
	//if no isolated points, return
	if (setValidVerInd.size()==vecSubPoint.size())
	{
		return;
	}
	//refresh the vertex index in each face
	vector<int> vecRefrVerInd;
	set<int>::iterator SetIter=setValidVerInd.begin();
	for (unsigned int i=0;i<vecSubPoint.size();i++)
	{
		if (SetIter==setValidVerInd.end())
		{
			break;
		}
		if (i==*SetIter)
		{
			vecRefrVerInd.push_back(distance(setValidVerInd.begin(),SetIter));
			SetIter++;
		}
		else
		{
			//just to make the length of vecRefrVerInd equal to that of vecSubPoint
			vecRefrVerInd.push_back(-1);
		}
	}
	//save valid points(unisolated points)
	vector<Point_3> vecValidSubPoint;
	for (set<int>::iterator SetIter=setValidVerInd.begin();SetIter!=setValidVerInd.end();SetIter++)
	{
		vecValidSubPoint.push_back(vecSubPoint.at(*SetIter));
	}
	//save faces with refreshed vertex indices
	vector<vector<int>> vecValidSubSurf;
	for (unsigned int i=0;i<vecSubSurf.size();i++)
	{
		vector<int> ValidSubSurf;
		for (unsigned int j=0;j<vecSubSurf.at(i).size();j++)
		{
			ValidSubSurf.push_back(vecRefrVerInd.at(vecSubSurf.at(i).at(j)));
		}
		vecValidSubSurf.push_back(ValidSubSurf);
	}
	vecSubPoint=vecValidSubPoint;
	vecSubSurf=vecValidSubSurf;
}

void KW_CS2Surf::StitchMesh(vector<vector<Point_3>> vecvecSubPoint, vector<vector<vector<int>>> vecvecSubSurf,KW_Mesh& OutPolyh)
{
	//there may exist isolated vertices in vecvecSubPoint,so ignore its size and just make use of the position info it records
	vector<Point_3> vecFinalPoint=vecvecSubPoint.front();
	vector<vector<int>> vecFinalSurf=vecvecSubSurf.front();

	for (unsigned int i=1;i<vecvecSubSurf.size();i++)
	{
		////test
		//if (i!=2 && i!=6)
		//{
		//	continue;
		//}
		//continue;

		vector<Point_3> vecNewSubPoint=vecvecSubPoint.at(i);
		vector<vector<int>> vecNewSubSurf=vecvecSubSurf.at(i);
		DBWindowWrite("Polyh in %d th Subspace,point number: %d,tri num: %d\n",i,vecNewSubPoint.size(),vecNewSubSurf.size());
		//find same points in vecFinalPoint from vecNewSubPoint
		//if not found,push the new points in vecNewSubPoint to vecFinalPoint
		//record both the new and old point positions in vecFinalPoint
		int iOldPointNum=0;
		int iNewPointNum=0;
		vector<int> vecNewPos;
		for (unsigned int j=0;j<vecNewSubPoint.size();j++)
		{
			vector<Point_3>::iterator pFind=find(vecFinalPoint.begin(),vecFinalPoint.end(),vecNewSubPoint.at(j));
			if (pFind!=vecFinalPoint.end())//found same points in vecNewSubPoint
			{
				int iOldIndex=distance(vecFinalPoint.begin(),pFind);
				vecNewPos.push_back(iOldIndex);
				iOldPointNum++;
			}
			//else
			//{
			//	//points are very very close,regard as same points
			//	bool bFound=false;
			//	for (unsigned int k=0;k<vecFinalPoint.size();k++)
			//	{
			//		double dDist=CGAL::squared_distance(vecNewSubPoint.at(j),vecFinalPoint.at(k));
			//		if (dDist<0.0001)
			//		{
			//			DBWindowWrite("two points are close when stitching mesh. distance: %f\n",dDist);
			//			DBWindowWrite("point 0 position: %f %f %f\n",vecNewSubPoint.at(j).x(),vecNewSubPoint.at(j).y(),vecNewSubPoint.at(j).z());
			//			DBWindowWrite("point 1 position: %f %f %f\n",vecFinalPoint.at(k).x(),vecFinalPoint.at(k).y(),vecFinalPoint.at(k).z());
			//			vecNewPos.push_back(k);
			//			bFound=true;
			//			iOldPointNum++;
			//			break;
			//		}
			//	}
			//	if (!bFound)//add the new positions
			//	{
			//		vecFinalPoint.push_back(vecNewSubPoint.at(j));
			//		vecNewPos.push_back(vecFinalPoint.size()-1);
			//		iNewPointNum++;
			//	}
			//}
			else//add the new positions
			{
				vecFinalPoint.push_back(vecNewSubPoint.at(j));
				vecNewPos.push_back(vecFinalPoint.size()-1);
				iNewPointNum++;
			}
		}
		DBWindowWrite("%d old points found,%d new points added\n",iOldPointNum,iNewPointNum);
		//add the new triangles
		for (unsigned int j=0;j<vecNewSubSurf.size();j++)
		{
			vector<int> vecUpdatedTri;
			vecUpdatedTri.push_back(vecNewPos.at(vecNewSubSurf.at(j).at(0)));
			vecUpdatedTri.push_back(vecNewPos.at(vecNewSubSurf.at(j).at(1)));
			vecUpdatedTri.push_back(vecNewPos.at(vecNewSubSurf.at(j).at(2)));
			vecFinalSurf.push_back(vecUpdatedTri);
		}
		DBWindowWrite("tri num in new Polyh: %d\n",vecFinalSurf.size());
	}
	////test
	//for (unsigned int i=0;i<vecFinalSurf.size();i++)
	//{
	//	Triangle_3 currTri(vecFinalPoint.at(vecFinalSurf.at(i).at(0)),vecFinalPoint.at(vecFinalSurf.at(i).at(1)),
	//		vecFinalPoint.at(vecFinalSurf.at(i).at(2)));
	//	double dArea=currTri.squared_area();
	//	if (dArea<1)
	//	{
	//		DBWindowWrite("triangle area: %f\n",dArea);
	//		this->vecTestTri.push_back(currTri);
	//	}
	//}
	//output
	Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecFinalPoint,vecFinalSurf);
	OutPolyh.clear();
	OutPolyh.delegate(triangle);

	//fill the holes caused by mistake in RemoveFaceTriangles
	OutPolyh.normalize_border();
	vector<Facet_handle> vecFilledFace;
	while (OutPolyh.size_of_border_edges()!=0)
	{
		DBWindowWrite("border edge num: %d\n",OutPolyh.size_of_border_edges());
		Halfedge_iterator HIter=OutPolyh.border_halfedges_begin();
		
		//this->vecTestPoint.push_back(HIter->vertex()->point());
		//this->vecTestPoint.push_back(HIter->opposite()->vertex()->point());

		Halfedge_handle HhToNewFace;
		if (HIter->next()->is_border())
		{
			HhToNewFace=OutPolyh.fill_hole(HIter);
		}
		else
		{
			HhToNewFace=OutPolyh.fill_hole(HIter->opposite());
		}
		vecFilledFace.push_back(HhToNewFace->facet());
		OutPolyh.normalize_border();
	}

	//split filled polygons into triangles
	for (unsigned int i=0;i<vecFilledFace.size();i++)
	{
		if (vecFilledFace.at(i)->facet_degree()==3)
		{
			continue;
		}
		//get the center of the face
		vector<Point_3> vecFacePoint;
		Halfedge_around_facet_circulator Hafc=vecFilledFace.at(i)->facet_begin();
		do 
		{
			vecFacePoint.push_back(Hafc->vertex()->point());
			Hafc++;
		} while(Hafc!=vecFilledFace.at(i)->facet_begin());
		Point_3 Centroid=CGAL::centroid(vecFacePoint.begin(),vecFacePoint.end());
		Halfedge_handle HhNew=OutPolyh.create_center_vertex(Hafc);
		HhNew->vertex()->point()=Centroid;
	}
	//test
	//for (unsigned int i=0;i<this->vecTempCN.size();i++)
	//{
	//	CurveNetwork currentCN=this->vecTempCN.at(i);
	//	for (unsigned int j=0;j<currentCN.Profile3D.size();j++)
	//	{
	//		vector<Point_3> currentCS=currentCN.Profile3D.at(j);
	//		for (unsigned int k=0;k<currentCS.size();k++)
	//		{
	//			if (i==2 && j==0 && k==3)
	//			{
	//				this->vecTestPoint.push_back(currentCS.at(k));
	//			}
	//			else if (i==2 && j==0 && k==4)
	//			{
	//				this->vecTestPoint.push_back(currentCS.at(k));
	//			}
	//		}
	//	}
	//}

	//OBJHandle::UnitizeCGALPolyhedron(OutPolyh,false,false);
	//this->vecUnionPoly.push_back(OutPolyh);







	//test validate
	//for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	//{
	//	for (Vertex_iterator VerIter2=this->InitPolyh.vertices_begin();VerIter2!=this->InitPolyh.vertices_end();VerIter2++)
	//	{
	//		if (VerIter==VerIter2)
	//		{
	//			continue;
	//		}
	//		double dDist=CGAL::squared_distance(VerIter->point(),VerIter2->point());
	//		if (dDist<0.001)
	//		{
	//			int iIndex=distance(this->InitPolyh.vertices_begin(),VerIter);
	//			int iIndex2=distance(this->InitPolyh.vertices_begin(),VerIter2);
	//			DBWindowWrite("two points %d %d are quite near,distance: %f\n",iIndex,iIndex2,dDist);
	//			this->vecTestPoint.push_back(VerIter->point());
	//			this->vecTestPoint.push_back(VerIter2->point());
	//			Halfedge_around_vertex_circulator Havc=VerIter->vertex_begin();
	//			do 
	//			{
	//				Facet_handle Fhtest=Havc->facet();
	//				Halfedge_around_facet_circulator Hafctest=Fhtest->facet_begin();
	//				vector<Point_3> TriPointTest;
	//				do 
	//				{
	//					TriPointTest.push_back(Hafctest->vertex()->point());
	//					Hafctest++;
	//				} while(Hafctest!=Fhtest->facet_begin());
	//				Triangle_3 tritest(TriPointTest.at(0),TriPointTest.at(1),TriPointTest.at(2));
	//				this->vecTestTri.push_back(tritest);
	//				Havc++;
	//			} while(Havc!=VerIter->vertex_begin());

	//			Halfedge_around_vertex_circulator Havc2=VerIter2->vertex_begin();
	//			do 
	//			{
	//				Facet_handle Fhtest2=Havc2->facet();
	//				Halfedge_around_facet_circulator Hafctest2=Fhtest2->facet_begin();
	//				vector<Point_3> TriPointTest2;
	//				do 
	//				{
	//					TriPointTest2.push_back(Hafctest2->vertex()->point());
	//					Hafctest2++;
	//				} while(Hafctest2!=Fhtest2->facet_begin());
	//				Triangle_3 tritest2(TriPointTest2.at(0),TriPointTest2.at(1),TriPointTest2.at(2));
	//				this->vecTestTri.push_back(tritest2);
	//				Havc2++;
	//			} while(Havc2!=VerIter2->vertex_begin());
	//		}
	//	}
	//}
	//test
//	Convert_Array_To_CGALGmpPoly<GmpHalfedgeDS> testtriangle(vecFinalPoint,vecFinalSurf);
//	GmpPolyhedron gmptest;
//	gmptest.delegate(testtriangle);
//	this->vecTestGmpPoly.push_back(gmptest);
	//std::ofstream outf;
	//outf.open("out.off");
	//outf<<OutPolyh;
	//outf.close();
}






























































































































































































































//////////////temporarily unuseful functions/////////////////////////////////////////////////


void KW_CS2Surf::AdjustSSMesh(int iStartFaceId,KW_Mesh InSSMesh,KW_Mesh& OutSSMesh)
{
	//make a copy,modify the copy next and preserve the original subspace mesh
	OutSSMesh=InSSMesh;
	//compute the normals of the faces
	OBJHandle::UnitizeCGALPolyhedron(OutSSMesh,false,false);

	//to extend the start face
	Vector_3 StartFaceNorm=CGAL::NULL_VECTOR;
	//collect the faces to delete
	vector<Facet_handle> vecFaceToDel;
	for (Facet_iterator FaceIter=OutSSMesh.facets_begin();FaceIter!=OutSSMesh.facets_end();FaceIter++)
	{
		if (FaceIter->GetReserved()==iStartFaceId)
		{
			vecFaceToDel.push_back(FaceIter);
		}
	}
	StartFaceNorm=vecFaceToDel.front()->normal();
	//mark all the vertices,prepare for the shrikn before the topology changes
	for (Vertex_iterator VerIter=OutSSMesh.vertices_begin();VerIter!=OutSSMesh.vertices_end();VerIter++)
	{
		VerIter->SetReserved(-1);
	}
	//collect the vertices on the other faces
	for (Vertex_iterator VerIter=OutSSMesh.vertices_begin();VerIter!=OutSSMesh.vertices_end();VerIter++)
	{
		//judge if the vertex is on the starting facet of the subspace mesh
		bool bStartFace=false;
		Halfedge_around_vertex_circulator Havc=VerIter->vertex_begin();
		do 
		{
			if (Havc->facet()->GetReserved()==iStartFaceId)
			{
				bStartFace=true;
				break;
			}
			Havc++;
		} while(Havc!=VerIter->vertex_begin());
		if (!bStartFace)
		{
			VerIter->SetReserved(1);
		}
	}
	//delete the exising triangles
	for (unsigned int i=0;i<vecFaceToDel.size();i++)
	{
		Halfedge_handle HhDel=vecFaceToDel.at(i)->halfedge();
		while(HhDel->is_border())
		{
			HhDel=HhDel->next();
		}
		OutSSMesh.erase_facet(HhDel);
	}
	OutSSMesh.normalize_border();
	//collect the border halfedges&their end vertices in sequential order
	Halfedge_iterator HhBorder=OutSSMesh.border_halfedges_begin();
	if (!HhBorder->next()->is_border())
	{
		HhBorder=HhBorder->opposite();
	}
	vector<Halfedge_handle> vecHhBorder;
	vector<Point_3> vecNewBorderPoint;
	const double dDistToExtend=300;
	do 
	{
		vecHhBorder.push_back(HhBorder);
		Point_3 currBorderPoint=HhBorder->vertex()->point();
		Point_3 NewBorderPoint=currBorderPoint+StartFaceNorm*dDistToExtend;
		vecNewBorderPoint.push_back(NewBorderPoint);
		HhBorder=HhBorder->next();
	} while(HhBorder!=vecHhBorder.front());
	//fill the hole
	OutSSMesh.fill_hole(vecHhBorder.front());
	OutSSMesh.normalize_border();
	assert(OutSSMesh.size_of_border_edges()==0);
	//create the center vertex
	Point_3 NewCenter=CGAL::centroid(vecNewBorderPoint.begin(),vecNewBorderPoint.end());
	Halfedge_handle hhToCenter=OutSSMesh.create_center_vertex(vecHhBorder.front());
	hhToCenter->vertex()->point()=NewCenter;
	//get the halfedges pointing to the new center
	vector<Halfedge_handle> vecHhToCenter;
	for (unsigned int i=0;i<vecHhBorder.size();i++)
	{
		vecHhToCenter.push_back(vecHhBorder.at(i)->next());
	}
	//split the halfedges to center,place the new mid point
	vector<Halfedge_handle> vechhToNewEdgeVertex;
	for (unsigned int i=0;i<vecHhToCenter.size();i++)
	{
		Halfedge_handle HhToNewEdgeVertex=OutSSMesh.split_edge(vecHhToCenter.at(i));
		HhToNewEdgeVertex->vertex()->point()=vecNewBorderPoint.at(i);
		vechhToNewEdgeVertex.push_back(HhToNewEdgeVertex);
	}
	//split the faces
	for (unsigned int i=0;i<vechhToNewEdgeVertex.size();i++)
	{
		int itest=vechhToNewEdgeVertex.at(i)->facet()->facet_degree();
		Halfedge_handle hhnewQuad=OutSSMesh.split_facet(vechhToNewEdgeVertex.at(i),
			vechhToNewEdgeVertex.at(i)->next()->next());
		itest=hhnewQuad->facet()->facet_degree();
		OutSSMesh.split_facet(hhnewQuad,hhnewQuad->next()->next());
	}
	//finish extending the start face

	//shrink the other faces
	const double dDistToShrink=100;
	for (Vertex_iterator VerIter=OutSSMesh.vertices_begin();VerIter!=OutSSMesh.vertices_end();VerIter++)
	{
		if (VerIter->GetReserved()==1)
		{
			Point_3 NewPos=VerIter->point()-VerIter->normal()*dDistToShrink;
		}
	}

}

bool KW_CS2Surf::IntersectPolySS(int iFaceId,int iSubSpaceId,KW_Mesh& InOutPolyh)
{
	//extend the starting face of the subspace mesh,to make the bottom face of the cylinder 
	//completely contained in the subspace (avoid the overlapping of the faces of the subspace
	//and the cylinder)
	KW_Mesh TempSSMesh;
	AdjustSSMesh(iFaceId,this->vecSSMesh.at(iSubSpaceId),TempSSMesh);

	//compute the intersection of the polyhedron and subspace
	vector<CarveVertex> vecCarveVerPoly;
	vector<CarveFace> vecCarveFacePoly;
	CGALKW_MeshToCarveArray(InOutPolyh,vecCarveVerPoly,vecCarveFacePoly);
	CarvePoly* pCarvePoly=new CarvePoly(vecCarveFacePoly,vecCarveVerPoly);
	vector<CarveVertex> vecCarveVerSS;
	vector<CarveFace> vecCarveFaceSS;
	//CGALKW_MeshToCarveArray(this->vecSSMesh.at(iSubSpaceId),vecCarveVerSS,vecCarveFaceSS);
	CGALKW_MeshToCarveArray(TempSSMesh,vecCarveVerSS,vecCarveFaceSS);
	CarvePoly* pCarveSS=new CarvePoly(vecCarveFaceSS,vecCarveVerSS);
	clock_t   start   =   clock();   
	//DBWindowWrite("to use carve csg intersection...\n");

	//test
	//std::ofstream outf;
	//outf.open("0.off");
	//outf<<InOutPolyh;
	//outf.close();
	//outf.open("1.off");
	//outf<<this->vecSSMesh.at(iSubSpaceId);
	//outf.close();

	//std::ofstream outfFinal;
	//outfFinal.open("0.obj");
	//writeOBJ(outfFinal, pCarvePoly);
	//outfFinal.close();
	//outfFinal.open("1.obj");
	//writeOBJ(outfFinal, pCarveSS);
	//outfFinal.close();

	try 
	{
		pCarvePoly = carve::csg::CSG().compute(pCarvePoly,pCarveSS,carve::csg::CSG::INTERSECTION, NULL,carve::csg::CSG::CLASSIFY_NORMAL);//carve::csg::CSG::CLASSIFY_EDGE carve::csg::CSG::CLASSIFY_NORMAL
	}
	catch (carve::exception e) 
	{
		DBWindowWrite("error in computing intersection\n");
	}
	clock_t   end   =   clock();   
	DBWindowWrite("used carve csg intersection, time: %d ms\n",end-start);
	CarvePolyToCGALKW_Mesh(pCarvePoly,InOutPolyh);

	//test
	//outf.open("carve-out.off");
	//outf<<InOutPolyh;
	//outf.close();

	//outfFinal.open("1a.obj");
	//writeOBJ(outfFinal, pCarvePoly);//std::cout
	//outfFinal.close();

	delete pCarvePoly;pCarvePoly=NULL;
	delete pCarveSS;pCarveSS=NULL;

	//test
	return true;

	//remove the points
	//mark all the points
	for (Vertex_iterator VerIter=InOutPolyh.vertices_begin();VerIter!=InOutPolyh.vertices_end();VerIter++)
	{
		VerIter->SetReserved(-1);
	}
	//mark the points on the starting facet in KW_MESH
	//put them onto the face plane
	int iStartPlaneId=this->vecResortFace.at(iFaceId).iFacePlaneID;
	Plane_3 StartPlane=this->vecTempCN.at(iStartPlaneId).plane;
	for (Vertex_iterator VerIter=InOutPolyh.vertices_begin();VerIter!=InOutPolyh.vertices_end();VerIter++)
	{
		double dDist=CGAL::squared_distance(VerIter->point(),StartPlane);
		if (dDist<0.0001)
		{
			VerIter->SetReserved(1);
		}
	}
	//judge if the other points are on the other triangles;if yes, perturb
	for (Facet_iterator FaceIter=this->vecSSMesh.at(iSubSpaceId).facets_begin();FaceIter!=this->vecSSMesh.at(iSubSpaceId).facets_end();FaceIter++)
	{
		int icurrFaceId=FaceIter->GetReserved();
		if (icurrFaceId==iFaceId)
		{
			continue;
		}
		Vector_3 VectToMove=CGAL::NULL_VECTOR;
		if (this->vecResortFace.at(icurrFaceId).vecSubspaceId.front()==iSubSpaceId)
		{
			VectToMove=this->vecResortFace.at(icurrFaceId).vecHeightVect.front();
		}
		else if (this->vecResortFace.at(icurrFaceId).vecSubspaceId.back()==iSubSpaceId)
		{
			VectToMove=this->vecResortFace.at(icurrFaceId).vecHeightVect.back();
		}
		//normalize
		double dLength=sqrt(VectToMove.squared_length());
		VectToMove=Vector_3(VectToMove.x()/dLength,VectToMove.y()/dLength,VectToMove.z()/dLength);
		//get the plane of the current face
		Halfedge_handle h=FaceIter->halfedge();
		Plane_3 FacePlane=Plane_3(h->vertex()->point(),h->next()->vertex()->point(),h->prev()->vertex()->point());

		for (Vertex_iterator VerIter=InOutPolyh.vertices_begin();VerIter!=InOutPolyh.vertices_end();VerIter++)
		{
			if (VerIter->GetReserved()==1)//vertex on the start face,don't move
			{
				continue;
			}
			//judge the distance between the point and the plane
			//also should judge if the projection of the point on the plane is inside the triangle or not(skip this step here)
			double dDist=CGAL::squared_distance(VerIter->point(),FacePlane);
			if (dDist<0.0001)
			{
				VerIter->point()=VerIter->point()+VectToMove*EXTRU_OUTBOUND_PERTRU_DIST*0.01;
			}
		}
	}
	return true;
}

//bool KW_CS2Surf::PerturbOneOutBound(ResortedFace FaceInfo,Polygon_with_holes_3 Pwh3D,Vector_3 HeightVec,vector<Point_3>& NewOutBound,int iSubSpaceId)
//{
//	//get four edges of the face,and the corresponding width vector of them
//	vector<Segment_3> vecFaceEdge;
//	for (unsigned int i=0;i<FaceInfo.vecFaceVertex.size();i++)
//	{
//		Segment_3 currentSeg(FaceInfo.vecFaceVertex.at(i),FaceInfo.vecFaceVertex.at((i+1)%FaceInfo.vecFaceVertex.size()));
//		vecFaceEdge.push_back(currentSeg);
//	}
//	//see which/how many face edge has polygon vertices on
//	int iTotalEdgeNum=0;
//	for (unsigned int i=0;i<vecFaceEdge.size();i++)
//	{
//		if (iTotalEdgeNum>2)
//		{
//			break;
//		}
//		for (unsigned int j=0;j<Pwh3D.AssistOuterEdge.size();j++)
//		{
//			//get the current assist edge points
//			int iStartPointID=Pwh3D.AssistOuterEdge.at(j);
//			int iEndPointID=(Pwh3D.AssistOuterEdge.at(j)+1)%Pwh3D.outer_boundary.size();
//			Point_3 StartPoint=Pwh3D.outer_boundary.at(iStartPointID);
//			Point_3 EndPoint=Pwh3D.outer_boundary.at(iEndPointID);
//			double dStartDist=CGAL::squared_distance(StartPoint,vecFaceEdge.at(i));
//			double dEndDist=CGAL::squared_distance(EndPoint,vecFaceEdge.at(i));
//			if (dStartDist<1 && dEndDist<1)
//			{
//				iTotalEdgeNum++;
//				break;
//			}
//		}
//	}
//	
//	//test
//	iTotalEdgeNum=0;
//
//	//extrude
//	for (unsigned int i=0;i<Pwh3D.outer_boundary.size();i++)
//	{
//		//no polygon vertices lie on face edges
//		if (iTotalEdgeNum==0)
//		{
//			////just for make good examples for the "C" shape and partition example
//			//if (iSubSpaceId==13)
//			//{
//			//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*1.25);///2//1.25
//			//	//if (FaceInfo.iFacePlaneID==3)
//			//	//{
//			//	//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*1.7);///2//1.25//1.7
//			//	//}
//			//	//else
//			//	//{
//			//	//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*1.05);///2//1.25
//			//	//}
//			//}
//			//else if (iSubSpaceId==3)
//			//{
//			//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*1.1);///*1.2//1.1
//			//}
//			//else if (iSubSpaceId!=2 && iSubSpaceId!=3)
//			//{
//			//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO*0.3);///2
//			//}
//			//else
//			//{
//			//	NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO);///2
//			//}
//			NewOutBound.push_back(Pwh3D.outer_boundary.at(i)+HeightVec*NON_INTSC_POLY_EXTRU_HEIGHT_RATIO);///2
//		}
//		//since polygon may also have edges on two parallel face edges,so delete the following method and use a 
//		//uniform method (move towards the center)
//		////polygon has edges on one or two ORTHOGONAL face edges:move along the width vector
//		//else if (iTotalEdgeNum<=2)
//		//{
//		//	//normalize
//		//	double dLength=sqrt(VectToMove.squared_length());
//		//	VectToMove=Vector_3(VectToMove.x()/dLength,VectToMove.y()/dLength,VectToMove.z()/dLength);
//		//	Point_3 MovedPoint=Pwh3D.outer_boundary.at(i)+VectToMove*EXTRU_OUTBOUND_PERTRU_DIST;
//		//	NewOutBound.push_back(MovedPoint+HeightVec*INTSC_POLY_EXTRU_HEIGHT_RATIO);///4
//		//}
//		////has edges on three or four orthogonal face edges:move towords the face center
//		//has edges on face edges:move towords the face center
//		else
//		{
//			Point_3 FaceCenter=CGAL::centroid(FaceInfo.vecFaceVertex.begin(),FaceInfo.vecFaceVertex.end());
//			Vector_3 VectToMove=FaceCenter-Pwh3D.outer_boundary.at(i);
//			//normalize
//			double dLength=sqrt(VectToMove.squared_length());
//			VectToMove=Vector_3(VectToMove.x()/dLength,VectToMove.y()/dLength,VectToMove.z()/dLength);
//			Point_3 MovedPoint=Pwh3D.outer_boundary.at(i)+VectToMove*EXTRU_OUTBOUND_PERTRU_DIST;
//			NewOutBound.push_back(MovedPoint+HeightVec*INTSC_POLY_EXTRU_HEIGHT_RATIO);///4
//		}
//	}
//
//	if (Pwh3D.AssistOuterEdge.empty())
//	{
//		return false;
//	}
//	return true;
//}

//void KW_CS2Surf::GetOnePolyhFromPwh3(Polygon_with_holes_3 InPwh3,Polygon_with_holes_2 InPwh2,vector<Point_3> PerturbedOutBound, 
//									 Vector_3 HeightVec,KW_Mesh& OutPolyh)
//{
//	//compute the total point number first
//	int iTotalPointNum=InPwh3.outer_boundary.size();
//	for (unsigned int i=0;i<InPwh3.inner_hole.size();i++)
//	{
//		iTotalPointNum=iTotalPointNum+InPwh3.inner_hole.at(i).size();
//	}
//	//collect all old and new points,compute the side faces
//	vector<Point_3> vecOldPoint,vecNewPoint;
//	//collect outer boundary points
//	for (unsigned int i=0;i<InPwh3.outer_boundary.size();i++)
//	{
//		vecOldPoint.push_back(InPwh3.outer_boundary.at(i));
//		//Point_3 NewPoint=InPwh3.outer_boundary.at(i)+HeightVec/2;
//		//vecNewPoint.push_back(NewPoint);
//	}
//	assert(InPwh3.outer_boundary.size()==PerturbedOutBound.size());
//	vecNewPoint=PerturbedOutBound;
//
//	int iStartId=0;
//	vector<vector<int>> vecSideTri;
//	//build side triangles for outer boundary
//	BuildSideFaces(iStartId,InPwh3.outer_boundary.size(),iTotalPointNum,vecSideTri);
//	iStartId=iStartId+InPwh3.outer_boundary.size();
//	//collect inner holes points
//	for (unsigned int i=0;i<InPwh3.inner_hole.size();i++)
//	{
//		for (unsigned int j=0;j<InPwh3.inner_hole.at(i).size();j++)
//		{
//			vecOldPoint.push_back(InPwh3.inner_hole.at(i).at(j));
//			Point_3 NewPoint=InPwh3.inner_hole.at(i).at(j)+HeightVec/2;
//			vecNewPoint.push_back(NewPoint);
//		}
//		BuildSideFaces(iStartId,InPwh3.inner_hole.at(i).size(),iTotalPointNum,vecSideTri);
//		iStartId=iStartId+InPwh3.inner_hole.at(i).size();
//	}
//
//	vector<vector<int>> vecTopTri;
//	vector<vector<int>> vecBotTri;
//	//build bottom and top triangles
//	BuildTopBotFaces(InPwh2,vecTopTri,vecBotTri);
//
//	//convert to KW_MESH
//	vector<Point_3> vecTotalPoint=vecOldPoint;
//	vecTotalPoint.insert(vecTotalPoint.end(),vecNewPoint.begin(),vecNewPoint.end());
//	//this->vecTestPoint.insert(this->vecTestPoint.end(),vecTotalPoint.begin(),vecTotalPoint.end());
//
//	vector<vector<int>> vecTotalTri=vecSideTri;
//	vecTotalTri.insert(vecTotalTri.end(),vecTopTri.begin(),vecTopTri.end());
//	vecTotalTri.insert(vecTotalTri.end(),vecBotTri.begin(),vecBotTri.end());
//
//	Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecTotalPoint,vecTotalTri);
//	OutPolyh.delegate(triangle);
//
//	//since HeightVec is the vector orthogonal to the face of the subspace and points to the inside of the subspace
//	//so the normals of the bottom faces of the polyhedron should be opposite to HeightVec
//	//to ensure that the faces of the polyhedron point outward
//	//get the last face of the polyhedron(one bottom face)
//	Facet_iterator FaceIter=OutPolyh.facets_end();
//	FaceIter--;
//	//compute its normal
//	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
//	vector<Vector_3> vecEdgeVect;
//	for (int i=0;i<2;i++,Hafc++)
//	{
//		Point_3 CurrVer=Hafc->vertex()->point();
//		Point_3 PrevVer=Hafc->opposite()->vertex()->point();
//		Vector_3 CurrVect(PrevVer,CurrVer);
//		vecEdgeVect.push_back(CurrVect);
//	}
//	Vector_3 FaceNorm=CGAL::cross_product(vecEdgeVect.front(),vecEdgeVect.back());
//	//compute the angle between the polyhedron face normal and HeightVec
//	double dAngle=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(FaceNorm,HeightVec);
//	if (dAngle<=90)//the normal of the triangle points to the inside of the subspace,so invert(the angle acturally should equal to 0)
//	{
//		OutPolyh.inside_out();
//	}
//
//	//test
//	//this->vecTestGmpPoly.push_back(OutPolyh);
//}

//void KW_CS2Surf::ComputeUnionInSubspace(vector<Int_Int_Pair> IntersectPwh,vector<PolyhedronFromPOF> vecPFPOF,KW_Mesh& ResultPolyh)
//{
//	//for each face,combine the cylinders whose faces do NOT have edges on the bounding face edges
//	//by manipulating the data structure instead of using boolean operation (since they never intersect),to speed up the calculation
//	vector<KW_Mesh> vecCombinedCylinder;
//	for (unsigned int i=0;i<vecPFPOF.size();i++)
//	{
//		vector<KW_Mesh> vecCylinderToCombine;
//		for (unsigned int j=0;j<vecPFPOF.at(i).vecPwhCylinder.size();j++)
//		{
//			Int_Int_Pair currentPair=make_pair(i,j);
//			vector<Int_Int_Pair>::iterator pFind=find(IntersectPwh.begin(),IntersectPwh.end(),currentPair);
//			if (pFind==IntersectPwh.end())
//			{
//				vecCylinderToCombine.push_back(vecPFPOF.at(i).vecPwhCylinder.at(j));
//			}
//		}
//		KW_Mesh CombineResult;
//		if (vecCylinderToCombine.size()==0)
//		{
//			continue;
//		}
//		else if (vecCylinderToCombine.size()==1)//only one cylinder which does not intersect with face edges
//		{
//			CombineResult=vecCylinderToCombine.front();
//		}
//		else//multiple cylinders who do not intersect with face edges,combine them
//		{
//			Convert_vecKW_Mesh_To_KW_Mesh<HalfedgeDS> triangle(vecCylinderToCombine);
//			CombineResult.delegate(triangle);
//		}
//		vecCombinedCylinder.push_back(CombineResult);
//	}
//
//	//compute the union of vecCombinedCylinder and all other cylinders
//	for (unsigned int i=0;i<IntersectPwh.size();i++)
//	{
//		vecCombinedCylinder.push_back(vecPFPOF.at(IntersectPwh.at(i).first).vecPwhCylinder.at(IntersectPwh.at(i).second));
//	}
//
//
//	//Nef_polyhedron NefResult(vecCombinedCylinder.front());
//	if (vecCombinedCylinder.size()==1)
//	{
//		ResultPolyh=vecCombinedCylinder.front();
//		return;
//	}
//
//	clock_t   start   =   clock();   
//	DBWindowWrite( "CSG compute begins...\n");
//
//	vector<CarveVertex> vecCarveVer0;
//	vector<CarveFace> vecCarveFace0;
//	CGALKW_MeshToCarveArray(vecCombinedCylinder.front(),vecCarveVer0,vecCarveFace0);
//	CarvePoly* pCarveFinal=new CarvePoly(vecCarveFace0,vecCarveVer0);
//
//	for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
//	{
//		vector<CarveVertex> vecCarveVerNew;
//		vector<CarveFace> vecCarveFaceNew;
//		CGALKW_MeshToCarveArray(vecCombinedCylinder.at(i),vecCarveVerNew,vecCarveFaceNew);
//
//		clock_t   start   =   clock();   
//		DBWindowWrite("to use carve csg union...\n");
//
//		CarvePoly* pCarveNew=new CarvePoly(vecCarveFaceNew,vecCarveVerNew);
//
//		////test
//		std::ofstream outfFinal;
//		outfFinal.open("0.obj");
//		writeOBJ(outfFinal, pCarveFinal);//std::cout
//		outfFinal.close();
//
//		////test
//		std::ofstream outfNew;
//		outfNew.open("1.obj");
//		writeOBJ(outfNew, pCarveNew);//std::cout
//		outfNew.close();
//
//		try 
//		{
//			pCarveFinal = carve::csg::CSG().compute(pCarveFinal,pCarveNew,carve::csg::CSG::UNION, NULL,carve::csg::CSG::CLASSIFY_NORMAL);//carve::csg::CSG::CLASSIFY_EDGE carve::csg::CSG::CLASSIFY_NORMAL
//		} 
//		catch (carve::exception e) 
//		{
//			DBWindowWrite("error in computing union!!!!!!!!!!!!!!\n");
//		}
//
//		clock_t   end   =   clock();   
//		DBWindowWrite("used carve csg union, time: %d ms\n",end-start);
//
//		////test
//		std::ofstream outfResult;
//		outfResult.open("result.obj");
//		writeOBJ(outfResult, pCarveFinal);//std::cout
//		outfResult.close();
//
//		delete pCarveNew;pCarveNew=NULL;
//	}
//
//	//test
//	//std::ofstream outf;
//	//outf.open("carve csg out.obj");
//	//writeOBJ(outf, pCarveFinal);//std::cout
//	//outf.close();
//
//	CarvePolyToCGALKW_Mesh(pCarveFinal,ResultPolyh);
//
//	delete pCarveFinal;pCarveFinal=NULL;
//
//	//test
//	//FILE* pFile=fopen("triangle.txt","w");
//	//for (Facet_iterator FaceIter=ResultPolyh.facets_begin();FaceIter!=ResultPolyh.facets_end();FaceIter++)
//	//{
//	//	Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
//	//	vector<Point_3> vecPoint;
//	//	do 
//	//	{
//	//		vecPoint.push_back(Hafc->vertex()->point());
//	//		Hafc++;
//	//	} while(Hafc!=FaceIter->facet_begin());
//	//	Triangle_3 currTri(vecPoint.at(0),vecPoint.at(1),vecPoint.at(2));
//	//	if (currTri.squared_area()<1)
//	//	{
//	//		this->vecTestTri.push_back(currTri);
//	//		fprintf(pFile,"area: %f\n",currTri.squared_area());
//	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(0).x(),currTri.vertex(0).y(),currTri.vertex(0).z());
//	//		fprintf(pFile,"%f %f %f\n",currTri.vertex(1).x(),currTri.vertex(1).y(),currTri.vertex(1).z());
//	//		fprintf(pFile,"%f %f %f\n\n",currTri.vertex(2).x(),currTri.vertex(2).y(),currTri.vertex(2).z());
//	//	}
//	//}
//	//fclose(pFile);
//
//	//test
//	//std::ofstream outf;
//	//outf.open("0.off");
//	//outf<<vecCombinedCylinder.front();
//	//outf.close();
//
//	//for (unsigned int i=1;i<vecCombinedCylinder.size();i++)
//	//{
//	//test
//	//std::ofstream outf;
//	//outf.open("1.off");
//	//outf<<vecCombinedCylinder.at(i);
//	//outf.close();
//
//	//Nef_polyhedron NefCurrent(vecCombinedCylinder.at(i));
//	//NefResult=NefResult+NefCurrent;
//	//if (!NefResult.is_simple())
//	//{
//	//	DBWindowWrite("Nef Poly union error\n");
//	//}
//	//}
//	//NefResult.convert_to_Polyhedron(ResultPolyh);
//
//	//test
//	//std::ofstream outf;
//	//outf.open("out.off");
//	//outf<<ResultPolyh;
//	//outf.close();
//
//	clock_t   endt   =   clock();
//	DBWindowWrite("CSG compute  finished,taking: %d ms,%d poly involved\n",endt - start,vecCombinedCylinder.size());
//
//	//test
//	//this->vecTestGmpPoly.push_back(ResultPolyh);
//}

//void KW_CS2Surf::BuildTopBotFaces(Polygon_with_holes_2 InPwh2,vector<vector<int>>& vecTopTri,vector<vector<int>>& vecBotTri)
//{
//	vector<Point_2> vecTotalPoint;
//	//list suggesting the id of the start and end point of each segment
//	vector<int> vecSegList;
//
//
//	//DBWindowWrite("face outter polygon has %d vertices\n",InPwh2.outer_boundary().size());
//	//DBWindowWrite("face polygon has %d holes\n",InPwh2.number_of_holes());
//
//	//test
//	//FILE* pFile=fopen("Poly.txt","w");
//	for (Vertex_iterator_2 VerIter=InPwh2.outer_boundary().vertices_begin();VerIter!=InPwh2.outer_boundary().vertices_end();VerIter++)
//	{
//		vecTotalPoint.push_back(*VerIter);
//		//fprintf(pFile,"%f %f\n",(*VerIter).x(),(*VerIter).y());
//	}
//	//fclose(pFile);
//	for (unsigned int i=0;i<InPwh2.outer_boundary().size();i++)
//	{
//		vecSegList.push_back(i);
//		vecSegList.push_back((i+1)%InPwh2.outer_boundary().size());
//	}
//
//	//for calculating the triangulation
//	vector<Point_2> vecPointInHole;
//	for (Hole_const_iterator_2 HoleIter=InPwh2.holes_begin();HoleIter!=InPwh2.holes_end();HoleIter++)
//	{
//		for (unsigned int i=0;i<(*HoleIter).size();i++)
//		{
//			vecSegList.push_back(i+vecTotalPoint.size());
//			vecSegList.push_back((i+1)%(*HoleIter).size()+vecTotalPoint.size());
//		}
//
//		//test
//		//FILE* pFile=fopen("PolyIn.txt","w");
//		for (Vertex_iterator_2 VerIter=(*HoleIter).vertices_begin();VerIter!=(*HoleIter).vertices_end();VerIter++)
//		{
//			vecTotalPoint.push_back(*VerIter);
//			//fprintf(pFile,"%f %f\n",(*VerIter).x(),(*VerIter).y());
//		}
//		//fclose(pFile);
//
//		Point_2 ResultPoint;
//		if (GeometryAlgorithm::GetArbiPointInPolygon((*HoleIter),ResultPoint))
//		{
//			vecPointInHole.push_back(ResultPoint);
//		}
//		else
//		{
//			DBWindowWrite("Error!can NOT find a point inside the polygon");
//		}
//	}
//
//	//compute the triangulation
//	triangulateio Input,Output,Mid;
//
//	Input.numberofpoints=vecTotalPoint.size();
//	Input.pointlist=new REAL[2*Input.numberofpoints];
//	for (unsigned int i=0;i<vecTotalPoint.size();i++)
//	{
//		Input.pointlist[2*i]=vecTotalPoint.at(i).x();
//		Input.pointlist[2*i+1]=vecTotalPoint.at(i).y();
//	}
//	Input.pointmarkerlist=(int*)NULL;
//	Input.numberofpointattributes=0;
//
//	Input.numberofsegments=vecSegList.size()/2;
//	Input.segmentlist=new int[2*Input.numberofsegments];
//	for (unsigned int i=0;i<vecSegList.size();i++)
//	{
//		Input.segmentlist[i]=vecSegList.at(i);
//	}
//	Input.segmentmarkerlist=(int*)NULL;
//
//	Input.numberofholes=InPwh2.number_of_holes();
//	Input.holelist=new REAL[Input.numberofholes*2];
//	for (unsigned int i=0;i<vecPointInHole.size();i++)
//	{
//		Input.holelist[2*i]=vecPointInHole.at(i).x();
//		Input.holelist[2*i+1]=vecPointInHole.at(i).y();
//	}
//
//	Input.numberofregions=0;
//
//
//	Output.pointlist=(REAL*)NULL;
//	Output.pointmarkerlist=(int*)NULL;
//	Output.trianglelist=(int*)NULL;
//	Output.segmentlist=(int*)NULL;
//	Output.segmentmarkerlist=(int*)NULL;
//
//
//	triangulate("zQp", &Input, &Output, &Mid);
//
//	assert(Output.numberofpoints==Input.numberofpoints);
//
//	//compute the bottom triangles,formed from the original points on face
//	for (int i=0;i<Output.numberoftriangles;i++)
//	{
//		vector<int> currentBot;
//		for (int j=0;j<Output.numberofcorners;j++)
//		{
//			currentBot.push_back(Output.trianglelist[i*Output.numberofcorners+j]);
//		}
//		vecBotTri.push_back(currentBot);
//	}
//	//compute the top triangles,formed from the extruded points
//	for (unsigned int i=0;i<vecBotTri.size();i++)
//	{
//		vector<int> currentTop;
//		for (unsigned int j=0;j<vecBotTri.at(i).size();j++)
//		{
//			currentTop.push_back(vecBotTri.at(i).at(j)+vecTotalPoint.size());
//		}
//		vecTopTri.push_back(currentTop);
//	}
//
//	//invert the orientation of each top triangles
//	for (unsigned int i=0;i<vecTopTri.size();i++)
//	{
//		int iTemp=vecTopTri.at(i).at(0);
//		vecTopTri.at(i).at(0)=vecTopTri.at(i).at(1);
//		vecTopTri.at(i).at(1)=iTemp;
//	}
//
//
//
//	delete [] Input.pointlist;
//	delete [] Input.pointmarkerlist;
//	delete [] Input.segmentlist;
//	delete [] Input.holelist;
//
//	delete [] Output.pointlist;
//	delete [] Output.trianglelist;
//	delete [] Output.segmentlist;
//	delete [] Output.segmentmarkerlist;
//
//}