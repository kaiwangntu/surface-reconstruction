#include "StdAfx.h"
#include "KW_CS2Surf.h"

void KW_CS2Surf::PostProcMesh()
{
	//Get constratin edges and vertices
	vector<Halfedge_handle> vecConstrEdge;
	//two intersected CSs will contain a set of same vertices,so use set<> here instead of vector<>
	set<Vertex_handle> setConstrVertex;
	bool bConstrResult=GetConstraintEdges(vecConstrEdge,setConstrVertex);
	DBWindowWrite("constraint edges num: %d\n",vecConstrEdge.size());
	DBWindowWrite("constraint vertices num: %d\n",setConstrVertex.size());
	
	if (bConstrResult)
	{
		//RefineSmooth(0,1.414,10,0.5,vecConstrEdge,setConstrVertex);
		SaveToMesh(vecConstrEdge);
	}
}

bool KW_CS2Surf::GetConstraintEdges(vector<Halfedge_handle>& vecConstrEdge,set<Vertex_handle>& setConstrVertex)
{
	vector<Point_3> vecAllPoint;
	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	{
		vecAllPoint.push_back(VerIter->point());
	}

	for (unsigned int i=0;i<this->vecTempCN.size();i++)
	{
		CurveNetwork currentCN=this->vecTempCN.at(i);
		for (unsigned int j=0;j<currentCN.Profile3D.size();j++)
		{
			vector<Point_3> currentCS=currentCN.Profile3D.at(j);
			//find the first point of currentCS in Polyhedron
			double dMinDist=9999999;
			Vertex_iterator hFirstVer;
			for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
			{
				double dDist=CGAL::squared_distance(VerIter->point(),currentCS.front());
				if (dDist<dMinDist)
				{
					dMinDist=dDist;
					hFirstVer=VerIter;
				}
			}
			if (dMinDist>5)//0.0001
			{
				DBWindowWrite("CN %d CS %d,cannot find first point on CS, min dist: %f\n",i,j,dMinDist);
				break;
			}
			//save first constraint vertex
			setConstrVertex.insert(hFirstVer);
			vector<Halfedge_handle> vecCurrentHh;
			//get the edges on current CS
			for (unsigned int k=0;k<currentCS.size();k++)
			{
				Point_3 NextPoint=currentCS.at((k+1)%currentCS.size());
				Halfedge_around_vertex_circulator Havc=hFirstVer->vertex_begin();
				double dMinDist=9999999;
				
				//test
				vector<Point_3> vecNbTestPnt;
				vecNbTestPnt.push_back(hFirstVer->point());


				Halfedge_handle HhPointToNextVer;
				do 
				{
					Halfedge_handle HhTemp=Havc->opposite();
					double dDist=CGAL::squared_distance(HhTemp->vertex()->point(),NextPoint);
					
					//test
					vecNbTestPnt.push_back(HhTemp->vertex()->point());

					//if (HhPointToNextVer->vertex()->point()==NextPoint)
					if (dDist<dMinDist)
					{
						HhPointToNextVer=HhTemp;
						dMinDist=dDist;
					}
					Havc++;
				} while(Havc!=hFirstVer->vertex_begin());

				if (dMinDist<5)
				{
					vecCurrentHh.push_back(HhPointToNextVer);
					hFirstVer=HhPointToNextVer->vertex();
					setConstrVertex.insert(hFirstVer);
				}
				else
				{
					//this->vecTestPoint.insert(this->vecTestPoint.end(),vecNbTestPnt.begin(),vecNbTestPnt.end());

					//record the facets around this vertex
					//do 
					//{
					//	Halfedge_handle HhPointToNextVer=Havc->opposite();
					//	Facet_handle Fhtest=Havc->facet();
					//	Halfedge_around_facet_circulator Hafctest=Fhtest->facet_begin();
					//	vector<Point_3> TriPointTest;
					//	do 
					//	{
					//		TriPointTest.push_back(Hafctest->vertex()->point());
					//		Hafctest++;
					//	} while(Hafctest!=Fhtest->facet_begin());
					//	Triangle_3 tritest(TriPointTest.at(0),TriPointTest.at(1),TriPointTest.at(2));
					//	this->vecTestTri.push_back(tritest);
					//	//this->vecTestSeg.push_back(Segment_3(Havc->vertex()->point(),HhPointToNextVer->vertex()->point()));
					//	Havc++;
					//} while(Havc!=hFirstVer->vertex_begin());

					//for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
					//{
					//	double dDist=CGAL::squared_distance(VerIter->point(),NextPoint);
					//	if (dDist<1)
					//	{
					//		//record the facets around this vertex
					//		Halfedge_around_vertex_circulator Havc=VerIter->vertex_begin();
					//		do 
					//		{
					//			Facet_handle Fhtest=Havc->facet();
					//			Halfedge_around_facet_circulator Hafctest=Fhtest->facet_begin();
					//			vector<Point_3> TriPointTest;
					//			do 
					//			{
					//				TriPointTest.push_back(Hafctest->vertex()->point());
					//				Hafctest++;
					//			} while(Hafctest!=Fhtest->facet_begin());
					//			Triangle_3 tritest(TriPointTest.at(0),TriPointTest.at(1),TriPointTest.at(2));
					//			this->vecTestTri.push_back(tritest);
					//			//this->vecTestSeg.push_back(Segment_3(Havc->vertex()->point(),HhPointToNextVer->vertex()->point()));
					//			Havc++;
					//		} while(Havc!=VerIter->vertex_begin());
					//		this->vecTestPoint.push_back(NextPoint);
					//		break;
					//	}
					//}
					DBWindowWrite("CN %d CS %d Point %d,distance between two points>1: %f\n",i,j,k,dMinDist);
					break;
				}
			}
			assert(vecCurrentHh.size()==currentCS.size());
			//for (unsigned int itest=0;itest<vecCurrentHh.size();itest++)
			//{
			//	this->vecTestPoint.push_back(vecCurrentHh.at(itest)->vertex()->point());
			//}
			vecConstrEdge.insert(vecConstrEdge.end(),vecCurrentHh.begin(),vecCurrentHh.end());
		}
	}
	//test
	//return false;
	//for (unsigned int i=0;i<vecConstrEdge.size();i++)
	//{
	//	this->vecTestPoint.push_back(vecConstrEdge.at(i)->vertex()->point());
	//}
	return true;
}

