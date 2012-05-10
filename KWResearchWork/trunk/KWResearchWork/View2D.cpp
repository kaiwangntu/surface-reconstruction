// View2D.cpp : implementation file
//

#include "stdafx.h"
#include "KWResearchWork.h"
#include "View2D.h"
#include "KWResearchWorkDoc.h"
#include "MainFrm.h"

// CView2D

IMPLEMENT_DYNCREATE(CView2D, CView)

CView2D::CView2D()
{
	// OpenGL
	g_pBLDC=NULL;
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;

	hCursor_Rotate=AfxGetApp()->LoadCursor(IDC_ROTATE);
	g_fSpinZ=0.0;g_iLastPosZ=0;
	bStrokeDrawing=false;
	bAxis=true;
}

CView2D::~CView2D()
{
}

BEGIN_MESSAGE_MAP(CView2D, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_COMMAND(ID_VIEW_2DAXIS, &CView2D::OnView2daxis)
END_MESSAGE_MAP()


// CView2D drawing

void CView2D::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CView2D diagnostics

#ifdef _DEBUG
void CView2D::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CView2D::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CView2D message handlers

int CView2D::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if(InitOpenGL()==FALSE)
	{
		MessageBox("OpenGL初始化失败","OpenGL初始化失败",MB_OK);
	}
	return 0;
}

BOOL CView2D::InitOpenGL()
{
	//HWND hWnd = GetSafeHwnd();//得到应用程序句柄
	//HDC hDC = ::GetDC(hWnd);//得到设备上下文
	g_pBLDC = new CClientDC(this);

	if(SetWindowPixelFormat(g_pBLDC->GetSafeHdc())==FALSE)//设置象素格式
		return FALSE;

	if(CreateViewGLContext(g_pBLDC->GetSafeHdc())==FALSE)//创建渲染上下文
		return FALSE;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.5f,0.5f,0.5f,0.0f);
	glClearDepth(1.0f);							
	glEnable(GL_DEPTH_TEST);					
	glDepthFunc(GL_LEQUAL);	

	return TRUE;
}

BOOL CView2D::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;

	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;

	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;

	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}

	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;

	return TRUE;
}

BOOL CView2D::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);

	if(m_hGLContext==NULL)
		return FALSE;

	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;

	return TRUE;
}


void CView2D::OnDestroy()
{
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);

	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}
	if(g_pBLDC){
		delete g_pBLDC;
		g_pBLDC = NULL;
	}

	CView::OnDestroy();
}

void CView2D::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (cy<=0)
	{
		return;
	}
	wglMakeCurrent(g_pBLDC->GetSafeHdc(), m_hGLContext);

	RedrawWindow();

	CSize size(cx,cy);
	double aspect;
	aspect = (cy == 0) ? (double)size.cx : (double)size.cx/(double)size.cy;

	glViewport(0,0,size.cx,size.cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.2f,1.2f,-2.0f,2.0f,-10.0f,10.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	wglMakeCurrent(NULL,NULL);

	Invalidate(FALSE);
}

