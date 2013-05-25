#include "StdAfx.h"
#include "KW_CS2Surf.h"
#include "../../OBJHandle.h"

void KW_CS2Surf::clearStlSubspaceInfo()
{
	this->vecSSver.clear();
	this->vecSSedge.clear();
	this->iSSfacenum=0;
	this->vecSSfaceedgenum.clear();
	this->vecvecSSface.clear();
	this->vecSSface_planeindex.clear();
	this->iSSspacenum=0;
	this->vecSSspacefacenum.clear();
	this->vecvecSSspace.clear();
}

void KW_CS2Surf::partitionStl()
{
	clearFaceContour();
	//clear the old result, a new generation start from here!
	//because other data may be dependent on the value of partition result, 
	//so before clearpartition, other data must be cleared too!
	clearPartition();

	clearStlSubspaceInfo();

	//temporary data
	floatvector tssver;
	intvector tssedge;
	vector<intvector> tssface;
	intvector tssface_planeindex;
	vector<intvector> tssspace;
	vector<intvector> tssspace_planeside;

	//partition!
	SpacePartitioner partitioner;

	partitioner.partition( planenum, pparam, pbbox, enlargeratio,
		tssver, tssedge, tssface, tssface_planeindex, tssspace, tssspace_planeside);

	//copy them out
	this->vecSSver=tssver;
	this->vecSSedge=tssedge;
	this->iSSfacenum=tssface.size();
	this->vecSSface_planeindex=tssface_planeindex;
	this->vecvecSSface=tssface;
	for (int i=0;i<this->iSSfacenum;i++)
	{
		this->vecSSfaceedgenum.push_back(vecvecSSface.at(i).size());
	}
	this->iSSspacenum=tssspace.size();
	this->vecvecSSspace=tssspace;
	for (int i=0;i<this->iSSspacenum;i++)
	{
		this->vecSSspacefacenum.push_back(this->vecvecSSspace.at(i).size());
	}

	//clear the temporary data   
	tssver.clear();
	tssedge.clear();

	int size = tssface.size();
	for( int i = 0; i < size; i ++ )
		tssface[ i ].clear();
	tssface.clear();
	tssface_planeindex.clear();

	size = tssspace.size();
	for( int i = 0; i < size; i ++)
	{
		tssspace[ i ].clear();
		tssspace_planeside[ i ].clear();
	}
	tssspace.clear();
	tssspace_planeside.clear();	

	SortFaceInfo();
}

//void KW_CS2Surf::SortFaceInfo()
//{
//	//fill in basic info for each face
//	for (int i=0;i<this->iSSfacenum;i++)
//	{
//		ResortedFace currentFace;
//		if (this->vecSSface_planeindex.at(i)<0)//face on the bounding box
//		{
//			currentFace.bBoundaryFace=true;
//		}
//		else
//		{
//			currentFace.bBoundaryFace=false;
//			currentFace.iFacePlaneID=this->vecSSface_planeindex.at(i);
//			//get the four vertices of the face
//			set<Point_3> setCurrentFace;
//			assert(this->vecSSfaceedgenum.at(i)==4);
//			for (int j=0;j<this->vecSSfaceedgenum[i];j++)//ssfaceedgenum[i] should == 4
//			{
//				int iEdgeInd=this->vecvecSSface.at(i).at(j);
//				int iVerBeginInd=this->vecSSedge.at(2*iEdgeInd);
//				int iVerEndInd=this->vecSSedge.at(2*iEdgeInd+1);
//				Point_3 VerBegin=Point_3(this->vecSSver.at(3*iVerBeginInd),this->vecSSver.at(3*iVerBeginInd+1),this->vecSSver.at(3*iVerBeginInd+2));
//				Point_3 VerEnd=Point_3(this->vecSSver.at(3*iVerEndInd),this->vecSSver.at(3*iVerEndInd+1),this->vecSSver.at(3*iVerEndInd+2));
//				setCurrentFace.insert(VerBegin);
//				setCurrentFace.insert(VerEnd);
//			}
//			vector<Point_3> tempFaceVer;
//			tempFaceVer.resize(setCurrentFace.size());
//			copy(setCurrentFace.begin(),setCurrentFace.end(),tempFaceVer.begin());
//			GetSquareFace(tempFaceVer);
//			currentFace.vecFaceVertex=tempFaceVer;
//		}
//		this->vecResortFace.push_back(currentFace);
//	}
//	//fill in the properties related to subspace for each face
//	for (int i=0;i<this->iSSspacenum;i++)
//	{
//		for (int j=0;j<vecSSspacefacenum.at(i);j++)
//		{
//			int iFaceId=this->vecvecSSspace.at(i).at(j);
//			//if boundary face,do not calculate at all
//			if (this->vecResortFace.at(iFaceId).bBoundaryFace)
//			{
//				continue;
//			}
//			this->vecResortFace.at(iFaceId).vecSubspaceId.push_back(i);
//			bool bOrient=false;
//			Vector_3 HeightVec=CGAL::NULL_VECTOR;
//			GetFacePara(iFaceId,i,bOrient,HeightVec);
//			this->vecResortFace.at(iFaceId).vecOrient.push_back(bOrient);
//			this->vecResortFace.at(iFaceId).vecHeightVect.push_back(HeightVec);
//		}
//
//	}
//	assert(this->vecResortFace.size()==this->iSSfacenum);
//	for (unsigned int i=0;i<vecResortFace.size();i++)
//	{
//		if (!vecResortFace.at(i).bBoundaryFace)
//		{
//			assert(this->vecResortFace.at(i).vecFaceVertex.size()==4);
//			assert(this->vecResortFace.at(i).vecSubspaceId.size()==2);
//			assert(this->vecResortFace.at(i).vecOrient.size()==2);
//			assert(this->vecResortFace.at(i).vecHeightVect.size()==2);
//		}
//	}
//}