//bool KW_CS2Surf::GetConstraintEdges(vector<Halfedge_handle>& vecConstrEdge)
//{
//	vector<Point_3> vecAllPoint;
//	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
//	{
//		vecAllPoint.push_back(VerIter->point());
//	}
//
//	for (unsigned int i=0;i<this->vecTempCN.size();i++)
//	{
//		CurveNetwork currentCN=this->vecTempCN.at(i);
//		for (unsigned int j=0;j<currentCN.Profile3D.size();j++)
//		{
//			vector<Point_3> currentCS=currentCN.Profile3D.at(j);
//			//find the first point of currentCS in Polyhedron
//			vector<Point_3>::iterator PointIter=find(vecAllPoint.begin(),vecAllPoint.end(),currentCS.front());
//			if (PointIter==vecAllPoint.end())
//			{
//				DBWindowWrite("cannot find first point on CS\n");
//				break;
//			}
//			int iDist=distance(vecAllPoint.begin(),PointIter);
//			Vertex_iterator hFirstVer=this->InitPolyh.vertices_begin();
//			for (int k=0;k<iDist;k++)
//			{
//				hFirstVer++;
//			}
//			vector<Halfedge_handle> vecCurrentHh;
//			//get the edges on current CS
//			for (unsigned int k=0;k<currentCS.size();k++)
//			{
//				Point_3 NextPoint=currentCS.at((k+1)%currentCS.size());
//				Halfedge_around_vertex_circulator Havc=hFirstVer->vertex_begin();
//				do 
//				{
//					Halfedge_handle HhPointToNextVer=Havc->opposite();
//					double dDist=CGAL::squared_distance(HhPointToNextVer->vertex()->point(),NextPoint);
//					//if (HhPointToNextVer->vertex()->point()==NextPoint)
//					if (dDist<1)
//					{
//						vecCurrentHh.push_back(HhPointToNextVer);
//						hFirstVer=HhPointToNextVer->vertex();
//						break;
//					}
//					Havc++;
//				} while(Havc!=hFirstVer->vertex_begin());
//			}
//			assert(vecCurrentHh.size()==currentCS.size());
//			vecConstrEdge.insert(vecConstrEdge.end(),vecCurrentHh.begin(),vecCurrentHh.end());
//		}
//	}
//	//for (unsigned int i=0;i<vecConstrEdge.size();i++)
//	//{
//	//	this->vecTestPoint.push_back(vecConstrEdge.at(i)->vertex()->point());
//	//}
//	return true;
//}

