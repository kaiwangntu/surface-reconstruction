#include "StdAfx.h"
#include "KW_CS2Surf.h"

void KW_CS2Surf::PutCNtoFace()
{
	//subtract the inner holes from the outter CS for each CN
	vector<Pwh_list_2> vecCSSubtractResult;
	for (unsigned int i=0;i<this->vecTempCN.size();i++)
	{
		Pwh_list_2 currPwhList;
		SubtractInnerHole(this->vecTempCN.at(i),currPwhList);
		vecCSSubtractResult.push_back(currPwhList);
	}


	//go through each face
	//when the index of plane >0, this index of the plane that this face lies on is also 
	//that of the curve network in vector<CurveNetwork> vecTempCN
	for (int i=0;i<this->iSSfacenum;i++)
	{
		PolygonOnFace currentPOF;
		if (this->vecResortFace.at(i).bBoundaryFace)//face on the bounding box
		{
			this->vecPOF.push_back(currentPOF);
		}
		else//face of the plane that CN lies on
		{
			//get the vertices of the face
			vector<Point_3> currentFace=this->vecResortFace.at(i).vecFaceVertex;
			//get the curve network on this plane
			CurveNetwork currentCN=this->vecTempCN.at(this->vecSSface_planeindex.at(i));
			Pwh_list_2 CSSubtractResult=vecCSSubtractResult.at(this->vecSSface_planeindex.at(i));
			//bool bResult=IntersectCnFace(currentFace,currentCN,currentPOF);
			IntersectCnFace(currentFace,currentCN,CSSubtractResult,currentPOF);
			
			//align currentPOF with currentCN & currentFace
			AlignPOFData(currentCN,currentFace,currentPOF);

			this->vecPOF.push_back(currentPOF);
		}
		DBWindowWrite("%d face belongs to %d plane\n",i,this->vecSSface_planeindex.at(i));
	}

	//test
	//FILE* pFile=fopen("Ori CS.txt","at");
	//for (unsigned int i=0;i<this->vecTempCN.size();i++)
	//{
	//	for (unsigned int j=0;j<this->vecTempCN.at(i).Profile3D.size();j++)
	//	{
	//		for (unsigned int k=0;k<this->vecTempCN.at(i).Profile3D.at(j).size();k++)
	//		{
	//			Point_3 currPoint=this->vecTempCN.at(i).Profile3D.at(j).at(k);
	//			fprintf(pFile,"%f %f %f\n",currPoint.x(),currPoint.y(),currPoint.z());
	//		}
	//		fprintf(pFile,"\n");
	//	}
	//	fprintf(pFile,"\n");
	//}
	//fclose(pFile);
}

void KW_CS2Surf::SubtractInnerHole(CurveNetwork InputCN,Pwh_list_2& CSSubtractResult)
{
	for (unsigned int i=0;i<InputCN.Profile2D.size();i++)
	{
		if (InputCN.CurveInOut.at(i)!=-1)//inner of a hole
		{
			continue;
		}
		Polygon_with_holes_2 currOutCS;
		currOutCS.outer_boundary()=InputCN.Profile2D.at(i);
		Pwh_list_2 currSubResult;
		currSubResult.push_back(currOutCS);
		//check all the inner holes inside this outer profile
		for (unsigned int j=0;j<InputCN.CurveInOut.size();j++)//the hole index may be ahead of the outter boundary index
		//for (unsigned int j=i+1;j<InputCN.CurveInOut.size();j++)
		{
			if (InputCN.CurveInOut.at(j)!=i)//this hole is NOT inside the outer of hole/circle
			{
				continue;
			}
			//change the hole from cw to ccw to compute intersection
			InputCN.Profile2D.at(j).reverse_orientation();
			Polygon_with_holes_2 currInnerCS;
			currInnerCS.outer_boundary()=InputCN.Profile2D.at(j);
			for (Pwh_list_2::iterator SubIter=currSubResult.begin();SubIter!=currSubResult.end();SubIter++)
			{
				bool bOneIntersect=GeometryAlgorithm::CGAL_do_intersect(*SubIter,currInnerCS);
				if (bOneIntersect)//has intersection, so continue the subtraction
				{
					Pwh_list_2 TempResult;
					GeometryAlgorithm::CGAL_difference(*SubIter,currInnerCS,TempResult);
					if (!TempResult.empty())
					{
						//erase the current Outer Pwh and insert new results
						currSubResult.erase(SubIter);
						currSubResult.insert(currSubResult.end(),TempResult.begin(),TempResult.end());
					}
					else
					{
						DBWindowWrite("Unable to compute difference\n");
					}
					break;
				}
			}
		}
		CSSubtractResult.insert(CSSubtractResult.end(),currSubResult.begin(),currSubResult.end());
	}
}