void KW_CS2Surf::SortFaceInfo()
{
	//fill in basic info for each face
	for (int i=0;i<iSSfacenum;i++)
	{
		ResortedFace currentFace;
		currentFace.bBoundaryFace=false;
		if (vecSSface_planeindex.at(i)<0)//face on the bounding box
		{
			currentFace.bBoundaryFace=true;
		}
		currentFace.iFacePlaneID=vecSSface_planeindex.at(i);
		//get the vertices of the face in sequantial order
		vector<Int_Int_Pair> vecEdges;
		for (int j=0;j<vecSSfaceedgenum[i];j++)
		{
			int iEdgeInd=vecvecSSface.at(i).at(j);
			int iVerBeginInd=vecSSedge.at(2*iEdgeInd);
			int iVerEndInd=vecSSedge.at(2*iEdgeInd+1);
			vecEdges.push_back(make_pair(iVerBeginInd,iVerEndInd));
		}
		vector<int> vecVerInd;//indices of vertices in sequential order
		GetPolygonFace(vecEdges,vecVerInd);
		currentFace.vecFaceVertexInd=vecVerInd;
		vector<Point_3> FaceVer;
		for (unsigned int j=0;j<vecVerInd.size();j++)
		{
			Point_3 CurrentVer=Point_3(vecSSver.at(3*vecVerInd.at(j)),vecSSver.at(3*vecVerInd.at(j)+1),vecSSver.at(3*vecVerInd.at(j)+2));
			FaceVer.push_back(CurrentVer);
		}
		currentFace.vecFaceVertex=FaceVer;
		this->vecResortFace.push_back(currentFace);
	}
	//fill in the properties related to subspace for each face
	for (int i=0;i<iSSspacenum;i++)
	{
		//get the center of the subspace
		Point_3 SSCenter;
		GetSSCenter(i,SSCenter);
		//test
		//this->vecTestPoint.push_back(SSCenter);
		for (int j=0;j<vecSSspacefacenum.at(i);j++)
		{
			int iFaceId=vecvecSSspace.at(i).at(j);
			////if boundary face,do not calculate at all
			//if (this->vecResortFace.at(iFaceId).bBoundaryFace)
			//{
			//	continue;
			//}
			this->vecResortFace.at(iFaceId).vecSubspaceId.push_back(i);
			bool bOrient=false;
			Vector_3 HeightVec=CGAL::NULL_VECTOR;
			GetFacePara(iFaceId,i,SSCenter,bOrient,HeightVec);
			this->vecResortFace.at(iFaceId).vecOrient.push_back(bOrient);
			this->vecResortFace.at(iFaceId).vecHeightVect.push_back(HeightVec);
		}

	}
	assert(this->vecResortFace.size()==iSSfacenum);
	for (unsigned int i=0;i<vecResortFace.size();i++)
	{
		if (!vecResortFace.at(i).bBoundaryFace)
		{
			//assert(this->vecResortFace.at(i).vecFaceVertex.size()==4);
			assert(this->vecResortFace.at(i).vecSubspaceId.size()==2);
			assert(this->vecResortFace.at(i).vecOrient.size()==2);
			assert(this->vecResortFace.at(i).vecHeightVect.size()==2);
		}
		else
		{
			assert(this->vecResortFace.at(i).vecSubspaceId.size()==1);
			assert(this->vecResortFace.at(i).vecOrient.size()==1);
			assert(this->vecResortFace.at(i).vecHeightVect.size()==1);
		}
	}
	////compute mesh formed by each subspace
	//GetSSMesh();
}