void KW_CS2Surf::SaveToMesh(vector<Halfedge_handle> vecConstrEdge)
{
	//set the mesh
	//clear
	if(this->mesh!= NULL )
		delete this->mesh;

	//save vertex positions
	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	{
		this->mver.push_back(VerIter->point().x());
		this->mver.push_back(VerIter->point().y());
		this->mver.push_back(VerIter->point().z());
	}
	//save faces
	for (Facet_iterator FaceIter=this->InitPolyh.facets_begin();FaceIter!=this->InitPolyh.facets_end();FaceIter++)
	{
		vector<int> currentFace;
		Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
		do 
		{
			int iIndex=distance(this->InitPolyh.vertices_begin(),Hafc->vertex());
			currentFace.push_back(iIndex);
			Hafc++;
		} while(Hafc!=FaceIter->facet_begin());
		currentFace.push_back(2);
		currentFace.push_back(1);
		this->mface.insert(this->mface.end(),currentFace.begin(),currentFace.end());
	}
	//save constraint edges
	for (unsigned int i=0;i<vecConstrEdge.size();i++)
	{
		Halfedge_handle currentHh=vecConstrEdge.at(i);
		int iStartId=distance(this->InitPolyh.vertices_begin(),currentHh->opposite()->vertex());
		int iEndId=distance(this->InitPolyh.vertices_begin(),currentHh->vertex());
		this->ctrmedge.push_back(iStartId);
		this->ctrmedge.push_back(iEndId);
	}

	//set
	this->mesh = new Mesh(this->mver,this->mface,this->ctrmedge,this->center,this->unitlen,PROCSIZE);
	//clear
	this->mver.clear();
	this->mface.clear();
	this->ctrmedge.clear();
}

void KW_CS2Surf::RefineSmooth(float RefiAlpha0, float RefiAlphaN, int times, float SmoRatio,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex)
{
	float delta = (RefiAlphaN - RefiAlpha0)/times;
	float alpha = RefiAlpha0;

	for( int i = 0; i < times; i ++)
	{
		DBWindowWrite("refin&smooth iteration:%d\n",i);
		//FiberMeshSmooth(SmoRatio, times);
		ConstrLaplacianSmooth(0.8,40,setConstrVertex);
		LiepaRefine(alpha,vecConstrEdge,setConstrVertex);//alpha
		//ConstrLaplacianSmooth(0.8,40,setConstrVertex);
		alpha += delta;
	}
}

void KW_CS2Surf::LiepaRefine(float alpha,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex)
{
	//compute the average edge length of each vertex,this value don't update during the refine process
	PreComputeAveEdgeLen(vecConstrEdge,setConstrVertex);
	//edge swap
	SwapEdge(vecConstrEdge,setConstrVertex);
	//triangle split and edge swap
	int iCount=0;
	while (iCount<MESH_REFINE_MAX_TIME)
	{
		if (SplitTriangle(alpha,vecConstrEdge,setConstrVertex))
		{
			SwapEdge(vecConstrEdge,setConstrVertex);
		}
		else
		{
			break;
		}
		iCount++;
	}
}