void CView2D::OnPaint()
{
	wglMakeCurrent(NULL,NULL);
	wglMakeCurrent(g_pBLDC->GetSafeHdc(), m_hGLContext);
	// Do not call CView::OnPaint() for painting messages
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();

	//	glTranslatef(0.0f,0.0f,-5.0f);
	glRotatef(g_fSpinZ,0.0f,0.0f,1.0f);

	glGetIntegerv(GL_VIEWPORT, viewport); 
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 
	glGetDoublev(GL_PROJECTION_MATRIX, projection); 

	//draw X,Y axis
	if (bAxis)
	{
		glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex2f(-1.0,0.0);
		glVertex2f(1.0,0.0);
		glColor3f(0,1,0);
		glVertex2f(0.0,1.0);
		glVertex2f(0.0,-1.0);
		glEnd();

		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0,1,0);
		glVertex2f(0.0,1.0);
		glEnd();
		glPointSize(1);
	}

	//draw the line/points from 3d view
	if (!NormalizedPointsFrom3D.empty())
	{
		for (unsigned int i=0;i<NormalizedPointsFrom3D.size()-1;i++)
		{
			glLineWidth(3);
			glBegin(GL_LINES);
			glColor3f(0,1,0);
			glVertex2f(NormalizedPointsFrom3D.at(i).x,NormalizedPointsFrom3D.at(i).y);
			glVertex2f(NormalizedPointsFrom3D.at(i+1).x,NormalizedPointsFrom3D.at(i+1).y);
			glEnd();
			glLineWidth(1);
		}

		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0,0,1);
		glVertex2f(NormalizedPointsFrom3D.at(0).x,NormalizedPointsFrom3D.at(0).y);
		glEnd();
		glPointSize(1);
	}

	//draw user curve
	glLineWidth(3.0);			
	if ((!UserCurvePoints.empty())&&bStrokeDrawing)
	{
		for (unsigned int i=0;i<UserCurvePoints.size()-1;i++)
		{
			HPCPoint pt;
			pt.x =UserCurvePoints.at(i).x;
			pt.y =UserCurvePoints.at(i).y;
			HPCPoint pt2;
			pt2.x =UserCurvePoints.at(i+1).x;
			pt2.y =UserCurvePoints.at(i+1).y;
			Point3D *pPoint3d=new Point3D;
			ScreenToGL(&pt,pPoint3d);
			Point3D *pPoint3d2=new Point3D;
			ScreenToGL(&pt2,pPoint3d2);

			glBegin(GL_LINES);
			{
				glColor3f(1,0,0);
				glVertex2f(pPoint3d->x,pPoint3d->y);
				glVertex2f(pPoint3d2->x,pPoint3d2->y);
			}
			glEnd();

			delete pPoint3d;pPoint3d=NULL;
			delete pPoint3d2;pPoint3d2=NULL;
		}
		//draw the start point
		glPointSize(5);
		HPCPoint pt;
		pt.x =UserCurvePoints.at(0).x;
		pt.y =UserCurvePoints.at(0).y;
		Point3D *pPoint3d=new Point3D;
		ScreenToGL(&pt,pPoint3d);
		glBegin(GL_POINTS);
		{
			glColor3f(0,0,1);
			glVertex2f(pPoint3d->x,pPoint3d->y);
		}
		glEnd();
		delete pPoint3d;pPoint3d=NULL;
		glPointSize(1);
	} 
	glLineWidth(1.0);			

	//draw the modified point to 3D
	glLineWidth(3.0);			
	if ((!ModifiedPointsTo3D.empty())&&!bStrokeDrawing)
	{
		for (unsigned int i=0;i<ModifiedPointsTo3D.size()-1;i++)
		{
			glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex2f(ModifiedPointsTo3D.at(i).x,ModifiedPointsTo3D.at(i).y);
			glVertex2f(ModifiedPointsTo3D.at(i+1).x,ModifiedPointsTo3D.at(i+1).y);
			glEnd();
		}
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(0,0,1);
		glVertex2f(ModifiedPointsTo3D.at(0).x,ModifiedPointsTo3D.at(0).y);
		glEnd();
		glPointSize(1);
	} 
	glLineWidth(1.0);			

	glPopMatrix();
	// Double buffer
	SwapBuffers(g_pBLDC->m_hDC);
	ValidateRect(NULL);
}

BOOL CView2D::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}


void CView2D::IfAxisOn(bool bAxisOn)
{
	bAxis=bAxisOn;
}