//void KW_CS2Surf::GetFacePara(int iFaceId,int iSubspaceId,bool& bOrient,Vector_3& HeightVec)
//{
//	//get the four vertices of the current face
//	vector<Point_3> vecCurrentFace=this->vecResortFace.at(iFaceId).vecFaceVertex;
//
//	//get an arbitrary edge of the subspace which has only one point on the face[iFaceId]
//	//record the length of the edge(the height w.r.t. the face in the subspace) and the vertex which is not on the face
//	Point_3 ArbitPoint;
//	//iterate other faces of the subspace 
//	for (int i=0;i<this->vecSSspacefacenum.at(iSubspaceId);i++)
//	{
//		int iOtherFaceId=this->vecvecSSspace.at(iSubspaceId).at(i);
//		if (iOtherFaceId==iFaceId)//if it is the current face
//		{
//			continue;
//		}
//		for (int j=0;j<this->vecSSfaceedgenum.at(iOtherFaceId);j++)//ssfaceedgenum[i] should == 4
//		{
//			int iEdgeInd=this->vecvecSSface.at(iOtherFaceId).at(j);
//			int iVerBeginInd=this->vecSSedge.at(2*iEdgeInd);
//			int iVerEndInd=this->vecSSedge.at(2*iEdgeInd+1);
//			Point_3 VerBegin=Point_3(this->vecSSver.at(3*iVerBeginInd),this->vecSSver.at(3*iVerBeginInd+1),this->vecSSver.at(3*iVerBeginInd+2));
//			Point_3 VerEnd=Point_3(this->vecSSver.at(3*iVerEndInd),this->vecSSver.at(3*iVerEndInd+1),this->vecSSver.at(3*iVerEndInd+2));
//			//judge if one is on the face and the other is not
//			vector<Point_3>::iterator pFind0=find(vecCurrentFace.begin(),vecCurrentFace.end(),VerBegin);
//			vector<Point_3>::iterator pFind1=find(vecCurrentFace.begin(),vecCurrentFace.end(),VerEnd);
//			if (pFind0==vecCurrentFace.end()&&pFind1!=vecCurrentFace.end())
//			{
//				ArbitPoint=VerBegin;
//				HeightVec=VerBegin-VerEnd;
//				break;
//			}
//			else if (pFind0!=vecCurrentFace.end()&&pFind1==vecCurrentFace.end())
//			{
//				ArbitPoint=VerEnd;
//				HeightVec=VerEnd-VerBegin;
//				break;
//			}
//		}
//		if (HeightVec!=CGAL::NULL_VECTOR)
//		{
//			break;
//		}
//	}
//	assert(HeightVec!=CGAL::NULL_VECTOR);
//	//judge the type of the plane of the current face
//	//since the bounding box is also considered,vector<CurveNetwork> vecTempCN cannot be used for the calculation
//	//bOrient has nothing to do with the order of the vertices of the face
//	Point_3 Point0=vecCurrentFace.at(0);
//	Point_3 Point1=vecCurrentFace.at(1);
//	Point_3 Point2=vecCurrentFace.at(2);
//	Point_3 Point3=vecCurrentFace.at(3);
//	if (Point0.x()==Point1.x() && Point1.x()==Point2.x() && Point2.x()==Point3.x())//yoz plane
//	{
//		if (ArbitPoint.x()<Point0.x())
//		{
//			bOrient=false;
//		}
//		else
//		{
//			bOrient=true;
//		}
//	}
//	else if (Point0.y()==Point1.y() && Point1.y()==Point2.y() && Point2.y()==Point3.y())//xoz plane
//	{
//		if (ArbitPoint.y()<Point0.y())
//		{
//			bOrient=false;
//		}
//		else
//		{
//			bOrient=true;
//		}
//	}
//	else if (Point0.z()==Point1.z() && Point1.z()==Point2.z() && Point2.z()==Point3.z())//xoy plane
//	{
//		if (ArbitPoint.z()<Point0.z())
//		{
//			bOrient=false;
//		} 
//		else
//		{
//			bOrient=true;
//		}
//	}
//	else
//	{
//		DBWindowWrite("plane type judge error\n");
//		DBWindowWrite("X: %f %f %f %f\n",Point0.x(),Point1.x(),Point2.x(),Point3.x());
//		DBWindowWrite("Y: %f %f %f %f\n",Point0.y(),Point1.y(),Point2.y(),Point3.y());
//		DBWindowWrite("Z: %f %f %f %f\n",Point0.z(),Point1.z(),Point2.z(),Point3.z());
//	}
//}