void KW_CS2Surf::PreComputeAveEdgeLen(vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex)
{
	//set mark and average edge length(0) for all vertices first
	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	{
		VerIter->SetReserved(0);
		VerIter->SetGaussianCurvature(0);
	}
	//compute the average edge length for each constraint vertex,only from constraint edges
	for (unsigned int i=0;i<vecConstrEdge.size();i++)
	{
		Vertex_handle Ver0=vecConstrEdge.at(i)->vertex();
		Vertex_handle Ver1=vecConstrEdge.at(i)->opposite()->vertex();
		double dEdgeLen=sqrt(CGAL::squared_distance(Ver0->point(),Ver1->point()));
		Ver0->SetGaussianCurvature(Ver0->GetGaussianCurvature()+dEdgeLen);
		Ver0->SetReserved(Ver0->GetReserved()+1);
		Ver1->SetGaussianCurvature(Ver1->GetGaussianCurvature()+dEdgeLen);
		Ver1->SetReserved(Ver1->GetReserved()+1);
	}
	for (set<Vertex_handle>::iterator SetIter=setConstrVertex.begin();SetIter!=setConstrVertex.end();SetIter++)
	{
		(*SetIter)->SetGaussianCurvature((*SetIter)->GetGaussianCurvature()/(double)(*SetIter)->GetReserved());
		(*SetIter)->SetReserved(0);
	}
	
	//iterate ring by ring inward to compute the estimated average length for all other vertices
	bool bAgain=false;
	do 
	{
		//set unchecked mark for each edge
		for (Halfedge_iterator HIter=this->InitPolyh.halfedges_begin();HIter!=this->InitPolyh.halfedges_end();HIter++)
		{
			HIter->SetReserved(0);
		}

		bAgain=false;
		for (Halfedge_iterator HIter=this->InitPolyh.halfedges_begin();HIter!=this->InitPolyh.halfedges_end();HIter++)
		{
			if (HIter->GetReserved()!=0)
			{
				continue;
			}
			//set checked mark for each edge
			HIter->SetReserved(2);
			HIter->opposite()->SetReserved(2);

			Vertex_handle Ver0=HIter->vertex();
			Vertex_handle Ver1=HIter->opposite()->vertex();
			if((Ver0->GetReserved()==0 && Ver0->GetGaussianCurvature()!=0) && (Ver1->GetReserved()!=0 || Ver1->GetGaussianCurvature()==0))//Ver0:outter Ver1:inner
			{
				double dOldInEdgeLen=Ver1->GetGaussianCurvature();
				double dOutEdgeLen=Ver0->GetGaussianCurvature();
				Ver1->SetGaussianCurvature(dOldInEdgeLen+dOutEdgeLen);
				int iEdgeNum=Ver1->GetReserved()+1;
				Ver1->SetReserved(iEdgeNum);
			}
			else if ((Ver1->GetReserved()==0 && Ver1->GetGaussianCurvature()!=0) && (Ver0->GetReserved()!=0 || Ver0->GetGaussianCurvature()==0))//Ver1:outter Ver0:inner
			{
				double dOldInEdgeLen=Ver0->GetGaussianCurvature();
				double dOutEdgeLen=Ver1->GetGaussianCurvature();
				Ver0->SetGaussianCurvature(dOldInEdgeLen+dOutEdgeLen);
				int iEdgeNum=Ver0->GetReserved()+1;
				Ver0->SetReserved(iEdgeNum);
			}
		}

		for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
		{
			if (VerIter->GetReserved()!=0)
			{
				bAgain=true;
				double dAveEdgeLen=VerIter->GetGaussianCurvature()/(double)VerIter->GetReserved();
				VerIter->SetGaussianCurvature(dAveEdgeLen);
				VerIter->SetReserved(0);
			}
		}
	} while(bAgain);
}