bool KW_CS2Surf::IntersectCnFace(vector<Point_3>& InputFace,CurveNetwork InputCN,Pwh_list_2 CSSubtractResult,PolygonOnFace& InOutPOF)
{
	//ensure the square is ccw
	Polygon_2 Face2D;
	//GeometryAlgorithm::PlanarPolygonToXOYCCW(InputFace,Face2D,InputCN.ProfilePlaneType);
	GeometryAlgorithm::PlanarPolygonToXOYCCW(InputFace,Face2D,InputCN.plane);

	//intersect the subtraction result and face
	list<bool> listBIntersectFace;
	IntersectCSSubFace(Face2D,CSSubtractResult,listBIntersectFace);

	//convert 2D polygon back to 3D
	Polygon2Dto3D(InputCN,CSSubtractResult,listBIntersectFace,InOutPOF);

	return true;
}


void KW_CS2Surf::IntersectCSSubFace(Polygon_2 Face2D,Pwh_list_2& CSSubtractResult,list<bool>& listBIntersectFace)
{
	//get the edges of ss face
	vector<Segment_2> vecFaceEdge;
	for (unsigned int i=0;i<Face2D.size();i++)
	{
		Segment_2 currSeg(Face2D.vertex(i),Face2D.vertex((i+1)%Face2D.size()));
		vecFaceEdge.push_back(currSeg);
	}

	Pwh_list_2 CSFaceResult;
	for (Pwh_list_2::iterator PwhIter=CSSubtractResult.begin();PwhIter!=CSSubtractResult.end();PwhIter++)
	{
		//get the intersection between the CS subtraction result and the ss face
		Pwh_list_2 IntersectResult;
		GeometryAlgorithm::CGAL_intersection(Face2D, *PwhIter, IntersectResult);
		if (IntersectResult.empty())
		{
			continue;
		}
		//remove duplicated points first
		GeometryAlgorithm::RemoveCGALInterDupPoints(IntersectResult);
		//judge if each Pwh intersects with the face or not
		for (Pwh_list_2::iterator ResultIter=IntersectResult.begin();ResultIter!=IntersectResult.end();ResultIter++)
		{
			bool bIntersect=false;
			for (Vertex_iterator_2 VerIter=ResultIter->outer_boundary().vertices_begin();
				VerIter!=ResultIter->outer_boundary().vertices_end();VerIter++)
			{
				for (Edge_const_iterator_2 EdgeIter=Face2D.edges_begin();EdgeIter!=Face2D.edges_end();EdgeIter++)
				{
					double dDist=CGAL::squared_distance(*VerIter,*EdgeIter);
					if (dDist<5)
					{
						bIntersect=true;
						break;
					}
				}
				if (bIntersect)
				{
					break;
				}
			}
			listBIntersectFace.push_back(bIntersect);
		}
		CSFaceResult.insert(CSFaceResult.end(),IntersectResult.begin(),IntersectResult.end());
	}
	assert(CSFaceResult.size()==listBIntersectFace.size());
	CSSubtractResult=CSFaceResult;
}