void KW_CS2Surf::GetFacePara(int iFaceId,int iSubspaceId,Point_3 SSCenter,bool& bOrient,Vector_3& HeightVec)
{
	//get the indices vertices of the current face
	vector<int> vecCurrFaceVerInd=this->vecResortFace.at(iFaceId).vecFaceVertexInd;
	//get an arbitrary point which does not lie on this face in the subspace
	//use the SSCenter here
	Point_3 ArbitPoint=SSCenter;
	//for (int i=0;i<vecSSspacefacenum.at(iSubspaceId);i++)
	//{
	//	int iOtherFaceId=vecvecSSspace.at(iSubspaceId).at(i);
	//	if (iOtherFaceId==iFaceId)//if it is the current face
	//	{
	//		continue;
	//	}
	//	for (int j=0;j<vecSSfaceedgenum.at(iOtherFaceId);j++)
	//	{
	//		int iEdgeInd=vecvecSSface.at(iOtherFaceId).at(j);
	//		int iVerBeginInd=vecSSedge.at(2*iEdgeInd);
	//		int iVerEndInd=vecSSedge.at(2*iEdgeInd+1);
	//		vector<int>::iterator pFind0=find(vecCurrFaceVerInd.begin(),vecCurrFaceVerInd.end(),iVerBeginInd);
	//		vector<int>::iterator pFind1=find(vecCurrFaceVerInd.begin(),vecCurrFaceVerInd.end(),iVerEndInd);
	//		if (pFind0!=vecCurrFaceVerInd.end() && pFind1!=vecCurrFaceVerInd.end())//same edge,continue
	//		{
	//			continue;
	//		}
	//		else if (pFind0==vecCurrFaceVerInd.end())
	//		{
	//			ArbitPoint=Point_3(vecSSver.at(3*iVerBeginInd),vecSSver.at(3*iVerBeginInd+1),vecSSver.at(3*iVerBeginInd+2));
	//			break;
	//		}
	//		else if (pFind1==vecCurrFaceVerInd.end())
	//		{
	//			ArbitPoint=Point_3(vecSSver.at(3*iVerEndInd),vecSSver.at(3*iVerEndInd+1),vecSSver.at(3*iVerEndInd+2));
	//			break;
	//		}
	//	}
	//}

	//judge the angle between the face normal and the vector pointing from a face vertex to the arbitrary point in the subspace
	Vector_3 currEdgeVec0=this->vecResortFace.at(iFaceId).vecFaceVertex.at(1)-this->vecResortFace.at(iFaceId).vecFaceVertex.at(0);
	Vector_3 currEdgeVec1=this->vecResortFace.at(iFaceId).vecFaceVertex.at(2)-this->vecResortFace.at(iFaceId).vecFaceVertex.at(1);
	Vector_3 currFaceNorm=CGAL::cross_product(currEdgeVec0,currEdgeVec1);
	currFaceNorm=currFaceNorm/sqrt(currFaceNorm.squared_length());
	Point_3 FaceCenter=CGAL::centroid(this->vecResortFace.at(iFaceId).vecFaceVertex.begin(),this->vecResortFace.at(iFaceId).vecFaceVertex.end());
	Vector_3 ConnectVec=ArbitPoint-FaceCenter;//this->vecResortFace.at(iFaceId).vecFaceVertex.at(0);
	double dAngle=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(currFaceNorm,ConnectVec);
	if (dAngle>=0 && dAngle<=90)
	{
		bOrient=false;
		//save the height vec as the normalized face normal temprorarily
		HeightVec=currFaceNorm;
	}
	else
	{
		bOrient=true;
		//save the height vec as the normalized face normal temprorarily
		HeightVec=-currFaceNorm;
	}
	//if not boundary face, then set the length of the height vector
	if (!this->vecResortFace.at(iFaceId).bBoundaryFace)
	{
		//id of the plane that the face belongs to
		int iPlaneId=this->vecResortFace.at(iFaceId).iFacePlaneID;
		//plane
		Plane_3 plane=this->vecTempCN.at(iPlaneId).plane;
		//get the distance between the center point and the plane
		double dDist=sqrt(CGAL::squared_distance(SSCenter,plane));
		HeightVec=HeightVec*dDist;
	}
}

void KW_CS2Surf::GetSSCenter(int iSubspaceId,Point_3& SSCenter)
{
	set<int> setVerId;
	//get the id of the vertices of the subspace
	for (int i=0;i<vecSSspacefacenum.at(iSubspaceId);i++)
	{
		int iFaceId=vecvecSSspace.at(iSubspaceId).at(i);
		for (int j=0;j<vecSSfaceedgenum.at(iFaceId);j++)
		{
			int iEdgeInd=vecvecSSface.at(iFaceId).at(j);
			int iVerBeginInd=vecSSedge.at(2*iEdgeInd);
			int iVerEndInd=vecSSedge.at(2*iEdgeInd+1);
			setVerId.insert(iVerBeginInd);
			setVerId.insert(iVerEndInd);
		}
	}
	//compute the center
	vector<Point_3> vecVerPoint;
	for (set<int>::iterator SetIter=setVerId.begin();SetIter!=setVerId.end();SetIter++)
	{
		vecVerPoint.push_back(Point_3(vecSSver.at(3*(*SetIter)),
			vecSSver.at(3*(*SetIter)+1),
			vecSSver.at(3*(*SetIter)+2)));
	}
	SSCenter=CGAL::centroid(vecVerPoint.begin(),vecVerPoint.end());
}

void KW_CS2Surf::CombineSS()
{
	//iteratively combine until no SS can be combined
	while (true)
	{
		//collect subspaces need to be combined
		//pairs of subspaces to combine
		vector<Int_Int_Pair> vecSpacesToCombine;
		//the only face having CSs on in each subspace need to combine
		vector<int> vecValidFace;
		CollectSSToCombine(vecSpacesToCombine,vecValidFace);
		if (vecSpacesToCombine.empty())//no SS can be combined
		{
			break;
		}
		//start combination
		//in each list<int>,the first element is the id after combination,
		//the following elements are the id of subspaces to delete(note that this is opposite with the order in vecSpacesToCombine)
		vector<list<int>> vecSSComb;
		for (unsigned int i=0;i<vecSpacesToCombine.size();i++)
		{
			int iLeftSSId=-1;//id of subspace to be left
			int iDelSSId=-1;//id of subspace to delete
			for (unsigned int j=0;j<vecSSComb.size();j++)
			{
				list<int>::iterator pFindFirst=find(vecSSComb.at(j).begin(),vecSSComb.at(j).end(),vecSpacesToCombine.at(i).first);
				if (pFindFirst!=vecSSComb.at(j).end())
				{
					if (pFindFirst==vecSSComb.at(j).begin())
					{
						vecSSComb.at(j).push_front(vecSpacesToCombine.at(i).second);
						iLeftSSId=vecSpacesToCombine.at(i).second;
						iDelSSId=vecSpacesToCombine.at(i).first;
					}
					else
					{
						DBWindowWrite("error pair in subspace combination\n");
					}
					break;
				}
				list<int>::iterator pFindSecond=find(vecSSComb.at(j).begin(),vecSSComb.at(j).end(),vecSpacesToCombine.at(i).second);
				if (pFindSecond!=vecSSComb.at(j).end())
				{
					if (pFindSecond==vecSSComb.at(j).begin())
					{
						vecSSComb.at(j).push_back(vecSpacesToCombine.at(i).first);
						iLeftSSId=vecSpacesToCombine.at(i).second;
						iDelSSId=vecSpacesToCombine.at(i).first;
					}
					else
					{
						vecSSComb.at(j).push_back(vecSpacesToCombine.at(i).first);
						iLeftSSId=vecSSComb.at(j).front();
						iDelSSId=vecSpacesToCombine.at(i).first;
					}
					break;
				}
			}
			if (iLeftSSId==-1 && iDelSSId==-1)
			{
				//a fresh new pair
				list<int> ListNewPair;
				ListNewPair.push_back(vecSpacesToCombine.at(i).second);
				ListNewPair.push_back(vecSpacesToCombine.at(i).first);
				vecSSComb.push_back(ListNewPair);
				iLeftSSId=vecSpacesToCombine.at(i).second;
				iDelSSId=vecSpacesToCombine.at(i).first;
			}
			CombineTwoSS(iLeftSSId,iDelSSId,vecValidFace.at(i));
		}
	}
	//compute mesh formed by each subspace
	GetSSMesh();
}

