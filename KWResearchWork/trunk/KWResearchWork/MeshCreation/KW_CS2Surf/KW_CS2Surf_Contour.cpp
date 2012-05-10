#include "StdAfx.h"
#include "KW_CS2Surf.h"

void KW_CS2Surf::readContourFromVec(vector<CurveNetwork> vecCurveNetwork,float* fPreSetBBox,vector<vector<Point_3>>& MeshBoundingProfile3D)
{
	Ctr2SufManager::readContourFromVec(vecCurveNetwork,fPreSetBBox,MeshBoundingProfile3D);
	this->vecTempCN.clear();
	//adjust the plane parameters and vertex positions in this->vecTempCN, according to the resize() function of Ctr2SufManager
	this->vecTempCN=vecCurveNetwork;
	for (unsigned int i=0;i<this->vecTempCN.size();i++)
	{
		DBWindowWrite("original plane parameter: a:%.4f b:%.4f c:%.4f d:%.4f\n",this->vecTempCN.at(i).plane.a(),this->vecTempCN.at(i).plane.b(),
			this->vecTempCN.at(i).plane.c(),this->vecTempCN.at(i).plane.d());
		//attention: the "d" in plane parameters of CGAL and Ctr2SufManager (the pparam parameter) are opposite
		this->vecTempCN.at(i).plane=Plane_3(this->pparam[4*i],this->pparam[4*i+1],this->pparam[4*i+2],-this->pparam[4*i+3]);
		DBWindowWrite("processed plane parameter: a:%.4f b:%.4f c:%.4f d:%.4f\n",this->vecTempCN.at(i).plane.a(),this->vecTempCN.at(i).plane.b(),
			this->vecTempCN.at(i).plane.c(),this->vecTempCN.at(i).plane.d());
		for (unsigned int j=0;j<this->vecTempCN.at(i).Profile3D.size();j++)
		{
			for (unsigned int k=0;k<this->vecTempCN.at(i).Profile3D.at(j).size();k++)
			{
				double dOriginalX=this->vecTempCN.at(i).Profile3D.at(j).at(k).x();
				double dOriginalY=this->vecTempCN.at(i).Profile3D.at(j).at(k).y();
				double dOriginalZ=this->vecTempCN.at(i).Profile3D.at(j).at(k).z();
				this->vecTempCN.at(i).Profile3D.at(j).at(k)=Point_3(dOriginalX*PROCSIZE,dOriginalY*PROCSIZE,dOriginalZ*PROCSIZE);
			}
			//update 2D polygon for further use
			Polygon_2 NewProfile2D;
			//GeometryAlgorithm::PlanarPolygonToXOY(this->vecTempCN.at(i).Profile3D.at(j),NewProfile2D,this->vecTempCN.at(i).ProfilePlaneType);
			GeometryAlgorithm::PlanarPolygonToXOY(this->vecTempCN.at(i).Profile3D.at(j),NewProfile2D,this->vecTempCN.at(i).plane);
			//ensure only the geometry of the Profile2D is updated, the orientation does NOT change
			assert(NewProfile2D.orientation()==this->vecTempCN.at(i).Profile2D.at(j).orientation());
			this->vecTempCN.at(i).Profile2D.at(j)=NewProfile2D;
		}
	}
	//do not lower precision to ensure the consistency of the cross sections
	//LowerPrecision();
}

void KW_CS2Surf::LowerPrecision()
{
	for (unsigned int i=0;i<this->vecTempCN.size();i++)
	{
		for (unsigned int j=0;j<this->vecTempCN.at(i).Profile3D.size();j++)
		{
			for (unsigned int k=0;k<this->vecTempCN.at(i).Profile3D.at(j).size();k++)
			{
				float fOriginalX=this->vecTempCN.at(i).Profile3D.at(j).at(k).x();
				float fOriginalY=this->vecTempCN.at(i).Profile3D.at(j).at(k).y();
				float fOriginalZ=this->vecTempCN.at(i).Profile3D.at(j).at(k).z();
				this->vecTempCN.at(i).Profile3D.at(j).at(k)=Point_3(fOriginalX,fOriginalY,fOriginalZ);
			}
		}
		for (unsigned int j=0;j<this->vecTempCN.at(i).Profile2D.size();j++)
		{
			for (Vertex_iterator_2 VerIter=this->vecTempCN.at(i).Profile2D.at(j).vertices_begin();
				VerIter!=this->vecTempCN.at(i).Profile2D.at(j).vertices_end();VerIter++)
			{
				float fX=(*VerIter).x();
				float fY=(*VerIter).y();
				*VerIter=Point_2(fX,fY);
			}
		}
		float fPlaneA=this->vecTempCN.at(i).plane.a();
		float fPlaneB=this->vecTempCN.at(i).plane.b();
		float fPlaneC=this->vecTempCN.at(i).plane.c();
		float fPlaneD=this->vecTempCN.at(i).plane.d();
		this->vecTempCN.at(i).plane=Plane_3(fPlaneA,fPlaneB,fPlaneC,fPlaneD);
	}

	//vector<Point_3> vecTotalPoint;
	//vector<vector<int>> vecInd;
	//for (unsigned int i=0;i<this->vecTempCN.size();i++)
	//{
	//	CurveNetwork currentCN=this->vecTempCN.at(i);
	//	for (unsigned int j=0;j<currentCN.Profile3D.size();j++)
	//	{
	//		vector<Point_3> currentCS=currentCN.Profile3D.at(j);
	//		for (unsigned int k=0;k<currentCS.size();k++)
	//		{
	//			vecTotalPoint.push_back(currentCS.at(k));
	//			vector<int> temp;
	//			temp.push_back(i);temp.push_back(j);temp.push_back(k);
	//			vecInd.push_back(temp);
	//		}
	//	}
	//}
	//for (unsigned int i=0;i<vecTotalPoint.size();i++)
	//{
	//	for (unsigned int j=0;j<vecTotalPoint.size();j++)
	//	{
	//		if (i==j || vecTotalPoint.at(i)==vecTotalPoint.at(j))
	//		{
	//			continue;
	//		}
	//		double dDist=CGAL::squared_distance(vecTotalPoint.at(i),vecTotalPoint.at(j));
	//		if (dDist<10)
	//		{
	//			DBWindowWrite("two CN points are quite near,distance: %f\n",dDist);
	//			DBWindowWrite("Point1: %f %f %f\n",vecTotalPoint.at(i).x(),vecTotalPoint.at(i).y(),vecTotalPoint.at(i).z());
	//			DBWindowWrite("Point2: %f %f %f\n",vecTotalPoint.at(j).x(),vecTotalPoint.at(j).y(),vecTotalPoint.at(j).z());
	//			DBWindowWrite("index of point1: %d %d %d\n",vecInd.at(i).at(0),vecInd.at(i).at(1),vecInd.at(i).at(2));
	//			DBWindowWrite("index of point2: %d %d %d\n",vecInd.at(j).at(0),vecInd.at(j).at(1),vecInd.at(j).at(2));
	//		}
	//	}
	//}
}