void KW_CS2Surf::Polygon2Dto3D(CurveNetwork InputCN,Pwh_list_2 PwhResult2D,list<bool> listBIntersectFace,PolygonOnFace& InOutPOF)
{
	if (listBIntersectFace.empty())
	{
		return;
	}

	list<bool>::iterator ListBoolIter=listBIntersectFace.begin();

	Pwh_list_3 PwhResult3D;
	for (Pwh_list_2::const_iterator  Pwh_list_2_Iter=PwhResult2D.begin();Pwh_list_2_Iter!=PwhResult2D.end();Pwh_list_2_Iter++,ListBoolIter++)
	{
		//Pwh 3d
		Polygon_with_holes_3 currPwh3d;
		currPwh3d.bIntersectSSFace=*ListBoolIter;

		//outer polygon
		vector<Point_3> OuterPoly3D;
		Polygon_2 OuterPoly2D=Pwh_list_2_Iter->outer_boundary();

		//test
		//GeometryAlgorithm::PrintCGALPolygon_2toFile("outbnd.txt",OuterPoly2D);

		GeometryAlgorithm::XOYPolygonTo3DPlanar(OuterPoly2D,OuterPoly3D,InputCN.plane);
		currPwh3d.outer_boundary=OuterPoly3D;

		//inner polygons
		vector<vector<Point_3>> vecInnerPoly3D;
		for (Hole_const_iterator_2 HoleIter=(*Pwh_list_2_Iter).holes_begin();HoleIter!=(*Pwh_list_2_Iter).holes_end();HoleIter++)
		{
			vector<Point_3> InnerPoly3D;
			Polygon_2 InnerPoly2D=*HoleIter;
			//test
			//GeometryAlgorithm::PrintCGALPolygon_2toFile("inhole.txt",InnerPoly2D);
			//convert and save
			GeometryAlgorithm::XOYPolygonTo3DPlanar(InnerPoly2D,InnerPoly3D,InputCN.plane);
			vecInnerPoly3D.push_back(InnerPoly3D);
		}
		currPwh3d.inner_hole=vecInnerPoly3D;
		PwhResult3D.push_back(currPwh3d);
	}
	InOutPOF.PwhList2D=PwhResult2D;
	InOutPOF.PwhList3D=PwhResult3D;
}