void KW_CS2Surf::CollectSSToCombine(vector<Int_Int_Pair>& vecSpacesToCombine,vector<int>& vecValidFace)
{
	for (int i=0;i<this->iSSspacenum;i++)
	{
		//iterate all faces,collecting full faces(with CSs on)
		int iFullFaceNum=0;
		int iValidFaceID=-1;
		for (int j=0;j<this->vecSSspacefacenum.at(i);j++)
		{
			if (iFullFaceNum>=2)
			{
				break;
			}
			int iFaceID=this->vecvecSSspace.at(i).at(j);
			PolygonOnFace currentPOF=this->vecPOF.at(iFaceID);
			if (!this->vecResortFace.at(iFaceID).bBoundaryFace)
			{
				if (!currentPOF.PwhList3D.empty())
				{
					iValidFaceID=iFaceID;
					iFullFaceNum++;
				}
			}
		}
		if (iFullFaceNum!=1)//more than one face has CSs,no need to combine
		{
			continue;
		}
		//combine this subspace with neighbors
		int iNbSpaceID=-1;
		int iAdjFaceID=-1;
		GetSSToCombine(i,iValidFaceID,iAdjFaceID,iNbSpaceID);
		if (iNbSpaceID!=-1 && iAdjFaceID!=-1)
		{
			//judge if the same pair has been recorded
			Int_Int_Pair currPair=make_pair(i,iNbSpaceID);
			Int_Int_Pair currPairOpp=make_pair(iNbSpaceID,i);
			vector<Int_Int_Pair>::iterator pFind=find(vecSpacesToCombine.begin(),vecSpacesToCombine.end(),currPair);
			vector<Int_Int_Pair>::iterator pFindOpp=find(vecSpacesToCombine.begin(),vecSpacesToCombine.end(),currPairOpp);
			if (pFind==vecSpacesToCombine.end() && pFindOpp==vecSpacesToCombine.end())
			{
				vecSpacesToCombine.push_back(make_pair(i,iNbSpaceID));
				vecValidFace.push_back(iValidFaceID);
			}
		}
	}
}

void KW_CS2Surf::GetSSToCombine(int iCurrSSID,int iValidFaceID,int& iAdjFaceID,int& iNbSpaceID)
{
	//get a ray starting from the center of the POF, with direction same with the normal of the face
	PolygonOnFace currentPOF=this->vecPOF.at(iValidFaceID);
	Point_3 POFCenter=CGAL::centroid(currentPOF.PwhList3D.front().outer_boundary.begin(),
		currentPOF.PwhList3D.front().outer_boundary.end());
	Vector_3 ValidFaceNorm;
	if (this->vecResortFace.at(iValidFaceID).vecSubspaceId.front()==iCurrSSID)
	{
		ValidFaceNorm=this->vecResortFace.at(iValidFaceID).vecHeightVect.front();
	}
	else
	{
		ValidFaceNorm=this->vecResortFace.at(iValidFaceID).vecHeightVect.back();
	}
	Ray_3 ValidRay(POFCenter,ValidFaceNorm);
	//intersect the ray with every facets of the subspace,select the face
	//on which the intersection point(if exist) is nearst to the center of POF
	double dMinDist=999999999999;
	for (int i=0;i<this->vecSSspacefacenum.at(iCurrSSID);i++)
	{
		int iOppFaceID=this->vecvecSSspace.at(iCurrSSID).at(i);
		if (iOppFaceID==iValidFaceID)//the face itself,skip
		{
			continue;
		}
		if (this->vecResortFace.at(iOppFaceID).bBoundaryFace)//if the face to judge is a boundary face,skip
		{
			continue;
		}
		vector<Point_3> vecOppFacePoint=this->vecResortFace.at(iOppFaceID).vecFaceVertex;
		int iSecondVerIndex=1;
		//split the facet into triangles,compute the intersection point(if exist)--same
		//method as used in GetSSMesh() function
		for (unsigned int j=0;j<vecOppFacePoint.size()-2;j++,iSecondVerIndex++)
		{
			Triangle_3 FaceTri(vecOppFacePoint.at(0),vecOppFacePoint.at(iSecondVerIndex),
				vecOppFacePoint.at(iSecondVerIndex+1));
			Point_3 PointOut;
			Segment_3 SegOut;
			int iResult=GeometryAlgorithm::CGAL_intersection(ValidRay,FaceTri,PointOut,SegOut);
			if (iResult==1)
			{
				double dDist=CGAL::squared_distance(POFCenter,PointOut);
				if (dDist<dMinDist)
				{
					dMinDist=dDist;
					iAdjFaceID=iOppFaceID;
				}
				break;
			}
		}
	}
	if (iAdjFaceID!=-1)
	{
		//get the neighbor subspace id
		if (this->vecResortFace.at(iAdjFaceID).vecSubspaceId.front()==iCurrSSID)//the subspace itself
		{
			iNbSpaceID=this->vecResortFace.at(iAdjFaceID).vecSubspaceId.back();
		}
		else
		{
			iNbSpaceID=this->vecResortFace.at(iAdjFaceID).vecSubspaceId.front();
		}
	}
}

