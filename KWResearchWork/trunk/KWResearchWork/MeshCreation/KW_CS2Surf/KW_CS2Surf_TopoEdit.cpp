#include "StdAfx.h"
#include "KW_CS2Surf.h"
#include "../../CurveDeform.h"
#include "../../Math/DeTriangulator.h"
#include "../../Math/glu_tesselator.h"
#include "../../OBJHandle.h"
#include "../../PaintingOnMesh.h"

bool KW_CS2Surf::SketchSplit(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport)
{
	//first filter to reduce the num of points on input curve
	GeometryAlgorithm compute;
	compute.ProcessCurverPoint(UserScrPoint,20.0);//15.0
	//get the cylinders selected
	vector<int> vecSelCylin;
	int iSSId=-1;
	bool bResult=SSGetSelCylin(UserScrPoint,modelview,projection,viewport,vecSelCylin,iSSId);
	if (!bResult)
	{
		return false;
	}
	//collect the cylinders in this subspace
	vector<KW_Mesh> vecSplitPoly,vecOtherPoly;
	for (unsigned int i=0;i<vecSelCylin.size();i++)
	{
		vecSplitPoly.push_back(this->vecSinglePoly.at(vecSelCylin.at(i)));
	}
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		if (vecSinglePoly.at(i).GetReserved()==iSSId)
		{
			vector<int>::iterator IntIter=find(vecSelCylin.begin(),vecSelCylin.end(),i);
			if (IntIter==vecSelCylin.end())
			{
				vecOtherPoly.push_back(this->vecSinglePoly.at(i));
			}
		}
	}
	//compute the union
	KW_Mesh ResultPolyh;
	ComputeUnionInSubspace(vecSplitPoly,vecOtherPoly,ResultPolyh);
	ResultPolyh.SetReserved(iSSId);
	OBJHandle::UnitizeCGALPolyhedron(ResultPolyh,false,false);
	//update the cylinders stored in vecUnionPoly
	for (unsigned int i=0;i<this->vecUnionPoly.size();i++)
	{
		if (this->vecUnionPoly.at(i).GetReserved()==iSSId)
		{
			this->vecUnionPoly.at(i)=ResultPolyh;
			break;
		}
	}
	//remove face points
	vector<int>::iterator IntIter=find(this->vecSubSurfSSId.begin(),this->vecSubSurfSSId.end(),iSSId);
	assert(IntIter!=this->vecSubSurfSSId.end());
	int iIndex=distance(this->vecSubSurfSSId.begin(),IntIter);
	vector<Point_3> vecNewSubPoint;
	vector<vector<int>> vecNewSubSurf;
	RemoveFaceTriangles(iSSId,ResultPolyh,this->vecvecvecFacePoint.at(iIndex),vecNewSubPoint,vecNewSubSurf);
	//update the points and triangles
	this->vecvecSubPoint.at(iIndex)=vecNewSubPoint;
	this->vecvecSubSurf.at(iIndex)=vecNewSubSurf;
	//stitch surface
	//stitch the submeshes together
	StitchMesh(this->vecvecSubPoint,this->vecvecSubSurf,this->InitPolyh);
	PostProcMesh();

	return true;
}

bool KW_CS2Surf::SSGetSelCylin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport,vector<int>& vecSelCylin,int& iSSId)
{
	CPaintingOnMesh Paint;
	//get the first selected cylinder
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		//judge if in the same subspace with the first selected cylinder
		if (iSSId!=-1 && this->vecSinglePoly.at(i).GetReserved()!=iSSId)
		{
			continue;
		}
		for (unsigned int j=0;j<UserScrPoint.size();j++)
		{
			//don't forget the coordinates of the cylinder vertices have been scaled
			bool bIntersect=Paint.PaintingScrPointOnFrontalMeshQuick(this->vecSinglePoly.at(i),UserScrPoint.at(j),
				modelview,projection,viewport,DIM/PROCSIZE);
			if (bIntersect)
			{
				vecSelCylin.push_back(i);
				if (iSSId==-1)
				{
					iSSId=this->vecSinglePoly.at(i).GetReserved();
				}
				break;
			}
		}
	}
	if (vecSelCylin.size()<2)
	{
		return false;
	}
	return true;
}