void KW_CS2Surf::AlignPOFData(CurveNetwork InCN,vector<Point_3> InFace,PolygonOnFace& InOutPOF)
{
	vector<Point_3> vecCNPoint;
	////test
	//FILE* pFileCN=fopen("CN data.txt","w");
	for (unsigned int i=0;i<InCN.Profile3D.size();i++)
	{
		for (unsigned int j=0;j<InCN.Profile3D.at(i).size();j++)
		{
			vecCNPoint.push_back(InCN.Profile3D.at(i).at(j));
			////test
			//fprintf(pFileCN,"%f %f %f\n",InCN.Profile3D.at(i).at(j).x(),InCN.Profile3D.at(i).at(j).y(),InCN.Profile3D.at(i).at(j).z());
		}
	}
	////test
	//fclose(pFileCN);
	////test
	//FILE* pFileFace=fopen("SS Face Data.txt","w");
	//for (unsigned int i=0;i<InFace.size();i++)
	//{
	//	fprintf(pFileFace,"%f %f %f\n",InFace.at(i).x(),InFace.at(i).y(),InFace.at(i).z());
	//}
	//fclose(pFileFace);

	//find the nearst point in InOutPOF and in vecCNPoint & InFace
	//align the former to the latter
	////test
	//FILE* pFileB=fopen("data to align.txt","w");
	//FILE* pFileA=fopen("data aligned.txt","w");
	for (Pwh_list_3::iterator PwhListIter=InOutPOF.PwhList3D.begin();PwhListIter!=InOutPOF.PwhList3D.end();PwhListIter++)
	{
		for (unsigned int i=0;i<PwhListIter->outer_boundary.size();i++)
		{
			////test
			//fprintf(pFileB,"%f %f %f\n",PwhListIter->outer_boundary.at(i).x(),PwhListIter->outer_boundary.at(i).y(),PwhListIter->outer_boundary.at(i).z());
			//find the nearst point in CN points
			double dMinDist=99999999;
			Point_3 NearestPoint;
			for (unsigned int j=0;j<vecCNPoint.size();j++)
			{
				double dDist=CGAL::squared_distance(PwhListIter->outer_boundary.at(i),vecCNPoint.at(j));
				if (dDist<dMinDist)
				{
					dMinDist=dDist;
					NearestPoint=vecCNPoint.at(j);
				}
			}
			//continue search for the nearst point in CN points
			for (unsigned int j=0;j<InFace.size();j++)
			{
				double dDist=CGAL::squared_distance(PwhListIter->outer_boundary.at(i),InFace.at(j));
				if (dDist<dMinDist)
				{
					dMinDist=dDist;
					NearestPoint=InFace.at(j);
				}
			}
			//if the intersection between CNs is calculated right,every point in InOutPOF should 
			//come from vecCNPoint & InFace
			//if (dDist<2)
			//DBWindowWrite("aligned one point in POF,dist: %f\n",dMinDist);
			//DBWindowWrite("Ori point pos: %f %f %f\n",PwhListIter->outer_boundary.at(j).x(),
			//	PwhListIter->outer_boundary.at(j).y(),PwhListIter->outer_boundary.at(j).z());
			if (dMinDist>15)//6
			{
				DBWindowWrite("corres. point failed to find when in align POF,dist: %f\n",dMinDist);
			}
			else
			{
				//DBWindowWrite("aligned one point in POF,dist: %f\n",dMinDist);
				PwhListIter->outer_boundary.at(i)=NearestPoint;
			}
			//DBWindowWrite("Ori point pos: %f %f %f\n",PwhListIter->outer_boundary.at(j).x(),
			//	PwhListIter->outer_boundary.at(j).y(),PwhListIter->outer_boundary.at(j).z());
			////test
			//fprintf(pFileA,"%f %f %f\n",PwhListIter->outer_boundary.at(j).x(),PwhListIter->outer_boundary.at(j).y(),PwhListIter->outer_boundary.at(j).z());
		}

		for (unsigned int i=0;i<PwhListIter->inner_hole.size();i++)
		{
			for (unsigned int j=0;j<PwhListIter->inner_hole.at(i).size();j++)
			{
				double dMinDist=99999999;
				Point_3 NearestPoint;
				for (unsigned int k=0;k<vecCNPoint.size();k++)
				{
					double dDist=CGAL::squared_distance(PwhListIter->inner_hole.at(i).at(j),vecCNPoint.at(k));
					if (dDist<dMinDist)
					{
						dMinDist=dDist;
						NearestPoint=vecCNPoint.at(k);
					}
				}
				//continue search for the nearst point in CN points
				for (unsigned int k=0;k<InFace.size();k++)
				{
					double dDist=CGAL::squared_distance(PwhListIter->inner_hole.at(i).at(j),InFace.at(k));
					if (dDist<dMinDist)
					{
						dMinDist=dDist;
						NearestPoint=InFace.at(k);
					}
				}
				//if the intersection between CNs is calculated right,every point in InOutPOF should 
				//come from vecCNPoint & InFace
				//if (dDist<2)
				//DBWindowWrite("aligned one point in POF,dist: %f\n",dMinDist);
				//DBWindowWrite("Ori point pos: %f %f %f\n",PwhListIter->outer_boundary.at(j).x(),
				//	PwhListIter->outer_boundary.at(j).y(),PwhListIter->outer_boundary.at(j).z());
				if (dMinDist>15)//6
				{
					DBWindowWrite("corres. point failed to find when in align POF,dist: %f\n",dMinDist);
				}
				else
				{
					//DBWindowWrite("aligned one point in POF,dist: %f\n",dMinDist);
					PwhListIter->inner_hole.at(i).at(j)=NearestPoint;
				}
			}
		}
	}	
	////test
	//fclose(pFileB);
	//fclose(pFileA);
	//bTest=true;
	//DBWindowWrite("test point size: %d\n",this->vecTestPoint.size());
}