void KW_CS2Surf::CombineTwoSS(int iLeftSSId,int iDelSSId,int iValidFaceId)
{
	//typedef struct _Resorted_Face
	//{
	//	//boundary face or not. if boundary face,don't record the following info
	//	bool bBoundaryFace;
	//	//id of plane it belongs to(==id of curve network)
	//	int iFacePlaneID;
	//	//vertex indices in all subspace vertices
	//	vector<int> vecFaceVertexInd;
	//	//vertex positions
	//	//if it is empty after resorting,it means this face has been deleted during subspace combination
	//	vector<Point_3> vecFaceVertex;
	//	//id of its two subspaces
	//	vector<int> vecSubspaceId;
	//	//orientations in its two subspaces
	//	//true:the normal(computed from the cross product of two edges) of the face points outward the subspace
	//	//which means the order of vertices in vecFaceVertexInd & vecFaceVertex is consistent with the mesh formed
	//	//from the subspace
	//	//false:the normal(computed from the cross product of two edges) of the face points inward the subspace
	//	vector<bool> vecOrient;
	//	//height vectors in its two subspace
	//	vector<Vector_3> vecHeightVect;
	//}*pResortedFace,ResortedFace;

	//get the adjacent faces between the two subspaces
	//these faces will be deleted permanently later
	vector<int> vecAdjFace;
	for (unsigned int i=0;i<this->vecResortFace.size();i++)
	{
		if (this->vecResortFace.at(i).bBoundaryFace)
		{
			continue;;
		}
		if (this->vecResortFace.at(i).vecFaceVertex.empty())
		{
			continue;
		}
		if ((this->vecResortFace.at(i).vecSubspaceId.front()==iLeftSSId && this->vecResortFace.at(i).vecSubspaceId.back()==iDelSSId)
			|| (this->vecResortFace.at(i).vecSubspaceId.front()==iDelSSId && this->vecResortFace.at(i).vecSubspaceId.back()==iLeftSSId))
		{
			vecAdjFace.push_back(i);
		}
	}

	//update the vecHeightVect first,then the vecSubspaceId
	//compute the new subspace center through averaging the centers of the two subspaces
	Point_3 LeftSSCenter,DelSSCenter;
	GetSSCenter(iLeftSSId,LeftSSCenter);
	GetSSCenter(iDelSSId,DelSSCenter);
	Point_3 NewSSCenter=CGAL::midpoint(LeftSSCenter,DelSSCenter);
	for (int i=0;i<this->vecSSspacefacenum.at(iDelSSId);i++)
	{
		int iFaceID=this->vecvecSSspace.at(iDelSSId).at(i);
		if (iFaceID==iValidFaceId)
		{
			if (this->vecResortFace.at(iFaceID).vecSubspaceId.front()==iDelSSId)
			{
				//update the height vector if the face has CSs
				Vector_3 currHeightVec=this->vecResortFace.at(iFaceID).vecHeightVect.front();
				currHeightVec=currHeightVec/sqrt(currHeightVec.squared_length());
				//plane of the face
				Plane_3 plane=this->vecTempCN.at(this->vecResortFace.at(iFaceID).iFacePlaneID).plane;
				//get the distance between the center point and the plane
				double dDist=sqrt(CGAL::squared_distance(NewSSCenter,plane));
				currHeightVec=currHeightVec*dDist;
				this->vecResortFace.at(iFaceID).vecHeightVect.front()=currHeightVec;
			}
			else if (this->vecResortFace.at(iFaceID).vecSubspaceId.back()==iDelSSId)
			{
				//update the height vector if the face has CSs
				Vector_3 currHeightVec=this->vecResortFace.at(iFaceID).vecHeightVect.back();
				currHeightVec=currHeightVec/sqrt(currHeightVec.squared_length());
				//plane of the face
				Plane_3 plane=this->vecTempCN.at(this->vecResortFace.at(iFaceID).iFacePlaneID).plane;
				//get the distance between the center point and the plane
				double dDist=sqrt(CGAL::squared_distance(NewSSCenter,plane));
				currHeightVec=currHeightVec*dDist;
				this->vecResortFace.at(iFaceID).vecHeightVect.back()=currHeightVec;
			}
			break;
		}
	}
	//update the vecSubspaceId in the subspace to delete
	for (int i=0;i<this->vecSSspacefacenum.at(iDelSSId);i++)
	{
		int iFaceID=this->vecvecSSspace.at(iDelSSId).at(i);
		if (this->vecResortFace.at(iFaceID).vecSubspaceId.front()==iDelSSId)
		{
			//update the neighbor subspace info of the face
			this->vecResortFace.at(iFaceID).vecSubspaceId.front()=iLeftSSId;
		}
		else if (this->vecResortFace.at(iFaceID).vecSubspaceId.back()==iDelSSId)
		{
			//update the neighbor subspace info of the face
			this->vecResortFace.at(iFaceID).vecSubspaceId.back()=iLeftSSId;
		}
	}
	//clear the information of the adjacent faces between the two subspace
	for (unsigned int i=0;i<vecAdjFace.size();i++)
	{
		this->vecResortFace.at(vecAdjFace.at(i)).vecFaceVertex.clear();
		this->vecResortFace.at(vecAdjFace.at(i)).vecFaceVertexInd.clear();
		this->vecResortFace.at(vecAdjFace.at(i)).vecHeightVect.clear();
		this->vecResortFace.at(vecAdjFace.at(i)).vecOrient.clear();
		this->vecResortFace.at(vecAdjFace.at(i)).vecSubspaceId.clear();
	}

	////kw: position of bounding points of all subspaces
	//vector<float> vecSSver;
	////kw: indices of two endpoints of all bounding edges
	//vector<int> vecSSedge;
	////kw: number of faces of all subspaces
	//int iSSfacenum;
	////kw: number of edges on each face
	//vector<int> vecSSfaceedgenum;
	////kw: indices of edges on each face
	//vector<vector<int>> vecvecSSface;
	////kw: index of the plane each face lies on
	//vector<int> vecSSface_planeindex;
	////kw: number of subspaces
	//int iSSspacenum;
	////kw: number of faces in each subspace
	//vector<int> vecSSspacefacenum;
	////kw: indices of faces in each subspace
	//vector<vector<int>> vecvecSSspace;

	//update the information in subspace stl
	//to maintain the indices of left subspaces/faces/...,do not delete the combined subspaces/faces/...
	//only update vecSSspacefacenum and vecvecSSspace info
	//erase the adjacent face from the left subspace
	for (unsigned int i=0;i<vecAdjFace.size();i++)
	{
		vector<int>::iterator pFind=find(this->vecvecSSspace.at(iLeftSSId).begin(),
			this->vecvecSSspace.at(iLeftSSId).end(),vecAdjFace.at(i));
		if (pFind!=this->vecvecSSspace.at(iLeftSSId).end())
		{
			this->vecvecSSspace.at(iLeftSSId).erase(pFind);
		}
	}
	//for (int i=0;i<this->vecSSspacefacenum.at(iLeftSSId);i++)
	//{
	//	int iFaceID=this->vecvecSSspace.at(iLeftSSId).at(i);
	//	vector<int>::iterator pFind=find(vecAdjFace.begin(),vecAdjFace.end(),iFaceID);
	//	if (pFind!=vecAdjFace.end())
	//	{
	//		this->vecvecSSspace.at(iLeftSSId).erase(this->vecvecSSspace.at(iLeftSSId).begin()+i);
	//	}
	//}
	//put all the faces(except the adjacent face) in the subspace to delete into the subspace to remain
	for (int i=0;i<this->vecSSspacefacenum.at(iDelSSId);i++)
	{
		int iFaceID=this->vecvecSSspace.at(iDelSSId).at(i);
		vector<int>::iterator pFind=find(vecAdjFace.begin(),vecAdjFace.end(),iFaceID);
		if (pFind==vecAdjFace.end())
		{
			this->vecvecSSspace.at(iLeftSSId).push_back(iFaceID);
		}
	}
	this->vecvecSSspace.at(iDelSSId).clear();
	//update the face number in the left and delete subspace
	this->vecSSspacefacenum.at(iDelSSId)=0;
	this->vecSSspacefacenum.at(iLeftSSId)=this->vecvecSSspace.at(iLeftSSId).size();
}

