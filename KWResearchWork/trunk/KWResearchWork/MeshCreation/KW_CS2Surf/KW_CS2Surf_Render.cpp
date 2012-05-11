#include "StdAfx.h"
#include "KW_CS2Surf.h"

void KW_CS2Surf::Render()
{
	RenderSubspaceStl();
//	RenderPolyOnFace();
	RenderTestPoints();
	RenderTestSeg();
	RenderSinglePolyh();
	RenderUnionPolyh();
	RenderTestTriangle();
}

void KW_CS2Surf::RenderSubspaceStl()
{
	if (this->iRenderSingleSS==CR_RENDER_NONE_SS)
	{
		return;
	}
	glLineWidth(2);
	//draw the frame of the plane
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);	
	//glColor3f(0,0,1);
	glColor3f(1,0,0);
	//for each subspace
	for (int i=0;i<this->iSSspacenum;i++)
	{
		if (this->iRenderSingleSS!=CR_RENDER_ALL_SS && i!=this->iRenderSingleSS)
		{
			continue;
		}
		//for each face in the subspace
		for (int j=0;j<this->vecSSspacefacenum.at(i);j++)
		{
			int iFaceID=this->vecvecSSspace.at(i).at(j);
			ResortedFace FaceInfo=this->vecResortFace.at(iFaceID);
			if (FaceInfo.bBoundaryFace)
			{
				continue;
			}
			//////test,for making the partition example only
			//if (iFaceID==10 || iFaceID==35 || iFaceID==45 || iFaceID==67 || iFaceID==59)
			//{
			//	glColor3f(0,0,1);
			//}
			//else
			//{
			//	glColor3f(1,0,0);
			//}
			//glBegin(GL_QUADS);
			glBegin(GL_POLYGON);
			for (unsigned int k=0;k<FaceInfo.vecFaceVertex.size();k++)
			{
				glVertex3d(FaceInfo.vecFaceVertex.at(k).x()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).y()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).z()*DIM/PROCSIZE);
			}
			glEnd();
		}
	}
	//draw the transparent face of the plane(2 faces)
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glEnable(GL_LIGHTING);
	glLineWidth(1);

	glDepthMask(FALSE);
	//	glDisable(GL_DEPTH_TEST);

	const GLfloat red_color[] = {1.0f, 0.0f, 0.0f, 0.2f};
	const GLfloat mat_red_emission[] = {1.0f, 0.0f, 0.0f, 1.0f};
	const GLfloat green_color[] = {0.0f, 1.0f, 0.0f, 0.2f};
	const GLfloat mat_green_emission[] = {0.0f, 1.0f, 0.0f, 1.0f};
	const GLfloat blue_color[] = {0.0f, 0.0f, 0.6f, 0.2f};
	const GLfloat mat_blue_emission[] = {0.0f, 0.0f, 1.0f, 1.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat_red_emission);

	//for each subspace
	for (int i=0;i<this->iSSspacenum;i++)
	{
		if (this->iRenderSingleSS!=CR_RENDER_ALL_SS && i!=this->iRenderSingleSS)
		{
			continue;
		}
		//for each face in the subspace
		for (int j=0;j<this->vecSSspacefacenum.at(i);j++)
		{
			int iFaceID=this->vecvecSSspace.at(i).at(j);
			ResortedFace FaceInfo=this->vecResortFace.at(iFaceID);
			if (FaceInfo.bBoundaryFace)
			{
				continue;
			}

			////test,for making the partition example only
			//if (iFaceID==10 || iFaceID==35 || iFaceID==45 || iFaceID==67 || iFaceID==59)
			//{
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue_color);
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat_blue_emission);
			//}
			//else
			//{
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red_color);
			//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat_red_emission);
			//}

			//glBegin(GL_QUADS);
			glBegin(GL_POLYGON);
			glNormal3f(FaceInfo.vecHeightVect.front().x(),FaceInfo.vecHeightVect.front().y(),FaceInfo.vecHeightVect.front().z());
			for (unsigned int k=0;k<FaceInfo.vecFaceVertex.size();k++)
			{
				glVertex3d(FaceInfo.vecFaceVertex.at(k).x()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).y()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).z()*DIM/PROCSIZE);
			}
			glNormal3f(FaceInfo.vecHeightVect.back().x(),FaceInfo.vecHeightVect.back().y(),FaceInfo.vecHeightVect.back().z());
			for (unsigned int k=0;k<FaceInfo.vecFaceVertex.size();k++)
			{
				glVertex3d(FaceInfo.vecFaceVertex.at(k).x()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).y()*DIM/PROCSIZE,
					FaceInfo.vecFaceVertex.at(k).z()*DIM/PROCSIZE);
			}
			glEnd();
		}
	}
	//	glEnable(GL_DEPTH_TEST);
	glDepthMask(TRUE);

	//glColor3f(1,0,0);
	//glDisable(GL_LIGHTING);
	//glLineWidth(2);
	////for each subspace
	//for (int i=0;i<this->iSSspacenum;i++)
	//{
	//	//for each face in the subspace
	//	for (int j=0;j<this->vecSSspacefacenum.at(i);j++)
	//	{
	//		int iFaceID=this->vecvecSSspace.at(i).at(j);
	//		//for each edge of the face
	//		for (int k=0;k<this->vecSSfaceedgenum.at(iFaceID);k++)
	//		{
	//			int iEdgeID=this->vecvecSSface.at(iFaceID).at(k);
	//			int iVerBeginID=this->vecSSedge.at(2*iEdgeID);
	//			int iVerEndID=this->vecSSedge.at(2*iEdgeID+1);
	//			Point_3 VerBegin=Point_3(this->vecSSver.at(3*iVerBeginID),this->vecSSver.at(3*iVerBeginID+1),this->vecSSver.at(3*iVerBeginID+2));
	//			Point_3 VerEnd=Point_3(this->vecSSver.at(3*iVerEndID),this->vecSSver.at(3*iVerEndID+1),this->vecSSver.at(3*iVerEndID+2));
	//			glBegin(GL_LINES);
	//			glVertex3f(VerBegin.x()*DIM/PROCSIZE+this->OldCenter.x(),VerBegin.y()*DIM/PROCSIZE+this->OldCenter.y(),VerBegin.z()*DIM/PROCSIZE+this->OldCenter.z());
	//			glVertex3f(VerEnd.x()*DIM/PROCSIZE+this->OldCenter.x(),VerEnd.y()*DIM/PROCSIZE+this->OldCenter.y(),VerEnd.z()*DIM/PROCSIZE+this->OldCenter.z());
	//			glEnd();
	//		}

	//	}
	//}
	//glLineWidth(1);
	//glEnable(GL_LIGHTING);
}