void CView2D::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	if (nFlags & MK_LBUTTON||nFlags & MK_MBUTTON||nFlags & MK_RBUTTON)
	{
		CRect rect;
		GetWindowRect(&rect);   
		ClipCursor(&rect);   
	}
	else
	{
		ClipCursor(NULL);//解除锁定
	}

	int g_iStepZ = point.y-g_iLastPosZ;

	if (nFlags & MK_LBUTTON)
	{
		if (nFlags & MK_SHIFT)
		{
			UserCurvePoints.push_back(point);
		}
		else
		{
			SetCursor(hCursor_Rotate);
			g_fSpinZ += 0.5f*g_iStepZ;
			if ((g_fSpinZ > 360.0f) || (g_fSpinZ < -360.0f))
			{
				g_fSpinZ = 0.0f;
			}
		}
	}
	// middle mouse button
	else if (nFlags & MK_MBUTTON)
	{
	}
	// right mouse button
	else if (nFlags & MK_RBUTTON)
	{
	}

	g_iLastPosZ  = point.y;


	Invalidate(FALSE);
	CView::OnMouseMove(nFlags, point);
}

void CView2D::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 1:
		break;
	//case 2:
	//	{
	//		POINT point;
	//		GetCursorPos(&point);
	//		ScreenToClient(&point);
	//		UserCurvePoints.push_back(CPoint(point.x,point.y));
	//		break;
	//	}

	default:
		break;
	}
	CView::OnTimer(nIDEvent);
}

void CView2D::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ((UserCurvePoints.empty())&&(nFlags & MK_SHIFT)&&(!PointsFrom3D.empty()))//
	{	
		UserCurvePoints.push_back(point);
		bStrokeDrawing=true;
//		SetTimer(2,10,0);
	}
	else
	{
		SetCursor(hCursor_Rotate);
	}
	CView::OnLButtonDown(nFlags, point);
}

void CView2D::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ((!UserCurvePoints.empty())&&(bStrokeDrawing))
	{
		bStrokeDrawing=false;
//		KillTimer(2);

		GeometryAlgorithm compute;
		compute.ProcessCurverPoint(UserCurvePoints,25.0,NormalizedPointsFrom3D.size());

		FindModifiedPointsPosition();
		UserCurvePoints.clear();
	}
	CView::OnLButtonUp(nFlags, point);
}

void CView2D::ScreenToGL(HPCPoint * point,Point3D * point3D,bool bInverse)
{
	GLdouble  winX, winY, winZ; 
	GLdouble posX, posY, posZ; 

	if (bInverse)
	{
		gluProject(point3D->x,point3D->y,point3D->z, modelview, projection, viewport, &winX, &winY, &winZ); 
		point->x=winX;point->y=winY;
	} 
	else
	{
		winX = (float)point->x; 
		winY = viewport[3] - (float)point->y;
		//		winY = (float)point->y;
		glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); 
		gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);

		point3D->x=posX;point3D->y=posY;point3D->z=posZ;
	}
}

int CView2D::GetCurrentDesiredPointsPos(vector<HPCPoint> DesiredPointsPosition)
{
	g_fSpinZ=0.0;

	//when a new curve from 3d is loaded in,clear all the current curve
	RestoredModifiedPointsTo3D.clear();
	ModifiedPointsTo3D.clear();
	PointsFrom3D.clear();

	PointsFrom3D=DesiredPointsPosition;

	Normalize3DPoints();

	return PointsFrom3D.size();
}