void KW_CS2Surf::GetSSMesh()
{
	//faces in each subspace,each vector<int> represents a face composed of vertices with indices <int>
	vector<vector<vector<int>>> vecTotalFaceVer;
	//faces in each subspace,each int represents the face id of the facet in a subspace (a face may contain many facets(eg,triangles))
	vector<vector<int>> vecTotalFace;
	//the order of each element in vecTotalFaceVer&vecTotalFace is consistent with the order of subspaces
	vecTotalFaceVer.resize(this->iSSspacenum);
	vecTotalFace.resize(this->iSSspacenum);
	for (unsigned int i=0;i<this->vecResortFace.size();i++)
	{
		ResortedFace currFace=vecResortFace.at(i);
		vector<int> currFaceVertexInd=currFace.vecFaceVertexInd;
		//the two subspace this face belongs to
		for (unsigned int j=0;j<currFace.vecOrient.size();j++)
		{
			int icurrSSId=currFace.vecSubspaceId.at(j);
			if (!currFace.vecOrient.at(j))
			{
				reverse(currFaceVertexInd.begin(),currFaceVertexInd.end());
			}
			vecTotalFaceVer.at(icurrSSId).push_back(currFaceVertexInd);
			vecTotalFace.at(icurrSSId).push_back(i);
			//reverse back to the originally saved order
			if (!currFace.vecOrient.at(j))
			{
				reverse(currFaceVertexInd.begin(),currFaceVertexInd.end());
			}
		}
	}

	//id of vertices stored in vecTotalFaceVer is the global vertex id,need to convert it 
	//to the id within each subspace mesh
	//all the vertices are collected from the faces, so isolated vertices caused by the 
	//subspace combination will not be taken account into consideration

	//subspace number after combination
	int iSSNumAfterComb=0;
	for (unsigned int i=0;i<vecTotalFaceVer.size();i++)
	{
		set<int> setSSVer;
		vector<vector<set<int>::iterator>> vecFaceSetIter;
		//put all vertex indices in, store the iterator
		for (unsigned int j=0;j<vecTotalFaceVer.at(i).size();j++)
		{
			vector<set<int>::iterator> currFaceSetIter;
			for (unsigned int k=0;k<vecTotalFaceVer.at(i).at(j).size();k++)
			{
				pair<set<int>::iterator, bool> pairResult=setSSVer.insert(vecTotalFaceVer.at(i).at(j).at(k));
				currFaceSetIter.push_back(pairResult.first);
			}
			vecFaceSetIter.push_back(currFaceSetIter);
		}
		//get the id within the subspace according to the iterator
		vector<vector<int>> vecLocalVerInd;
		for (unsigned int j=0;j<vecFaceSetIter.size();j++)
		{
			vector<int> LocalFace;
			for (unsigned int k=0;k<vecFaceSetIter.at(j).size();k++)
			{
				int iLocalId=distance(setSSVer.begin(),vecFaceSetIter.at(j).at(k));
				LocalFace.push_back(iLocalId);
			}
			vecLocalVerInd.push_back(LocalFace);
		}
		//convert to triangular mesh
		//a simple method (012345->012 023 034 045),may have bug 
		vector<vector<int>> vecTriLocalVerInd;
		vector<int> vecTriFaceId;//the subspace face id of each triangle
		for (unsigned int j=0;j<vecLocalVerInd.size();j++)
		{
			vector<int> vecCurrFace=vecLocalVerInd.at(j);
			int iSecondVerLocalId=1;
			int iFacetFaceId=vecTotalFace.at(i).at(j);
			for (unsigned int k=0;k<vecCurrFace.size()-2;k++)
			{
				vector<int> vecTriFace;
				vecTriFace.push_back(vecCurrFace.at(0));
				vecTriFace.push_back(vecCurrFace.at(iSecondVerLocalId));
				vecTriFace.push_back(vecCurrFace.at(iSecondVerLocalId+1));
				iSecondVerLocalId++;
				vecTriLocalVerInd.push_back(vecTriFace);
				vecTriFaceId.push_back(iFacetFaceId);
			}
		}
		//store all the points
		vector<Point_3> vecAllPoint;
		for (set<int>::iterator SetIter=setSSVer.begin();SetIter!=setSSVer.end();SetIter++)
		{
			vecAllPoint.push_back(Point_3(vecSSver.at(3*(*SetIter)),
				vecSSver.at(3*(*SetIter)+1),
				vecSSver.at(3*(*SetIter)+2)));
		}
		KW_Mesh currSSMesh;
		//if a subspace has been combined into others,still save it as an empty one,
		//in order to keep the order of subspaces
		if (!vecAllPoint.empty() && !vecTriLocalVerInd.empty())
		{
			//Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecAllPoint,vecLocalVerInd);
			Convert_Array_To_KW_Mesh<HalfedgeDS> triangle(vecAllPoint,vecTriLocalVerInd);
			currSSMesh.delegate(triangle);
			//record the face id of each triangle
			assert(currSSMesh.size_of_facets()==vecTriFaceId.size());
			Facet_iterator FaceIter=currSSMesh.facets_begin();
			for (unsigned int j=0;j<vecTriFaceId.size();j++)
			{
				FaceIter->SetReserved(vecTriFaceId.at(j));
				FaceIter++;
			}
		}
		//save the subspace mesh
		if (!currSSMesh.empty())
		{
			iSSNumAfterComb++;
			OBJHandle::UnitizeCGALPolyhedron(currSSMesh,false,false);
		}
		this->vecSSMesh.push_back(currSSMesh);
		////test
		//if (!currSSMesh.empty())
		//{
		//	OBJHandle::UnitizeCGALPolyhedron(currSSMesh,false,false);
		//	this->vecSinglePoly.push_back(currSSMesh);
		//}
	}
	DBWindowWrite("initial subzone no.: %d\n",this->iSSspacenum);
	DBWindowWrite("subzone no. after combination: %d\n",iSSNumAfterComb);
}