//sketch to join the originally irrelevant components (cylinders)
bool KW_CS2Surf::SketchJoin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport)
{
	//get the cylinders selected
	vector<int> vecSelCylin;
	int iSSId=-1;
	Point_3 DstPoint;
	bool bResult=SJGetSelCylin(UserScrPoint,modelview,projection,viewport,vecSelCylin,iSSId,DstPoint);
	if (!bResult)
	{
		return false;
	}
	//stretch the first selected cylinder
	SJStretchCylin(this->vecSinglePoly.at(vecSelCylin.front()),DstPoint);
	//collect the cylinders in this subspace
	vector<KW_Mesh> vecCurrCylin;
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		if (vecSinglePoly.at(i).GetReserved()==iSSId)
		{
			vecCurrCylin.push_back(vecSinglePoly.at(i));
		}
	}
	//compute the union
	KW_Mesh ResultPolyh;
	ComputeUnionInSubspace(vecCurrCylin,ResultPolyh);
	ResultPolyh.SetReserved(iSSId);
	OBJHandle::UnitizeCGALPolyhedron(ResultPolyh,false,false);
	//update the cylinders stored in vecUnionPoly
	for (unsigned int i=0;i<this->vecUnionPoly.size();i++)
	{
		if (this->vecUnionPoly.at(i).GetReserved()==iSSId)
		{
			this->vecUnionPoly.at(i)=ResultPolyh;
			break;
		}
	}
	//remove face points
	vector<int>::iterator IntIter=find(this->vecSubSurfSSId.begin(),this->vecSubSurfSSId.end(),iSSId);
	assert(IntIter!=this->vecSubSurfSSId.end());
	int iIndex=distance(this->vecSubSurfSSId.begin(),IntIter);
	vector<Point_3> vecNewSubPoint;
	vector<vector<int>> vecNewSubSurf;
	RemoveFaceTriangles(iSSId,ResultPolyh,this->vecvecvecFacePoint.at(iIndex),vecNewSubPoint,vecNewSubSurf);
	//update the points and triangles
	this->vecvecSubPoint.at(iIndex)=vecNewSubPoint;
	this->vecvecSubSurf.at(iIndex)=vecNewSubSurf;
	//stitch surface
	//stitch the submeshes together
	StitchMesh(this->vecvecSubPoint,this->vecvecSubSurf,this->InitPolyh);
	PostProcMesh();

	return true;
}

bool KW_CS2Surf::SJGetSelCylin(vector<CPoint> UserScrPoint,GLdouble* modelview,GLdouble* projection,GLint* viewport,vector<int>& vecSelCylin,int& iSSId,Point_3& DstPoint)
{
	CPaintingOnMesh Paint;
	//get the first selected cylinder
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		//don't forget the coordinates of the cylinder vertices have been scaled
		bool bIntersect=Paint.PaintingScrPointOnFrontalMeshQuick(this->vecSinglePoly.at(i),UserScrPoint.front(),
			modelview,projection,viewport,DIM/PROCSIZE);
		if (bIntersect)
		{
			vecSelCylin.push_back(i);
			break;
		}
	}
	if (vecSelCylin.empty())
	{
		return false;
	}
	//get the second selected cylinder
	vector<Point_3> vecSecPoint;
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		//if the first selected cylinder,continue
		if (i==vecSelCylin.front())
		{
			continue;
		}
		//don't forget the coordinates of the cylinder vertices have been scaled
		bool bIntersect=Paint.PaintingScrPointOnMesh(this->vecSinglePoly.at(i),UserScrPoint.back(),vecSecPoint,modelview,projection,viewport,DIM/PROCSIZE);
		if (bIntersect)
		{
			//not in the same subspace with the first selected cylinder
			if (this->vecSinglePoly.at(i).GetReserved()!=this->vecSinglePoly.at(vecSelCylin.front()).GetReserved())
			{
				vecSecPoint.clear();
				continue;
			}
			else
			{
				vecSelCylin.push_back(i);
				break;
			}
		}
	}
	if (vecSelCylin.size()!=2)
	{
		return false;
	}
	iSSId=this->vecSinglePoly.at(vecSelCylin.front()).GetReserved();
	DstPoint=CGAL::centroid(vecSecPoint.begin(),vecSecPoint.end());
	return true;
}

void KW_CS2Surf::SJStretchCylin(KW_Mesh& InOutCylin,Point_3 NewFaceCenter)
{
	//don't fotget to judge if the stretched cylinder intersects with the subspace
	//omit it temporarily




	//shrink to the original face point gradually if intersects with subspace






	Vector_3 VectToMove=NewFaceCenter-InOutCylin.GetExtruCenter();
	for (Vertex_iterator VerIter=InOutCylin.vertices_begin();VerIter!=InOutCylin.vertices_end();VerIter++)
	{
		if (VerIter->GetReserved()==1)//the extruded vertices
		{
			VerIter->point()=VerIter->point()+VectToMove;
		}
	}
	//don't fotget to update extruded center
	InOutCylin.SetExtruCenter(NewFaceCenter);
	OBJHandle::UnitizeCGALPolyhedron(InOutCylin,false,false);
}