void KW_CS2Surf::RenderPolyOnFace()
{
	//if (this->vecPOF.empty())
	//{
	//	return;
	//}

	//glDisable(GL_LIGHTING);
	//for (unsigned int i=0;i<this->vecPOF.size();i++)
	//{
	//	PolygonOnFace CurrentPOF=this->vecPOF.at(i);
	//	if (CurrentPOF.vecPwhList3D.empty())
	//	{
	//		continue;
	//	}
	//	for (unsigned int j=0;j<CurrentPOF.vecPwhList3D.size();j++)
	//	{
	//		Pwh_list_3 CurrentPwhList=CurrentPOF.vecPwhList3D.at(j);
	//		Pwh_list_3::iterator PwhListIter;
	//		for (PwhListIter=CurrentPwhList.begin();PwhListIter!=CurrentPwhList.end();PwhListIter++)
	//		{
	//			Polygon_with_holes_3 CurrentPwh3=*PwhListIter;
	//			int iAssistOutEdgeId=0;
	//			for (unsigned int k=0;k<CurrentPwh3.outer_boundary.size();k++)
	//			{
	//				if (!CurrentPwh3.AssistOuterEdge.empty() && k==CurrentPwh3.AssistOuterEdge.at(iAssistOutEdgeId))
	//				{
	//					glColor3f(1,0,0);
	//					if (iAssistOutEdgeId!=CurrentPwh3.AssistOuterEdge.size()-1)
	//					{
	//						iAssistOutEdgeId++;
	//					}
	//				}
	//				else
	//				{
	//					glColor3f(0,0,1);
	//				}
	//				glLineWidth(8);
	//				glBegin(GL_LINES);
	//				int iStartId=k;
	//				int iEndId=(k+1)%CurrentPwh3.outer_boundary.size();
	//				glVertex3f(CurrentPwh3.outer_boundary.at(iStartId).x()*DIM/PROCSIZE,
	//					CurrentPwh3.outer_boundary.at(iStartId).y()*DIM/PROCSIZE,
	//					CurrentPwh3.outer_boundary.at(iStartId).z()*DIM/PROCSIZE);
	//				glVertex3f(CurrentPwh3.outer_boundary.at(iEndId).x()*DIM/PROCSIZE,
	//					CurrentPwh3.outer_boundary.at(iEndId).y()*DIM/PROCSIZE,
	//					CurrentPwh3.outer_boundary.at(iEndId).z()*DIM/PROCSIZE);
	//				glEnd();
	//				glLineWidth(1);
	//			}
	//			for (unsigned int m=0;m<CurrentPwh3.inner_hole.size();m++)
	//			{
	//				vector<Point_3> CurrentHole=CurrentPwh3.inner_hole.at(m);
	//				for (unsigned int n=0;n<CurrentHole.size();n++)
	//				{
	//					glColor3f(0,1,0);
	//					glLineWidth(8);
	//					glBegin(GL_LINES);
	//					int iStartId=n;
	//					int iEndId=(n+1)%CurrentHole.size();
	//					glVertex3f(CurrentHole.at(iStartId).x()*DIM/PROCSIZE,
	//						CurrentHole.at(iStartId).y()*DIM/PROCSIZE,
	//						CurrentHole.at(iStartId).z()*DIM/PROCSIZE);
	//					glVertex3f(CurrentHole.at(iEndId).x()*DIM/PROCSIZE,
	//						CurrentHole.at(iEndId).y()*DIM/PROCSIZE,
	//						CurrentHole.at(iEndId).z()*DIM/PROCSIZE);
	//					glEnd();
	//					glLineWidth(1);
	//				}
	//			}

	//		}
	//	}
	//}
	//glEnable(GL_LIGHTING);
};