void CView2D::Normalize3DPoints()
{
	NormalizedPointsFrom3D.clear();
	NormalizedPointsFrom3D=PointsFrom3D;

	double MaxX=NormalizedPointsFrom3D.at(0).x;
	double MinX=MaxX;
	double MaxY=NormalizedPointsFrom3D.at(0).y;
	double MinY=MaxY;
	for (unsigned int i=1;i<NormalizedPointsFrom3D.size();i++)
	{
		if (NormalizedPointsFrom3D.at(i).x>MaxX)
		{
			MaxX=NormalizedPointsFrom3D.at(i).x;
		}
		if (NormalizedPointsFrom3D.at(i).x<MinX)
		{
			MinX=NormalizedPointsFrom3D.at(i).x;
		}

		if (NormalizedPointsFrom3D.at(i).y>MaxY)
		{
			MaxY=NormalizedPointsFrom3D.at(i).y;
		}
		if (NormalizedPointsFrom3D.at(i).y<MinY)
		{
			MinY=NormalizedPointsFrom3D.at(i).y;
		}
	}

	dScale=(MaxX-MinX>MaxY-MinY)?MaxX-MinX:MaxY-MinY;
	dScale=1/dScale;
	dCenterX=(MaxX+MinX)/2;
	dCenterY=(MaxY+MinY)/2;

	for (unsigned int i=0;i<NormalizedPointsFrom3D.size();i++)
	{
		NormalizedPointsFrom3D.at(i).x-=dCenterX;
		NormalizedPointsFrom3D.at(i).y-=dCenterY;
		NormalizedPointsFrom3D.at(i).x*=dScale;
		NormalizedPointsFrom3D.at(i).y*=dScale;
	}
}

bool CView2D::FindModifiedPointsPosition()
{
	ModifiedPointsTo3D.clear();

	vector<HPCPoint> HPUserCurvePoint;
	for (unsigned int i=0;i<UserCurvePoints.size();i++)
	{
		HPCPoint pt;
		pt.x =UserCurvePoints.at(i).x;
		pt.y =UserCurvePoints.at(i).y;
		Point3D *pPoint3d=new Point3D;
		ScreenToGL(&pt,pPoint3d);
		HPCPoint resultpoint;
		resultpoint.x=pPoint3d->x;
		resultpoint.y=pPoint3d->y;
		HPUserCurvePoint.push_back(resultpoint);
		delete pPoint3d;pPoint3d=NULL;
	}

	GeometryAlgorithm compute;
	compute.ReversePointsOrder2d(NormalizedPointsFrom3D.at(0),HPUserCurvePoint);

	ModifiedPointsTo3D=HPUserCurvePoint;

	ASSERT(ModifiedPointsTo3D.size()==NormalizedPointsFrom3D.size());

	return true;
}

bool CView2D::Restore3DPoints()
{
	RestoredModifiedPointsTo3D.clear();
	RestoredModifiedPointsTo3D=ModifiedPointsTo3D;
	if (RestoredModifiedPointsTo3D.empty())
	{
		return false;
	}

	for (unsigned int i=0;i<RestoredModifiedPointsTo3D.size();i++)
	{
		RestoredModifiedPointsTo3D.at(i).x/=dScale;
		RestoredModifiedPointsTo3D.at(i).y/=dScale;
		RestoredModifiedPointsTo3D.at(i).x+=dCenterX;
		RestoredModifiedPointsTo3D.at(i).y+=dCenterY;
	}

	return true;
}

int CView2D::SetModifiedPointsPos(vector<HPCPoint> & DesiredPointsPosition)
{
	if (!Restore3DPoints())
	{
		return 0;
	}
	DesiredPointsPosition=RestoredModifiedPointsTo3D;

	NormalizedPointsFrom3D=ModifiedPointsTo3D;
	ModifiedPointsTo3D.clear();

	return DesiredPointsPosition.size();
}

void CView2D::OnView2daxis()
{
	// TODO: Add your command handler code here
	//CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	//if (pMain!=NULL)
	//{
	//	CMenu*	mainmenu=pMain->GetMenu();
	//	if (mainmenu->GetMenuState(ID_VIEW_2DAXIS,MF_BYCOMMAND)==MF_CHECKED)
	//	{
	//		mainmenu->CheckMenuItem(ID_VIEW_2DAXIS,MF_UNCHECKED);	
	//		IfAxisOn(false);
	//	} 
	//	else
	//	{
	//		mainmenu->CheckMenuItem(ID_VIEW_2DAXIS,MF_CHECKED);	
	//		IfAxisOn(true);
	//	}
	//	RedrawWindow();
	//}
}