void KW_CS2Surf::SwapEdge(vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex)
{
	//mark all edges
	for (Halfedge_iterator HIter=this->InitPolyh.halfedges_begin();HIter!=this->InitPolyh.halfedges_end();HIter++)
	{
		HIter->SetReserved(0);
	}
	//mark constraint edges
	for (unsigned int i=0;i<vecConstrEdge.size();i++)
	{
		vecConstrEdge.at(i)->SetReserved(1);
		vecConstrEdge.at(i)->opposite()->SetReserved(1);
	}
	//collect all plain edges (excl. constraint&border edges)
	vector<Halfedge_handle> vecPlainEdge;
	for (Halfedge_iterator HIter=this->InitPolyh.halfedges_begin();HIter!=this->InitPolyh.halfedges_end();HIter++)
	{
		if (HIter->is_border())
		{
			continue;
		}
		if (HIter->GetReserved()==0)
		{
			vecPlainEdge.push_back(HIter);
			HIter->SetReserved(2);
			HIter->opposite()->SetReserved(2);
		}
	}
	//DBWindowWrite("num of plain edges: %d\n",vecPlainEdge.size());

	bool bAgain=true;
	int iCount=0;
	//continue the refine until no edges need to be swapped
	while(bAgain && iCount<MESH_REFINE_MAX_TIME)
	{
		bAgain=false;
		iCount++;

		//test
		vector<int> vecFlippedInd;
		for (unsigned int i=0;i<vecPlainEdge.size();i++)
		{
			Halfedge_handle HhCurrent=vecPlainEdge.at(i);
			//get the two angles opposite to this edge
			Vector_3 Vect00=HhCurrent->next()->vertex()->point()-HhCurrent->vertex()->point();
			Vector_3 Vect01=HhCurrent->next()->vertex()->point()-HhCurrent->opposite()->vertex()->point();
			double dRadian0=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(Vect00,Vect01,true);
			Vector_3 Vect10=HhCurrent->opposite()->next()->vertex()->point()-HhCurrent->opposite()->vertex()->point();
			Vector_3 Vect11=HhCurrent->opposite()->next()->vertex()->point()-HhCurrent->vertex()->point();
			double dRadian1=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(Vect10,Vect11,true);
			double dResult=cos(dRadian0)*sin(dRadian1)+cos(dRadian1)*sin(dRadian0);
			if (dResult<-0.000001)//>180
			{
				//check if the swapped edge has existed in the mesh,if yes,can't swap
				bool bEdgeExist=false;
				Vertex_handle VhToCheck=HhCurrent->next()->vertex();
				Halfedge_around_vertex_circulator Havc=VhToCheck->vertex_begin();
				do 
				{
					if (Havc->opposite()->vertex()==HhCurrent->opposite()->next()->vertex())
					{
						bEdgeExist=true;
						break;
					}
					Havc++;
				} while(Havc!=VhToCheck->vertex_begin());
				if (bEdgeExist)
				{
					continue;
				}

				bAgain=true;
				this->InitPolyh.flip_edge(HhCurrent);
				vecFlippedInd.push_back(i);
			}
		}
		//DBWindowWrite("%d edges are swapped\n",vecFlippedInd.size());
	}
}

bool KW_CS2Surf::SplitTriangle(float alpha,vector<Halfedge_handle> vecConstrEdge,set<Vertex_handle> setConstrVertex)
{
	//iterate each triangle,judge to split or not
	bool bSplitted=false;
	vector<Facet_handle> vecFaceToSplit;
	vector<Point_3> vecNewCentroid;
	vector<double> vecNewAveEdgeLen;
	for (Facet_iterator FaIter=this->InitPolyh.facets_begin();FaIter!=this->InitPolyh.facets_end();FaIter++)
	{
		vector<Vertex_handle> vecTriVer;
		vector<Point_3> vecTriVerPos;
		Halfedge_around_facet_circulator Hafc=FaIter->facet_begin();
		do 
		{
			vecTriVer.push_back(Hafc->vertex());
			vecTriVerPos.push_back(Hafc->vertex()->point());
			Hafc++;
		} while(Hafc!=FaIter->facet_begin());
		Point_3 NewCentroid=CGAL::centroid(vecTriVerPos.begin(),vecTriVerPos.end());
		//average edge length computed at the centroid
		double dCentAveLen=(vecTriVer.at(0)->GetGaussianCurvature()+vecTriVer.at(1)->GetGaussianCurvature()+vecTriVer.at(2)->GetGaussianCurvature())/3;
		bool bTestSplit=true;
		for (unsigned int i=0;i<vecTriVerPos.size();i++)
		{
			double dDist=sqrt(CGAL::squared_distance(vecTriVerPos.at(i),NewCentroid));
			double dVerEdgeLen=vecTriVer.at(i)->GetGaussianCurvature();
			//if (dDist*alpha>dVerEdgeLen && dDist*alpha>dCentAveLen)
			//{
			//	vecFaceToSplit.push_back(FaIter);
			//	vecNewCentroid.push_back(NewCentroid);
			//	vecNewAveEdgeLen.push_back(dCentAveLen);
			//	bSplitted=true;
			//	break;
			//}
			//if (dDist*alpha<dVerEdgeLen)
			if (dDist*alpha<dVerEdgeLen || dDist*alpha<dCentAveLen)
			{
				bTestSplit=false;
				break;
			}
		}
		if (bTestSplit)
		{
			vecFaceToSplit.push_back(FaIter);
			vecNewCentroid.push_back(NewCentroid);
			vecNewAveEdgeLen.push_back(dCentAveLen);
			bSplitted=true;
		}
	}
	//split
	for (unsigned int i=0;i<vecFaceToSplit.size();i++)
	{
		Halfedge_handle HhToCenter=this->InitPolyh.create_center_vertex(vecFaceToSplit.at(i)->halfedge());
		HhToCenter->vertex()->point()=vecNewCentroid.at(i);
		//don't forget to save the average edge length for the new vertex!
		HhToCenter->vertex()->SetGaussianCurvature(vecNewAveEdgeLen.at(i));
	}
	DBWindowWrite("%d triangles splitted\n",vecFaceToSplit.size());

	return bSplitted;
}