void KW_CS2Surf::RenderTestPoints()
{
	if (this->vecTestPoint.empty())
	{
		return;
	}
	float fStep=1.0/this->vecTestPoint.size();
	glDisable(GL_LIGHTING);
	for (unsigned int i=0;i<this->vecTestPoint.size();i++)
	{
		glPointSize(10);
		glBegin(GL_POINTS);
		glColor3f(0,0,1);
		//glColor3f(0,0+(i+1)*fStep,0+(i+1)*fStep);
		glVertex3f(this->vecTestPoint.at(i).x()*DIM/PROCSIZE,this->vecTestPoint.at(i).y()*DIM/PROCSIZE,
			this->vecTestPoint.at(i).z()*DIM/PROCSIZE);
		glEnd();
		glPointSize(1);
	}
	glEnable(GL_LIGHTING);
}

void KW_CS2Surf::RenderTestSeg()
{
	if (this->vecTestSeg.empty())
	{
		return;
	}
	glDisable(GL_LIGHTING);
	for (unsigned int i=0;i<this->vecTestSeg.size();i++)
	{
		glLineWidth(3);
		glColor3f(0,0,1);
		glBegin(GL_LINES);
		glVertex3f(this->vecTestSeg.at(i).source().x()*DIM/PROCSIZE,
			this->vecTestSeg.at(i).source().y()*DIM/PROCSIZE,
			this->vecTestSeg.at(i).source().z()*DIM/PROCSIZE);
		glVertex3f(this->vecTestSeg.at(i).target().x()*DIM/PROCSIZE,
			this->vecTestSeg.at(i).target().y()*DIM/PROCSIZE,
			this->vecTestSeg.at(i).target().z()*DIM/PROCSIZE);
		glEnd();
		glLineWidth(1);
	}
	glEnable(GL_LIGHTING);
}

void KW_CS2Surf::RenderSinglePolyh()
{
	if (this->iRenderSinglePoly==CR_RENDER_NONE_SINGLE_CYLINDER)
	{
		return;
	}
	const GLfloat mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};//1.0f
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat_emission);
	GLfloat mat_amb[] =      { 0.9, 0.9, 0.9, 1.0 };//1.0
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_amb);
	GLfloat mat_dif[] =      { 0.5, 0.9, 0.4,1 };//1

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	{
		if (this->iRenderSinglePoly!=CR_RENDER_ALL_SINGLE_CYLINDER && i!=this->iRenderSinglePoly)
		{
			continue;
		}
		switch(i%12)//10
		{
		case 0:
			mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=0;
			break;
		case 1:
			mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=0;
			break;
		case 2:
			mat_dif[0]=0;mat_dif[1]=0;mat_dif[2]=1;
			//mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=1;
		    break;
		case 3:
			mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=1;
			//mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=1;
		    break;
		case 4:
			mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=1;
			//mat_dif[0]=0.1;mat_dif[1]=0.5;mat_dif[2]=0.3;
			break;
		case 5:
			mat_dif[0]=0.1;mat_dif[1]=0.5;mat_dif[2]=0.3;
			//mat_dif[0]=0.5;mat_dif[1]=0.5;mat_dif[2]=0.9;
			break;
		case 6:
			mat_dif[0]=1;mat_dif[1]=0.43;mat_dif[2]=0.72;
			//mat_dif[0]=0.9;mat_dif[1]=0.8;mat_dif[2]=0.1;
			break;
		case 7:
			mat_dif[0]=0.3;mat_dif[1]=0.8;mat_dif[2]=0.6;
			//mat_dif[0]=0.4;mat_dif[1]=0.2;mat_dif[2]=0.8;
			break;
		case 8:
			mat_dif[0]=0.4;mat_dif[1]=0.2;mat_dif[2]=0.8;
			break;
		case 9:
			mat_dif[0]=0.5;mat_dif[1]=0.5;mat_dif[2]=0.9;
			break;
		case 10:
			mat_dif[0]=0.6;mat_dif[1]=0.3;mat_dif[2]=0.1;
			break;
		case 11:
			mat_dif[0]=0.9;mat_dif[1]=0.8;mat_dif[2]=0.1;
			break;
		default:
			mat_dif[0]=1;mat_dif[1]=1;mat_dif[2]=0;
		    break;
		}

		glColor4fv(mat_dif);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);	
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		for (Facet_iterator FaceIter=vecSinglePoly.at(i).facets_begin();FaceIter!=vecSinglePoly.at(i).facets_end();FaceIter++)
		{
			//glBegin(GL_TRIANGLES);
			glBegin(GL_POLYGON);
			glNormal3f(FaceIter->normal().x(),FaceIter->normal().y(),FaceIter->normal().z());
			Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
			do 
			{
				glVertex3f(Hafc->vertex()->point().x()*DIM/PROCSIZE,
					Hafc->vertex()->point().y()*DIM/PROCSIZE,
					Hafc->vertex()->point().z()*DIM/PROCSIZE);
				Hafc++;
			} while(Hafc!=FaceIter->facet_begin());
			glEnd();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glDisable(GL_COLOR_MATERIAL);

	//glDisable(GL_LIGHTING);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_SMOOTH);//GL_LINE GL_SMOOTH 
	//glLineWidth(3);
	//float fStep=1.0/this->vecSinglePoly.size();
	//glColor3f(1,1,0);
	//for (unsigned int i=0;i<this->vecSinglePoly.size();i++)
	//{
	//	if (this->iRenderSinglePoly!=CR_RENDER_ALL_CYLINDER && i!=this->iRenderSinglePoly)
	//	{
	//		continue;
	//	}
	//	//glColor3f(0+(i+1)*fStep,0,0+(i+1)*fStep);
	//	switch(i%10)
	//	{
	//	case 0:
	//		glColor3f(1,0,0);
	//		break;
	//	case 1:
	//		glColor3f(0,1,0);
	//		break;
	//	case 2:
	//		glColor3f(0,0,1);
	//	    break;
	//	case 3:
	//		glColor3f(1,0,1);
	//	    break;
	//	case 4:
	//		glColor3f(0,1,1);
	//		break;
	//	case 5:
	//		glColor3f(0.1,0.5,0.3);
	//		break;
	//	case 6:
	//		glColor3f(0.2,0.4,0.8);
	//		break;
	//	case 7:
	//		glColor3f(0.3,0.3,0.6);
	//		break;
	//	case 8:
	//		glColor3f(0.4,0.1,0.8);
	//		break;
	//	case 9:
	//		glColor3f(0.5,0.2,0.6);
	//		break;
	//	default:
	//		glColor3f(1,1,0);
	//	    break;
	//	}
	//	for (Facet_iterator FaceIter=vecSinglePoly.at(i).facets_begin();FaceIter!=vecSinglePoly.at(i).facets_end();FaceIter++)
	//	{
	//		glBegin(GL_TRIANGLES);
	//		Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	//		do 
	//		{
	//			glVertex3f(Hafc->vertex()->point().x()*DIM/PROCSIZE,
	//				Hafc->vertex()->point().y()*DIM/PROCSIZE,
	//				Hafc->vertex()->point().z()*DIM/PROCSIZE);
	//			Hafc++;
	//		} while(Hafc!=FaceIter->facet_begin());
	//		glEnd();
	//	}
	//}
	//glLineWidth(1);
	//glEnable(GL_LIGHTING);
}