void KW_CS2Surf::ConstrLaplacianSmooth(float fRatio, int iStimes,set<Vertex_handle> setConstrVertex)
{
	//mark constraint vertices
	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	{
		VerIter->SetReserved(0);
	}
	for (set<Vertex_handle>::iterator SetIter=setConstrVertex.begin();SetIter!=setConstrVertex.end();SetIter++)
	{
		(*SetIter)->SetReserved(1);
	}
	//smooth
	for (int i=0;i<iStimes;i++)
	{
		//compute laplacian
		for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
		{
			double dLaplacian[3];
			dLaplacian[0]=dLaplacian[1]=dLaplacian[2]=0;
			Halfedge_around_vertex_circulator Havc=VerIter->vertex_begin();
			do 
			{
				dLaplacian[0]=dLaplacian[0]+Havc->opposite()->vertex()->point().x();
				dLaplacian[1]=dLaplacian[1]+Havc->opposite()->vertex()->point().y();
				dLaplacian[2]=dLaplacian[2]+Havc->opposite()->vertex()->point().z();
				Havc++;
			} while(Havc!=VerIter->vertex_begin());

			for (unsigned int k=0;k<3;k++)
			{
				dLaplacian[k]=dLaplacian[k]/(double)VerIter->vertex_degree();
			}

			Point_3 CentroidPoint(dLaplacian[0],dLaplacian[1],dLaplacian[2]);
			Vector_3 CurrenVector=CentroidPoint-VerIter->point();
			VerIter->SetUniformLaplacian(CurrenVector);
		}
		//move unconstrained vertices
		for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
		{
			if (VerIter->GetReserved()==1)
			{
				continue;
			}
			VerIter->point()=VerIter->point()+fRatio*VerIter->GetUniformLaplacian();
		}
	}
}

void KW_CS2Surf::TestSmooth(float fRatio, int iStimes,set<Vertex_handle> setConstrVertex)
{
	//mark constraint vertices
	for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
	{
		VerIter->SetReserved(0);
	}
	for (set<Vertex_handle>::iterator SetIter=setConstrVertex.begin();SetIter!=setConstrVertex.end();SetIter++)
	{
		(*SetIter)->SetReserved(1);
	}
	//smooth
	for (int i=0;i<iStimes;i++)
	{
		//compute laplacian
		for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
		{
			double dLaplacian[3];
			dLaplacian[0]=dLaplacian[1]=dLaplacian[2]=0;
			Halfedge_around_vertex_circulator Havc=VerIter->vertex_begin();
			do 
			{
				dLaplacian[0]=dLaplacian[0]+Havc->opposite()->vertex()->point().x();
				dLaplacian[1]=dLaplacian[1]+Havc->opposite()->vertex()->point().y();
				dLaplacian[2]=dLaplacian[2]+Havc->opposite()->vertex()->point().z();
				Havc++;
			} while(Havc!=VerIter->vertex_begin());

			for (unsigned int k=0;k<3;k++)
			{
				dLaplacian[k]=dLaplacian[k]/(double)VerIter->vertex_degree();
			}

			Point_3 CentroidPoint(dLaplacian[0],dLaplacian[1],dLaplacian[2]);
			Vector_3 CurrenVector=CentroidPoint-VerIter->point();
			VerIter->SetUniformLaplacian(CurrenVector);
		}
		//move unconstrained vertices
		for (Vertex_iterator VerIter=this->InitPolyh.vertices_begin();VerIter!=this->InitPolyh.vertices_end();VerIter++)
		{
			if (VerIter->GetReserved()==1)
			{
				continue;
			}
			VerIter->point()=VerIter->point()-fRatio*VerIter->GetUniformLaplacian();
		}
	}
}

void KW_CS2Surf::FiberMeshSmooth(float SmoRatio,int times)
{

}