void KW_CS2Surf::RenderUnionPolyh()
{
	if (this->iRenderUnionPoly==CR_RENDER_NONE_UNION_CYLINDER)
	{
		return;
	}
	const GLfloat mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};//1.0f
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  mat_emission);
	GLfloat mat_amb[] =      { 0.9, 0.9, 0.9, 1.0 };//1.0
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_amb);
	GLfloat mat_dif[] =      { 0.5, 0.9, 0.4,1 };//1

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	for (unsigned int i=0;i<this->vecUnionPoly.size();i++)
	{
		if (this->iRenderUnionPoly!=CR_RENDER_ALL_UNION_CYLINDER && i!=this->iRenderUnionPoly)
		{
			continue;
		}
		switch(i%12)//10
		{
		case 0:
			mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=0;
			break;
		case 1:
			mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=0;
			break;
		case 2:
			mat_dif[0]=0;mat_dif[1]=0;mat_dif[2]=1;
			//mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=1;
			break;
		case 3:
			mat_dif[0]=1;mat_dif[1]=0;mat_dif[2]=1;
			//mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=1;
			break;
		case 4:
			mat_dif[0]=0;mat_dif[1]=1;mat_dif[2]=1;
			//mat_dif[0]=0.1;mat_dif[1]=0.5;mat_dif[2]=0.3;
			break;
		case 5:
			mat_dif[0]=0.1;mat_dif[1]=0.5;mat_dif[2]=0.3;
			//mat_dif[0]=0.5;mat_dif[1]=0.5;mat_dif[2]=0.9;
			break;
		case 6:
			mat_dif[0]=1;mat_dif[1]=0.43;mat_dif[2]=0.72;
			//mat_dif[0]=0.9;mat_dif[1]=0.8;mat_dif[2]=0.1;
			break;
		case 7:
			mat_dif[0]=0.3;mat_dif[1]=0.8;mat_dif[2]=0.6;
			//mat_dif[0]=0.4;mat_dif[1]=0.2;mat_dif[2]=0.8;
			break;
		case 8:
			mat_dif[0]=0.4;mat_dif[1]=0.2;mat_dif[2]=0.8;
			break;
		case 9:
			mat_dif[0]=0.5;mat_dif[1]=0.5;mat_dif[2]=0.9;
			break;
		case 10:
			mat_dif[0]=0.6;mat_dif[1]=0.3;mat_dif[2]=0.1;
			break;
		case 11:
			mat_dif[0]=0.9;mat_dif[1]=0.8;mat_dif[2]=0.1;
			break;
		default:
			mat_dif[0]=1;mat_dif[1]=1;mat_dif[2]=0;
			break;
		}

		glColor4fv(mat_dif);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		for (Facet_iterator FaceIter=vecUnionPoly.at(i).facets_begin();FaceIter!=vecUnionPoly.at(i).facets_end();FaceIter++)
		{
			//glBegin(GL_TRIANGLES);
			glBegin(GL_POLYGON);
			glNormal3f(FaceIter->normal().x(),FaceIter->normal().y(),FaceIter->normal().z());
			Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
			do 
			{
				glVertex3f(Hafc->vertex()->point().x()*DIM/PROCSIZE,
					Hafc->vertex()->point().y()*DIM/PROCSIZE,
					Hafc->vertex()->point().z()*DIM/PROCSIZE);
				Hafc++;
			} while(Hafc!=FaceIter->facet_begin());
			glEnd();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glDisable(GL_COLOR_MATERIAL);

	//glDisable(GL_LIGHTING);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_SMOOTH);//GL_LINE GL_SMOOTH 
	//glLineWidth(3);
	//float fStep=1.0/this->vecUnionPoly.size();
	//glColor3f(1,1,0);
	//for (unsigned int i=0;i<this->vecUnionPoly.size();i++)
	//{
	//	if (this->iRenderSinglePoly!=CR_RENDER_ALL_CYLINDER && i!=this->iRenderSinglePoly)
	//	{
	//		continue;
	//	}
	//	//glColor3f(0+(i+1)*fStep,0,0+(i+1)*fStep);
	//	switch(i%10)
	//	{
	//	case 0:
	//		glColor3f(1,0,0);
	//		break;
	//	case 1:
	//		glColor3f(0,1,0);
	//		break;
	//	case 2:
	//		glColor3f(0,0,1);
	//	    break;
	//	case 3:
	//		glColor3f(1,0,1);
	//	    break;
	//	case 4:
	//		glColor3f(0,1,1);
	//		break;
	//	case 5:
	//		glColor3f(0.1,0.5,0.3);
	//		break;
	//	case 6:
	//		glColor3f(0.2,0.4,0.8);
	//		break;
	//	case 7:
	//		glColor3f(0.3,0.3,0.6);
	//		break;
	//	case 8:
	//		glColor3f(0.4,0.1,0.8);
	//		break;
	//	case 9:
	//		glColor3f(0.5,0.2,0.6);
	//		break;
	//	default:
	//		glColor3f(1,1,0);
	//	    break;
	//	}
	//	for (Facet_iterator FaceIter=vecUnionPoly.at(i).facets_begin();FaceIter!=vecUnionPoly.at(i).facets_end();FaceIter++)
	//	{
	//		glBegin(GL_TRIANGLES);
	//		Halfedge_around_facet_circulator Hafc=FaceIter->facet_begin();
	//		do 
	//		{
	//			glVertex3f(Hafc->vertex()->point().x()*DIM/PROCSIZE,
	//				Hafc->vertex()->point().y()*DIM/PROCSIZE,
	//				Hafc->vertex()->point().z()*DIM/PROCSIZE);
	//			Hafc++;
	//		} while(Hafc!=FaceIter->facet_begin());
	//		glEnd();
	//	}
	//}
	//glLineWidth(1);
	//glEnable(GL_LIGHTING);
}

void KW_CS2Surf::RenderTestTriangle()
{
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//GL_LINE GL_SMOOTH 
	glLineWidth(3);
	float fStep=1.0/this->vecTestTri.size();
	for (unsigned int i=0;i<this->vecTestTri.size();i++)
	{
		glColor3f(0,0,1);
//		glColor3f(0+(i+1)*fStep,0,0+(i+1)*fStep);
		//switch(i%10)
		//{
		//case 0:
		//	glColor3f(1,0,0);
		//	break;
		//case 1:
		//	glColor3f(0,1,0);
		//	break;
		//case 2:
		//	glColor3f(0,0,1);
		//	break;
		//case 3:
		//	glColor3f(1,0,1);
		//	break;
		//case 4:
		//	glColor3f(0,1,1);
		//	break;
		//case 5:
		//	glColor3f(0.1,0.5,0.3);
		//	break;
		//case 6:
		//	glColor3f(0.2,0.4,0.8);
		//	break;
		//case 7:
		//	glColor3f(0.3,0.3,0.6);
		//	break;
		//case 8:
		//	glColor3f(0.4,0.1,0.8);
		//	break;
		//case 9:
		//	glColor3f(0.5,0.2,0.6);
		//	break;
		//default:
		//	glColor3f(1,1,0);
		//	break;
		//}
		glBegin(GL_TRIANGLES);
		glVertex3f(this->vecTestTri.at(i).vertex(0).x()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(0).y()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(0).z()*DIM/PROCSIZE);
		glVertex3f(this->vecTestTri.at(i).vertex(1).x()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(1).y()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(1).z()*DIM/PROCSIZE);
		glVertex3f(this->vecTestTri.at(i).vertex(2).x()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(2).y()*DIM/PROCSIZE,this->vecTestTri.at(i).vertex(2).z()*DIM/PROCSIZE);
		glEnd();
	}
	glLineWidth(1);
	glEnable(GL_LIGHTING);
}

