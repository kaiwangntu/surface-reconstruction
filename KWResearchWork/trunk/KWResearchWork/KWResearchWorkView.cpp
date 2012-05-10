// KWResearchWorkView.cpp : implementation of the CKWResearchWorkView class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "KWResearchWork.h"

#include "KWResearchWorkDoc.h"
#include "KWResearchWorkView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKWResearchWorkView

IMPLEMENT_DYNCREATE(CKWResearchWorkView, CView)

BEGIN_MESSAGE_MAP(CKWResearchWorkView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CKWResearchWorkView construction/destruction

CKWResearchWorkView::CKWResearchWorkView()
{
	// TODO: add construction code here
	// OpenGL specific
	g_pBLDC=NULL;
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;
	//arcball
	Transform.M[0]=Transform.M[5]=Transform.M[10]=Transform.M[15]=1.0;
	Transform.M[1]=Transform.M[2]=Transform.M[3]=Transform.M[4]
	=Transform.M[6]=Transform.M[7]=Transform.M[8]=Transform.M[9]
	=Transform.M[11]=Transform.M[12]=Transform.M[13]=Transform.M[14]=0.0;
	Matrix3fSetIdentity(&LastRot);	// Reset Rotation
	Matrix3fSetIdentity(&ThisRot);	// Reset Rotation

	g_fZoom = 0.0f;g_fTransX = 0.0f;g_fTransY = 0.0f;
	hCursor_Rotate=AfxGetApp()->LoadCursor(IDC_ROTATE);
	hCursor_Move=AfxGetApp()->LoadCursor(IDC_MOVE);
	hCursor_Zoom=AfxGetApp()->LoadCursor(IDC_ZOOM);
	hCursor_PaintROI=AfxGetApp()->LoadCursor(IDC_PAINTROI);
	hCursor_Smooth=AfxGetApp()->LoadCursor(IDC_SMOOTH);
	hCursor_Pencil=AfxGetApp()->LoadCursor(IDC_PENCIL);
	g_iLastPosX=0;g_iLastPosY=0;
}

CKWResearchWorkView::~CKWResearchWorkView()
{
}

BOOL CKWResearchWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CKWResearchWorkView drawing

void CKWResearchWorkView::OnDraw(CDC* /*pDC*/)
{
	CKWResearchWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CKWResearchWorkView printing

BOOL CKWResearchWorkView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKWResearchWorkView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKWResearchWorkView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CKWResearchWorkView diagnostics

#ifdef _DEBUG
void CKWResearchWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CKWResearchWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKWResearchWorkDoc* CKWResearchWorkView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKWResearchWorkDoc)));
	return (CKWResearchWorkDoc*)m_pDocument;
}
#endif //_DEBUG


// CKWResearchWorkView message handlers
int CKWResearchWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL CKWResearchWorkView::InitOpenGL()
{
	//HWND hWnd = GetSafeHwnd();//得到应用程序句柄
	//HDC hDC = ::GetDC(hWnd);//得到设备上下文
	g_pBLDC = new CClientDC(this);

	if(SetWindowPixelFormat(g_pBLDC->GetSafeHdc())==FALSE)//设置象素格式
		return FALSE;

	if(CreateViewGLContext(g_pBLDC->GetSafeHdc())==FALSE)//创建渲染上下文
		return FALSE;

//	glClearColor(0.3f,0.3f,0.3f,1.0f);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glShadeModel(GL_SMOOTH);//GL_FLAT for patch view
	//	glEnable(GL_CULL_FACE);//enable this will make the polygon show only front face


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CKWResearchWorkDoc* pDoc=GetDocument();

//	float pos[4] = { 0, 0, 1, 0};
	float* pos=pDoc->GetLightPos();
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	//old settings
	//	float diffuse[4] = { 1, 1, 1, 1};
		float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
	//	float ambient[4] = { 1, 1, 1, 0.5};
		float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f};
	//	float specular[4] = { 1.0f, 0.0f, 0.0f, 1.0f};
		glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	//	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

	//new settings
	//GLfloat light_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
	//GLfloat light_diffuse[]  = {0.6f, 0.6f, 0.6f, 1.0f};
	//GLfloat light_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
	//glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	RenderText.BuildFont(g_pBLDC->GetSafeHdc());

	return TRUE;
}

BOOL CKWResearchWorkView::SetWindowPixelFormat(HDC hDC)
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

	//kyewong attention: don't know if this is right
	m_GLPixelIndex=32;

	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;

	return TRUE;
}

BOOL CKWResearchWorkView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);

	if(m_hGLContext==NULL)
		return FALSE;

	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;

	return TRUE;
}


void CKWResearchWorkView::OnDestroy()
{
	RenderText.KillFont();

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

void CKWResearchWorkView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	// Set OpenGL perspective, viewport and mode
	if (cy<=0)
	{
		return;
	}
	wglMakeCurrent(g_pBLDC->GetSafeHdc(), m_hGLContext);

	ArcBall.setBounds(cx,cy);

	RedrawWindow();

	CSize size(cx,cy);
	double aspect;
	aspect = (cy == 0) ? (double)size.cx : (double)size.cx/(double)size.cy;

	glViewport(0,0,size.cx,size.cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,0.01,5000.0f);//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	wglMakeCurrent(NULL,NULL);

	//Invalidate(FALSE);
	RedrawWindow();
}

void CKWResearchWorkView::OnPaint()
{
	Render(GL_RENDER);
}

BOOL CKWResearchWorkView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}

void CKWResearchWorkView::Reset(bool bInitial)
{
	//Arcball
	Matrix3fSetIdentity(&LastRot);	// Reset Rotation
	Matrix3fSetIdentity(&ThisRot);	// Reset Rotation
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);// Reset Rotation
	Transform.M[0]=Transform.M[5]=Transform.M[10]=Transform.M[15]=1;
	Transform.M[1]=Transform.M[2]=Transform.M[3]
	=Transform.M[4]=Transform.M[6]=Transform.M[7]
	=Transform.M[8]=Transform.M[9]=Transform.M[11]
	=Transform.M[12]=Transform.M[13]=Transform.M[14]=0;
	g_fZoom = 0.0f;g_fTransX = 0.0f;g_fTransY = 0.0f;
	//stop plane rotation
	KillTimer(TIMER_PLANE_ROTATE);
	KillTimer(TIMER_PLANE_WAIT_TO_ROTATE);
}

void CKWResearchWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(TIMER_PLANE_ROTATE);
	KillTimer(TIMER_PLANE_WAIT_TO_ROTATE);

	CKWResearchWorkDoc* pDoc=GetDocument();

	if (pDoc->GetManipMode()==VIEW_SELECTION_MODE)
	{
		ProcessMouseHit(point,MOUSE_LEFT_BUTTON_HIT);
		if ((pDoc->GetRBSelName()==NONE_SELECTED)||(pDoc->GetLBSelName()!=pDoc->GetRBSelName())
			|| (pDoc->GetRBSelName()==MODEL_NAME && pDoc->GetEditMode()==CREATION_MODE))
		{
			Point2fT    MousePt;// NEW: Current Mouse Point
			LastRot=ThisRot;// Set Last Static Rotation To Last Dynamic One
			MousePt.s.X=point.x;
			MousePt.s.Y=point.y;
			ArcBall.click(&MousePt);// Update Start Vector And Prepare For Dragging
			SetCursor(hCursor_Rotate);
		}
		else if (pDoc->GetLBSelName()==pDoc->GetRBSelName())
		{
		}
	}
	else if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		if ((pDoc->GetEditMode()==CREATION_MODE)&&(pDoc->GetMeshCreation().GetDrawingPlane()!=NONE_SELECTED))
		{
			if (pDoc->GetMeshCreation().CheckPlaneState())
			{
				pDoc->GetMeshCreation().Input2DProfilePoint(point);
			}
		}
		else if ((pDoc->GetEditMode()==CREATION_MODE)&&(GetKeyState(0x4A)<0 || GetKeyState(0x53)<0))//J/S key pressed,topology editing
		{
			pDoc->GetMeshCreation().Input2DProfilePointTopoEdit(point);
		}
		else if (pDoc->GetEditMode()==EDITING_MODE)
		{
			pDoc->GetMeshCreation().Input2DProfilePoint(point);
		}
		else if (pDoc->GetEditMode()==DEFORMATION_MODE)
		{
			if ((GetKeyState(0x52)<0))////R key pressed,curve to circle the ROI
			{
				if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
				{
					pDoc->GetMeshDeformation().SetDrawingCurveType(DEFORMATION_GESTURE_CIRCLE_ROI);
					pDoc->GetMeshDeformation().InputCurvePoint2D(point);
				}
			}
			else if (GetKeyState(0x50)<0)//P key pressed,points to paint the ROI
			{
				SetCursor(hCursor_PaintROI);
				if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
				{
					pDoc->GetMeshDeformation().SetDrawingCurveType(DEFORMATION_GESTURE_PAINT_ROI);
					pDoc->GetMeshDeformation().InputCurvePoint2D(point);
					pDoc->GetMeshDeformation().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
				}
			}
			else 
			{
				pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			}
		}
		else if (pDoc->GetEditMode()==CUTTING_MODE)
		{
			pDoc->GetMeshCutting().InputCurvePoint2D(point);
		}
		else if (pDoc->GetEditMode()==EXTRUSION_MODE)
		{
			pDoc->GetMeshExtrusion().InputCurvePoint2D(point);
		}
		else if (pDoc->GetEditMode()==SMOOTHING_MODE)
		{
			//if (GetKeyState(0x46)<0)//f key pressed,scratch to smooth
			//{
				SetCursor(hCursor_Smooth);
				pDoc->GetMeshSmoothing().Init(pDoc);
				pDoc->GetMeshSmoothing().InputCurvePoint2D(point);
				pDoc->GetMeshSmoothing().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
			//}
		}
	}

	CView::OnLButtonDown(nFlags, point);
	RedrawWindow();
	return;


	//CKWResearchWorkDoc* pDoc=GetDocument();


	if (nFlags & MK_SHIFT)//(this->DeformationCurvePoint.empty())&&
	{	
		//pDoc->GetMeshExtrusion().Init(pDoc);
		//pDoc->GetMeshCutting().Init(pDoc);
		//pDoc->GetMeshDeformation().Init(pDoc);

		//pDoc->GetMeshDeformation().SetDrawingCurveType(6);//handle vertex, if only one point
		//pDoc->GetMeshDeformation().InputCurvePoint2D(point);


		//if (GetKeyState(VK_LSHIFT)<0)
		//{
		//	pDoc->GetMeshDeformation().SetHandleStrokeType(true);
		//}
		//else
		//{
		//	pDoc->GetMeshDeformation().SetHandleStrokeType(false);
		//}
	} 
	else if (GetKeyState(VK_LMENU)<0)//left alt key pressed 
	{
		pDoc->GetMeshDeformation().SetDrawingCurveType(7);//handle vertex, if only one point
		pDoc->GetMeshDeformation().InputCurvePoint2D(point);
	}
	//else if (GetKeyState(VK_RMENU)<0)//right alt key pressed
	//{
		//pDoc->GetMeshDeformation().SetDrawingCurveType(3);
		//pDoc->GetMeshDeformation().InputCurvePoint2D(point);
	//}
	//else if ((GetKeyState(0x52)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//R key pressed,curve to circle the ROI
	//{
		//pDoc->GetMeshDeformation().SetDrawingCurveType(4);
		//pDoc->GetMeshDeformation().InputCurvePoint2D(point);
	//}
	else if ((GetKeyState(0x4D)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//M key pressed,frontal part selected
	{
		pDoc->GetMeshDeformation().SetDrawingCurveType(8);
		pDoc->GetMeshDeformation().InputCurvePoint2D(point);
	}
	else if ((GetKeyState(0x4E)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//N key pressed,all selected
	{
		pDoc->GetMeshDeformation().SetDrawingCurveType(9);
		pDoc->GetMeshDeformation().InputCurvePoint2D(point);
	}
	//else if ((GetKeyState(0x50)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//P key pressed,points to paint the ROI
	//{
		//SetCursor(hCursor_PaintROI);
		//if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
		//{
		//	pDoc->GetMeshDeformation().SetDrawingCurveType(5);
		//	pDoc->GetMeshDeformation().InputCurvePoint2D(point);
		//	pDoc->GetMeshDeformation().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
		//}
	//}
	else if ((GetKeyState(0x42)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//B key pressed
	{
		SetCursor(hCursor_PaintROI);
		pDoc->GetMeshDeformation().SetDrawingCurveType(10);
		pDoc->GetMeshDeformation().InputCurvePoint2D(point);
		pDoc->GetMeshDeformation().Conver2DCurveTo3D(pDoc->GetMesh());
	}
	//else if (GetKeyState(0x45)<0)//e key pressed &&(ExtrusionCurvePoint.empty())
	//{
	//	pDoc->GetMeshDeformation().Init(pDoc);
	//	pDoc->GetMeshCutting().Init(pDoc);
	//	pDoc->GetMeshExtrusion().Init(pDoc);
	//	pDoc->GetMeshExtrusion().SetDrawingCurveType(1);
	//	pDoc->GetMeshExtrusion().InputCurvePoint2D(point);
	//}
	//else if (GetKeyState(0x53)<0)//s key pressed &&(ExtrusionCurvePoint.empty())
	//{
	//	pDoc->GetMeshExtrusion().SetDrawingCurveType(2);
	//	pDoc->GetMeshExtrusion().InputCurvePoint2D(point);
	//}
	//else if ((GetKeyState(0x43)<0)&&(pDoc->GetEditMode()==CUTTING_MODE))//c key pressed &&(this->CuttingCurvePoint.empty())
	//{
	//	pDoc->GetMeshDeformation().Init(pDoc);
	//	pDoc->GetMeshExtrusion().Init(pDoc);
	//	pDoc->GetMeshCutting().Init(pDoc);
	//	pDoc->GetMeshCutting().SetDrawingCurveType(1);
	//	pDoc->GetMeshCutting().InputCurvePoint2D(point);
	//}
	//else if ((GetKeyState(0x4F)<0)&&(pDoc->GetEditMode()==CUTTING_MODE))//o key pressed
	//{
	//	pDoc->GetMeshCutting().SetDrawingCurveType(2);
	//	pDoc->GetMeshCutting().InputCurvePoint2D(point);
	//}
	//else if ((GetKeyState(0x48)<0)&&(pDoc->GetEditMode()==CUTTING_MODE))//H key pressed
	//{
	//	pDoc->GetMeshDeformation().Init(pDoc);
	//	pDoc->GetMeshExtrusion().Init(pDoc);
	//	pDoc->GetMeshCutting().Init(pDoc);
	//	pDoc->GetMeshCutting().SetDrawingCurveType(3);
	//	pDoc->GetMeshCutting().InputCurvePoint2D(point);
	//}
	//else if ((GetKeyState(0x46)<0)&&(pDoc->GetEditMode()==SMOOTHING_MODE))//f key pressed
	//{
	//	SetCursor(hCursor_Smooth);
	//	pDoc->GetMeshDeformation().Init(pDoc);
	//	pDoc->GetMeshCutting().Init(pDoc);
	//	pDoc->GetMeshExtrusion().Init(pDoc);
	//	pDoc->GetMeshSmoothing().Init(pDoc);
	//	pDoc->GetMeshSmoothing().InputCurvePoint2D(point);
	//	pDoc->GetMeshSmoothing().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
	//	RedrawWindow();
	//}
	else if ((GetKeyState(0x54)<0)&&(pDoc->GetEditMode()==TEST_MODE))//t key pressed
	{
		SetCursor(hCursor_Smooth);
		pDoc->GetMeshDeformation().Init(pDoc);
		pDoc->GetMeshCutting().Init(pDoc);
		pDoc->GetMeshExtrusion().Init(pDoc);
		pDoc->GetMeshSmoothing().Init(pDoc);
		pDoc->GetTest().Init(pDoc);
		pDoc->GetTest().InputCurvePoint2D(point);
		RedrawWindow();
	}
	else
	{
		//Point2fT    MousePt;// NEW: Current Mouse Point
		//LastRot=ThisRot;// Set Last Static Rotation To Last Dynamic One
		//MousePt.s.X=point.x;
		//MousePt.s.Y=point.y;
		//ArcBall.click(&MousePt);// Update Start Vector And Prepare For Dragging
		//SetCursor(hCursor_Rotate);
	}
	CView::OnLButtonDown(nFlags, point);
}

void CKWResearchWorkView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CKWResearchWorkDoc* pDoc=GetDocument();

	if (pDoc->GetManipMode()==VIEW_SELECTION_MODE)
	{
		if (pDoc->GetRBSelName()==NONE_SELECTED)
		{
			//do nothing
		}
		else if (pDoc->GetLBSelName()==pDoc->GetRBSelName())
		{
			//use short key instead of mouse to translate the reference plane
			//if (pDoc->GetEditMode()==CREATION_MODE)//stop translation of the reference plane in creation
			//{
			//	pDoc->GetMeshCreation().StopTranslateDrawingPlane();
			//}
		}
	}
	else if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		if ((pDoc->GetEditMode()==CREATION_MODE)&&(pDoc->GetMeshCreation().GetDrawingPlane()!=NONE_SELECTED))
		{
			pDoc->GetMeshCreation().Convert2DProfileTo3D();
		}
		else if ((pDoc->GetEditMode()==CREATION_MODE)&&(GetKeyState(0x4A)<0 || GetKeyState(0x53)<0))//J/S key pressed,topology editing
		{
			if (GetKeyState(0x4A)<0)//J key pressed,topology editing--join
			{
				pDoc->GetMeshCreation().Convert2DProfileTo3DTopoEdit(CR_TOPO_JOIN,pDoc->GetMesh(),pDoc->GetDefaultColor());
			}
			else if (GetKeyState(0x53)<0)//S key pressed,topology editing--split
			{
				pDoc->GetMeshCreation().Convert2DProfileTo3DTopoEdit(CR_TOPO_SPLIT,pDoc->GetMesh(),pDoc->GetDefaultColor());
			}
		}
		else if (pDoc->GetEditMode()==EDITING_MODE)
		{
			pDoc->GetMeshEditing().Convert2DProfileTo3D();
		}
		else if (pDoc->GetEditMode()==DEFORMATION_MODE)
		{
			pDoc->GetMeshDeformation().Conver2DCurveTo3D(pDoc->GetMesh());
		}
		else if (pDoc->GetEditMode()==CUTTING_MODE)
		{
			pDoc->GetMeshCutting().Conver2DCurveTo3D(pDoc->GetMesh());
		}
		else if (pDoc->GetEditMode()==EXTRUSION_MODE)
		{
			pDoc->GetMeshExtrusion().Conver2DCurveTo3D(pDoc->GetMesh());
		}
		else if(pDoc->GetEditMode()==SMOOTHING_MODE)
		{
			pDoc->GetMeshSmoothing().ClearROI();
		}
	}

	RedrawWindow();
	CView::OnLButtonUp(nFlags, point);
	return;

	if(pDoc->GetMeshDeformation().GetDrawingCurveType()!=0)
	{
		//pDoc->GetMeshDeformation().Conver2DCurveTo3D(pDoc->GetMesh());
	}
	//else if(pDoc->GetMeshExtrusion().GetDrawingCurveType()!=0)
	//{
	//	pDoc->GetMeshExtrusion().Conver2DCurveTo3D(pDoc->GetMesh());
	//}
	//else if(pDoc->GetMeshCutting().GetDrawingCurveType()!=0)
	//{
	//	pDoc->GetMeshCutting().Conver2DCurveTo3D(pDoc->GetMesh());
	//}
	//else if(pDoc->GetEditMode()==SMOOTHING_MODE)
	//{
	//	pDoc->GetMeshSmoothing().ClearROI();
	//}
	else if(pDoc->GetEditMode()==TEST_MODE)
	{
		//pDoc->GetTest().Conver2DCurveTo3D(this->modelview,this->projection,this->viewport);
	}

//	Invalidate(FALSE);
	RedrawWindow();
	CView::OnLButtonUp(nFlags, point);
}

void CKWResearchWorkView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(TIMER_PLANE_ROTATE);
	KillTimer(TIMER_PLANE_WAIT_TO_ROTATE);
	CKWResearchWorkDoc* pDoc=GetDocument();
	if (pDoc->GetManipMode()==VIEW_SELECTION_MODE)
	{
		ProcessMouseHit(point,MOUSE_RIGHT_BUTTON_HIT);
		if (pDoc->GetRBSelName()==NONE_SELECTED)
		{
			SetCursor(hCursor_Move);
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().SetDrawingPlane();
			}
			else if (pDoc->GetEditMode()==DEFORMATION_MODE)
			{
				pDoc->GetMeshDeformation().SetSelectedItem();
			}
			else if (pDoc->GetEditMode()==EXTRUSION_MODE)
			{
				pDoc->GetMeshExtrusion().SetSelectedItem();
			}
		}
		else
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().SetDrawingPlane();
			}
			else if (pDoc->GetEditMode()==DEFORMATION_MODE)
			{
				pDoc->GetMeshDeformation().SetSelectedItem();
			}
			else if (pDoc->GetEditMode()==EXTRUSION_MODE)
			{
				pDoc->GetMeshExtrusion().SetSelectedItem();
			}
		}
	}
	else if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		//do nothing
	}

	CView::OnRButtonDown(nFlags, point);
	RedrawWindow();
}

void CKWResearchWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CKWResearchWorkDoc* pDoc=GetDocument();

	//if (nFlags & MK_LBUTTON||nFlags & MK_MBUTTON||nFlags & MK_RBUTTON)
	//{
	//	CRect rect;
	//	GetWindowRect(&rect);   
	//	ClipCursor(&rect);   
	//}
	//else
	//{
	//	ClipCursor(NULL);//解除锁定
	//}
	int g_iStepX = point.x-g_iLastPosX;
	int g_iStepY = point.y-g_iLastPosY;

	if (pDoc->GetManipMode()==VIEW_SELECTION_MODE)
	{
		if (((pDoc->GetRBSelName()==NONE_SELECTED))||(pDoc->GetLBSelName()!=pDoc->GetRBSelName())
			|| (pDoc->GetRBSelName()==MODEL_NAME && pDoc->GetEditMode()==CREATION_MODE))
		{
			if (nFlags & MK_LBUTTON)
			{
				SetCursor(hCursor_Rotate);
				Quat4fT ThisQuat;
				Point2fT    MousePt;// NEW: Current Mouse Point
				MousePt.s.X=point.x;
				MousePt.s.Y=point.y;
				ArcBall.drag(&MousePt,&ThisQuat);// Update End Vector And Get Rotation As Quaternion
				Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);// Convert Quaternion Into Matrix3fT
				//each time the mouse is dragged,the rotation angle from the rest state is re-calculated   
				//LastRot is only computed when the mouse is pressed        --by KW
				Matrix3fMulMatrix3f(&ThisRot, &LastRot);// Accumulate Last Rotation Into This One
				Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);
			}
			else if (nFlags & MK_RBUTTON)
			{
				SetCursor(hCursor_Move);
				g_fTransX  += 0.01f*g_iStepX;
				g_fTransY  -= 0.01f*g_iStepY;
			}
		}
		else if (pDoc->GetLBSelName()==pDoc->GetRBSelName())
		{
			//use short key instead of mouse to translate the reference plane
			//if ((pDoc->GetEditMode()==CREATION_MODE)&&(nFlags & MK_LBUTTON))//translate the reference plane in creation
			//{
			//	SetCursor(hCursor_Move);
			//	pDoc->GetMeshCreation().TranslateDrawingPlane(0.01f*g_iStepX);
			//}
			if (pDoc->GetEditMode()==DEFORMATION_MODE && (nFlags & MK_LBUTTON))
			{
				SetCursor(hCursor_Move);
				pDoc->GetMeshDeformation().ManipSelItem(g_iStepX,g_iStepY);
			}
			else if (pDoc->GetEditMode()==EXTRUSION_MODE && (nFlags & MK_LBUTTON))
			{
				SetCursor(hCursor_Rotate);
				pDoc->GetMeshExtrusion().ManipSelItem(g_iStepX,g_iStepY);
			}
		}
	}
	else if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		if ((pDoc->GetEditMode()==CREATION_MODE)&&(pDoc->GetMeshCreation().GetDrawingPlane()!=NONE_SELECTED)
			&&((nFlags & MK_LBUTTON)))
		{
			pDoc->GetMeshCreation().Input2DProfilePoint(point);
		}
		else if ((pDoc->GetEditMode()==CREATION_MODE)&&((nFlags & MK_LBUTTON))&&(GetKeyState(0x4A)<0 || GetKeyState(0x53)<0))
		{
			pDoc->GetMeshCreation().Input2DProfilePointTopoEdit(point);
		}
		else if (pDoc->GetEditMode()==EDITING_MODE && (nFlags & MK_LBUTTON))
		{
			pDoc->GetMeshEditing().Input2DProfilePoint(point);
		}
		else if (pDoc->GetEditMode()==DEFORMATION_MODE && (nFlags & MK_LBUTTON))
		{
			if ((GetKeyState(0x52)<0))//R key pressed,curve to circle the ROI
			{
				if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
				{
					pDoc->GetMeshDeformation().InputCurvePoint2D(point);
				}
			}
			else if (GetKeyState(0x50)<0)//P key pressed,points to paint the ROI
			{
				SetCursor(hCursor_PaintROI);
				if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
				{
					pDoc->GetMeshDeformation().InputCurvePoint2D(point);
					pDoc->GetMeshDeformation().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
				}
			}
			else
			{
				pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			}

		}
		else if (pDoc->GetEditMode()==CUTTING_MODE && (nFlags & MK_LBUTTON))
		{
			pDoc->GetMeshCutting().InputCurvePoint2D(point);
		}
		else if (pDoc->GetEditMode()==EXTRUSION_MODE && (nFlags & MK_LBUTTON))
		{
			pDoc->GetMeshExtrusion().InputCurvePoint2D(point);
		}
		else if (pDoc->GetEditMode()==SMOOTHING_MODE && (nFlags & MK_LBUTTON))
		{
			//if (GetKeyState(0x46)<0)//f key pressed,scratch to smooth
			//{
				SetCursor(hCursor_Smooth);
				pDoc->GetMeshSmoothing().InputCurvePoint2D(point);
				pDoc->GetMeshSmoothing().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
			//}
		}
	}

	g_iLastPosX  = point.x;
	g_iLastPosY  = point.y;

	RedrawWindow();

	CView::OnMouseMove(nFlags, point);
	return;




	if (nFlags & MK_LBUTTON)
	{
		if (nFlags & MK_SHIFT)
		{
			//pDoc->GetMeshDeformation().SetDrawingCurveType(1);//handle curve, if more than one point
			//pDoc->GetMeshDeformation().InputCurvePoint2D(point);
		}
		//else if (nFlags & MK_CONTROL)
		//{
			//SetCursor(hCursor_Rotate);
			//if ((pDoc->GetEditMode()==DEFORMATION_MODE)&&(!pDoc->GetMeshDeformation().GetHandleCurvePoint().empty()))
			//{
				//pDoc->GetMeshDeformation().SetPlaneSpin(g_iStepX);
				//if ((pDoc->GetMeshDeformation().GetPlaneSpin()> 180.0) || (pDoc->GetMeshDeformation().GetPlaneSpin()< -180.0))
				//{
				//	pDoc->GetMeshDeformation().SetPlaneSpin(0.0);
				//}
				//pDoc->GetMeshDeformation().RotateDeformationPlane();
			//}else 
			//if ((pDoc->GetEditMode()==EXTRUSION_MODE)&&(!pDoc->GetMeshExtrusion().GetExtrusionClosedCurveVertex3d().empty()))
			//{
			//	pDoc->GetMeshExtrusion().SetPlaneSpin(g_iStepX);
			//	if ((pDoc->GetMeshExtrusion().GetPlaneSpin()> 90.0) || (pDoc->GetMeshExtrusion().GetPlaneSpin()< -90.0))
			//	{
			//		pDoc->GetMeshExtrusion().SetPlaneSpin(0.0);
			//	}
			//	if (GetKeyState(VK_LCONTROL)<0)
			//	{
			//		pDoc->GetMeshExtrusion().RotateExtrudeSilhPlaneX();
			//	}
			//	else if (GetKeyState(VK_RCONTROL)<0)
			//	{
			//		pDoc->GetMeshExtrusion().RotateExtrudeSilhPlaneY();
			//	}
			//} 
			//else if (pDoc->GetEditMode()==CUTTING_MODE)
			//{
				//pDoc->GetMeshCutting().SetPlaneSpin(g_iStepX);
				//if ((pDoc->GetMeshCutting().GetPlaneSpin()> 180.0) || (pDoc->GetMeshCutting().GetPlaneSpin()< -180.0))
				//{
				//	pDoc->GetMeshCutting().SetPlaneSpin(0.0);
				//}
				//if (GetKeyState(VK_LCONTROL)<0)
				//{
				//	pDoc->GetMeshCutting().RotateTunnelingPlaneX();
				//}
			//}
		//}
		//else if (GetKeyState(VK_MENU)<0)//alt key pressed
		//{
			//if ((!pDoc->GetMeshDeformation().GetHandleCurvePoint().empty()))
			//{
			//	if (GetKeyState(VK_LMENU)<0)//left alt key
			//	{
			//		pDoc->GetMeshDeformation().SetDrawingCurveType(2);//handle curve, if more than one point
			//	}
			//	pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			//}
		//}
		//else if ((GetKeyState(0x52)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//R key pressed
		//{
			//if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
			//{
			//	pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			//}
		//}
		else if ((GetKeyState(0x4D)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//M key pressed
		{
			pDoc->GetMeshDeformation().SetDrawingCurveType(8);
			pDoc->GetMeshDeformation().InputCurvePoint2D(point);
		}
		else if ((GetKeyState(0x4E)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//N key pressed,all selected
		{
			pDoc->GetMeshDeformation().SetDrawingCurveType(9);
			pDoc->GetMeshDeformation().InputCurvePoint2D(point);
		}
		//else if ((GetKeyState(0x50)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//P key pressed
		//{
			//SetCursor(hCursor_PaintROI);
			//if ((!pDoc->GetMeshDeformation().GetHandleNbVertex().empty()))
			//{
			//	pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			//	pDoc->GetMeshDeformation().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
			//}
		//}
		else if ((GetKeyState(0x42)<0)&&(pDoc->GetEditMode()==DEFORMATION_MODE))//B key pressed
		{
			SetCursor(hCursor_PaintROI);
			pDoc->GetMeshDeformation().SetDrawingCurveType(10);
			pDoc->GetMeshDeformation().InputCurvePoint2D(point);
			pDoc->GetMeshDeformation().Conver2DCurveTo3D(pDoc->GetMesh());
		}
		//else if ((GetKeyState(0x45)<0)||(GetKeyState(0x53)<0))//e or s key pressed
		//{
		//	pDoc->GetMeshExtrusion().InputCurvePoint2D(point);
		//}
		//else if (((GetKeyState(0x43)<0)||(GetKeyState(0x4F)<0)||(GetKeyState(0x48)<0))&&(pDoc->GetEditMode()==CUTTING_MODE))
		//	//C or O or H key pressed
		//{
		//	pDoc->GetMeshCutting().InputCurvePoint2D(point);
		//}
		//else if ((GetKeyState(0x46)<0)&&(pDoc->GetEditMode()==SMOOTHING_MODE))//f key pressed
		//{
		//	SetCursor(hCursor_Smooth);
		//	pDoc->GetMeshSmoothing().InputCurvePoint2D(point);
		//	pDoc->GetMeshSmoothing().PaintROIVertices(pDoc->GetMesh(),this->modelview,this->projection,this->viewport);
		//}
		else if ((GetKeyState(0x54)<0)&&(pDoc->GetEditMode()==TEST_MODE))//t key pressed
		{
			SetCursor(hCursor_Smooth);
			pDoc->GetTest().InputCurvePoint2D(point);
		}
		//else
		//{
		//	SetCursor(hCursor_Rotate);
		//	Quat4fT ThisQuat;
		//	Point2fT    MousePt;// NEW: Current Mouse Point
		//	MousePt.s.X=point.x;
		//	MousePt.s.Y=point.y;
		//	ArcBall.drag(&MousePt,&ThisQuat);// Update End Vector And Get Rotation As Quaternion
		//	Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);// Convert Quaternion Into Matrix3fT
		//	Matrix3fMulMatrix3f(&ThisRot, &LastRot);// Accumulate Last Rotation Into This One
		//	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);
		//}
	}
	// right mouse button
	else if (nFlags & MK_RBUTTON)
	{
		SetCursor(hCursor_Move);
		//if (GetKeyState(VK_MENU)<0)//alt key pressed
		//{
		//	if (!pDoc->GetMeshDeformation().GetHandleCurvePoint().empty())//handle vertices must be selected
		//	{
		//		pDoc->GetMeshDeformation().TranslateDeformCurvePoint3dOnBFPlane(g_iStepX*0.1,g_iStepY*0.1);
		//	}
		//}
		//else
		{
			g_fTransX  += 0.01f*g_iStepX;
			g_fTransY  -= 0.01f*g_iStepY;
		}
	}

	g_iLastPosX  = point.x;
	g_iLastPosY  = point.y;

	//	Invalidate(FALSE);
	RedrawWindow();

	CView::OnMouseMove(nFlags, point);
}

BOOL CKWResearchWorkView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(TIMER_PLANE_ROTATE);
	KillTimer(TIMER_PLANE_WAIT_TO_ROTATE);
	CKWResearchWorkDoc* pDoc=GetDocument();

	if (pDoc->GetManipMode()==VIEW_SELECTION_MODE)
	{
		if (pDoc->GetRBSelName()==NONE_SELECTED || (pDoc->GetRBSelName()==MODEL_NAME && pDoc->GetEditMode()==CREATION_MODE))
		{
			// middle mouse button
			SetCursor(hCursor_Zoom);
			if(zDelta>0)
			{
				g_fZoom+=0.15;
			}
			else
			{
				g_fZoom-=0.15;
			}
		}
		else if (pDoc->GetEditMode()==CREATION_MODE)
		{
			pDoc->GetMeshCreation().AdjustPlaneBoundary(zDelta);
		}
		else if (pDoc->GetEditMode()==DEFORMATION_MODE)
		{
			pDoc->GetMeshDeformation().AdjustPlaneBoundary(zDelta);
		}
		else if (pDoc->GetEditMode()==EXTRUSION_MODE)
		{
			pDoc->GetMeshExtrusion().AdjustPlaneBoundary(zDelta);
		}
	}
	else if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		//do nothing
	}


	RedrawWindow();
	return CView::OnMouseWheel(nFlags, zDelta, pt);

	if (nFlags & MK_CONTROL)
	{
		if (pDoc->GetEditMode()==CREATION_MODE)
		{
		}
		else if (pDoc->GetEditMode()==DEFORMATION_MODE)
		{
			pDoc->GetMeshDeformation().AdjustPlaneBoundary(zDelta);
		}
		else if (pDoc->GetEditMode()==EXTRUSION_MODE)
		{
			pDoc->GetMeshExtrusion().AdjustPlaneBoundary(zDelta);
		}
		else if (pDoc->GetEditMode()==CUTTING_MODE)
		{
			//pDoc->GetMeshCutting().AdjustPlaneBoundary(zDelta);
		}
	}
	else if(zDelta>0)
	{
		g_fZoom+=0.15;
	}
	else
	{
		g_fZoom-=0.15;
	}

//	Invalidate(FALSE);
	RedrawWindow();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CKWResearchWorkView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//keyboard messages
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST && pMsg->message==WM_KEYDOWN)
	{
		KillTimer(TIMER_PLANE_ROTATE);
		KillTimer(TIMER_PLANE_WAIT_TO_ROTATE);
		CKWResearchWorkDoc* pDoc=GetDocument();
		if(pMsg->wParam==VK_RETURN)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				if (pDoc->GetMeshCreation().FitLastPlaneCurves())
				{
					pDoc->GetMeshCreation().GenerateMesh(pDoc->GetMesh(),pDoc->GetDefaultColor());
				}
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_ESCAPE)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().CancelLastInput();
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_DELETE)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().DeleteSpecifiedCS();
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_F1)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().CopyCNFromLastParaPlane(0);
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_F2)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().CopyCNFromLastParaPlane(1);
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pMsg->wParam==VK_F3)
		{
			if (pDoc->GetEditMode()==CREATION_MODE)
			{
				pDoc->GetMeshCreation().CopyCNFromLastParaPlane(2);
				RedrawWindow();
				return TRUE;
			}
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (GetKeyState(0x52)<0))//R key pressed in creation mode,reset the drawing plane
		{
			pDoc->GetMeshCreation().ResetDrawingPlane();
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_RIGHT))//Right key pressed in creation mode
		{
			pDoc->GetMeshCreation().RotateDrawingPlane(0);
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_LEFT))//Left key pressed in creation mode
		{
			pDoc->GetMeshCreation().RotateDrawingPlane(1);
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_UP))//Up key pressed in creation mode
		{
			pDoc->GetMeshCreation().RotateDrawingPlane(2);
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_DOWN))//Down key pressed in creation mode
		{
			pDoc->GetMeshCreation().RotateDrawingPlane(3);
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_PRIOR))//PageUp key pressed in creation mode
		{
			pDoc->GetMeshCreation().TranslateDrawingPlane(0);
			RedrawWindow();
			return TRUE;
		}
		else if (pDoc->GetEditMode()==CREATION_MODE && (pMsg->wParam==VK_NEXT))//PageDown key pressed in creation mode
		{
			pDoc->GetMeshCreation().TranslateDrawingPlane(1);
			RedrawWindow();
			return TRUE;
		}
	}
	//keyboard messages
	else if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST && pMsg->message==WM_KEYUP)
	{
		CKWResearchWorkDoc* pDoc=GetDocument();
		if (pDoc->GetEditMode()==CREATION_MODE && 
			(pMsg->wParam==VK_PRIOR||pMsg->wParam==VK_NEXT||pMsg->wParam==VK_RIGHT
			||pMsg->wParam==VK_LEFT||pMsg->wParam==VK_UP||pMsg->wParam==VK_DOWN))//PageDown/PageUp key pressed in creation mode
		{
			pDoc->GetMeshCreation().StopTransformDrawingPlane();
			RedrawWindow();
			return TRUE;
		}
	}

	 return CView::PreTranslateMessage(pMsg);
}

void CKWResearchWorkView::ProcessMouseHit(CPoint point,int iButton)
{
	GLuint selectBuf[512];
	GLint iHits;

	glSelectBuffer(512, selectBuf);
	glRenderMode (GL_SELECT);
	glInitNames();

	RECT rect;
	GetWindowRect(&rect);
	int cx=rect.right-rect.left;
	int cy=rect.bottom-rect.top;
	double aspect;
	aspect = (cy == 0) ? (double)cx : (double)cx/(double)cy;

	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity ();
	//  在鼠标位置生成5X5像素区域
	gluPickMatrix ((GLdouble) point.x, (GLdouble) (this->viewport[3] - point.y), 
		5.0, 5.0, this->viewport);
	gluPerspective(45,aspect,0.01,5000.0f);//same as in OnSize function
	glMatrixMode(GL_MODELVIEW);

	Render(GL_SELECT);

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	iHits=glRenderMode(GL_RENDER);

	CKWResearchWorkDoc* pDoc=GetDocument();
	if (iButton==MOUSE_LEFT_BUTTON_HIT)
	{
		pDoc->SetLBSelName(GetSelectName(iHits,selectBuf));
	}
	else if (iButton==MOUSE_RIGHT_BUTTON_HIT)
	{
		pDoc->SetRBSelName(GetSelectName(iHits,selectBuf));
	}
}

int CKWResearchWorkView::GetSelectName(GLint hits, GLuint buffer[])
{
	GLuint iNumName, *ptr;
	vector<vector<GLuint>> names;//reserved for further use
	//priority: first consider min nearZ value; for similar nearZ values,consider max name
	float fMinNearZ=1000.0;
	const float fErrThre=0.0003;
	int iMaxName=NONE_SELECTED;

	DBWindowWrite("hits = %d\n", hits);
	ptr = (GLuint *) buffer;
	for (int i = 0; i < hits; i++) 
	{
		iNumName = *ptr;
		float fCurrentNearZ=0.0;
		//DBWindowWrite(" number of names for this hit = %d\n", iNumName); 
		ptr++;
		//DBWindowWrite("  z1 is %g;", (float) *ptr/0x7fffffff); 
		fCurrentNearZ=(float) *ptr/0x7fffffff;
		ptr++;
		//DBWindowWrite(" z2 is %g\n", (float) *ptr/0x7fffffff); 
		ptr++;
		//DBWindowWrite("   names are ");
		vector<GLuint> name;
		if (fCurrentNearZ-fMinNearZ<fErrThre)
		{
			for (unsigned int j = 0; j < iNumName; j++) 
			{ 
				name.push_back(*ptr);
				if (iMaxName<(int)*ptr)
				{
					iMaxName=*ptr;
					fMinNearZ=fCurrentNearZ;
				}
				//DBWindowWrite("%d ", *ptr);
				ptr++;
			}
		}
		else
		{
			for (unsigned int j = 0; j < iNumName; j++) 
			{ 
				name.push_back(*ptr);
				//DBWindowWrite("%d ", *ptr);
				ptr++;
			}
		}
		if (!name.empty())
		{
			names.push_back(name);
		}
		//DBWindowWrite("\n");
	}
	DBWindowWrite("select name: %d\n",iMaxName);
	return iMaxName;
}

void CKWResearchWorkView::ScreenToGL(HPCPoint * point,Point3D * point3D,bool bInverse)
{
	//GLint    viewport[4]; 
	//GLdouble modelview[16]; 
	//GLdouble projection[16]; 
	GLdouble  winX, winY, winZ; 
	GLdouble posX, posY, posZ; 

	//glGetIntegerv(GL_VIEWPORT, viewport); 
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 
	//glGetDoublev(GL_PROJECTION_MATRIX, projection); 

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

void CKWResearchWorkView::saveSceneImage(const char* filename)
{
	wglMakeCurrent(g_pBLDC->m_hDC,m_hGLContext);

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	int width  = viewport[2];
	int height = viewport[3];
	width -= width%4;
	int x=0;
	int y=0;

	GLubyte * bmpBuffer = NULL;
	bmpBuffer = new GLubyte[width*height*3*sizeof(GLubyte)];
	if (!bmpBuffer)
		return;// FALSE;
	glReadPixels((GLint)x, (GLint)y, (GLint)width, (GLint)height,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, bmpBuffer);
	wglMakeCurrent(g_pBLDC->m_hDC,NULL);


	FILE *filePtr;
	fopen_s(&filePtr, filename, "wb");
	if (!filePtr)
		return;// FALSE;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42; //"BM"
	bitmapFileHeader.bfSize = width*height*3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(bitmapFileHeader), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(bitmapInfoHeader), 1, filePtr);
	fwrite(bmpBuffer, width*height*3, 1, filePtr);
	fclose(filePtr);
	delete [] bmpBuffer;

	return;// TRUE; 

}

void CKWResearchWorkView::Render(GLenum mode)
{
	wglMakeCurrent(NULL,NULL);
	wglMakeCurrent(g_pBLDC->GetSafeHdc(), m_hGLContext);


	CKWResearchWorkDoc* pDoc=GetDocument();

	if (pDoc->GetManipMode()==SKETCH_MODE)
	{
		SetCursor(hCursor_Pencil);
	}

	// Do not call CView::OnPaint() for painting messages
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	float* pos=pDoc->GetLightPos();
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LESS);		//设置深度测试函数
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//render objects(GL_RENDER)
	glLoadIdentity();
	gluLookAt(0,1,5,0,0,0,0,1,0);
	glPushMatrix();
	glTranslatef(g_fTransX,g_fTransY,g_fZoom);
	glMultMatrixf(Transform.M);//旋转


	if (mode==GL_RENDER)
	{
		glGetIntegerv(GL_VIEWPORT, viewport); 
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 
		glGetDoublev(GL_PROJECTION_MATRIX, projection); 
	}

	if (pDoc->GetViewStyle())
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);	//设置多边形显示模式为双面填充显示
	} 
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);	//设置多边形显示模式为双面线显示
	}

	glDisable(GL_LIGHTING);
	if ((pDoc->IsAxisOn())&&(mode==GL_RENDER))
	{
		glLineWidth(2.0);
		glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
		glEnd();
		glLineWidth(1.0);
		//glEnable(GL_LINE_STIPPLE);
		//glLineStipple(1,0x00FF);
		//glBegin(GL_LINES);
		//	glColor3f(1,0,0);
		//	glVertex3f(0,0,0);
		//	glVertex3f(-1,0,0);
		//	glColor3f(0,1,0);
		//	glVertex3f(0,0,0);
		//	glVertex3f(0,-1,0);
		//	glColor3f(0,0,1);
		//	glVertex3f(0,0,0);
		//	glVertex3f(0,0,-1);
		//glEnd();
		//glDisable(GL_LINE_STIPPLE);
	} 
	glEnable(GL_LIGHTING);


	//if (pDoc->IsPrimalMeshShown())
	if (pDoc->GetRenderPreMesh()==MESH_EXIST_VIEW && !pDoc->GetMesh().empty())
	{
		//the mesh is selected
		if (pDoc->GetRBSelName()==MODEL_NAME)
		{
			OBJHandle::DrawCGALPolyhedron(&(pDoc->GetMesh()),pDoc->GetViewStyle(),COLOR_SELECTED_OPAQUE,
				pDoc->GetDefaultColor(),mode);
		}
		else
		{
			OBJHandle::DrawCGALPolyhedron(&(pDoc->GetMesh()),pDoc->GetViewStyle(),pDoc->GetColorMode(),
				pDoc->GetDefaultColor(),mode);
		}
	}

//	RenderIcon(mode);

	if (mode==GL_RENDER)
	{
		this->RenderText.SetTextPos(this->viewport);
	}
	switch(pDoc->GetEditMode())
	{
	case CREATION_MODE:
		if (mode==GL_RENDER)
		{
			this->RenderText.glPrint("Creation Mode");
		}

		pDoc->GetMeshCreation().Render(pDoc->GetViewStyle(),mode,this->modelview,this->projection,this->viewport);

		break;
	case EDITING_MODE:
		if (mode==GL_RENDER)
		{
			this->RenderText.glPrint("Editing Mode");
		}

		pDoc->GetMeshEditing().Render(mode);

		break;
	case DEFORMATION_MODE:
		if (mode==GL_RENDER)
		{
			this->RenderText.glPrint("Deformation Mode");
		}
		pDoc->GetMeshDeformation().Render(pDoc->GetViewStyle(),mode,pDoc->IsDualMeshShown());
		break;
	case EXTRUSION_MODE:
		if (mode==GL_RENDER)
		{
			this->RenderText.glPrint("Extrusion Mode");
		}
		pDoc->GetMeshExtrusion().Render(pDoc->GetViewStyle(),mode);
		break;
	case CUTTING_MODE:
		this->RenderText.glPrint("Cutting Mode");
		pDoc->GetMeshCutting().Render(pDoc->GetViewStyle(),mode);
		break;
	case SMOOTHING_MODE:
		this->RenderText.glPrint("Smoothing Mode");
		pDoc->GetMeshSmoothing().Render(pDoc->GetViewStyle(),
			this->modelview,this->projection,this->viewport,mode);
		break;
	//case TEST_MODE:
	//	this->RenderText.glPrint("Test Mode");
	//	pDoc->GetTest().Render(pDoc->GetViewStyle(),
	//		this->modelview,this->projection,this->viewport);
	//	break;
	default:
		break;
	}


	if (pDoc->GetRenderPreMesh()==MESH_PREVIEW && !pDoc->GetMesh().empty())
	{
		//the mesh is selected
		if (pDoc->GetRBSelName()==MODEL_NAME)
		{
			OBJHandle::DrawCGALPolyhedron(&(pDoc->GetMesh()),pDoc->GetViewStyle(),COLOR_SELECTED_TRANSPARENT,
				pDoc->GetDefaultColor(),mode);
		}
		else
		{
			OBJHandle::DrawCGALPolyhedron(&(pDoc->GetMesh()),pDoc->GetViewStyle(),COLOR_TRANSPARENT,//pDoc->GetColorMode(),
				pDoc->GetDefaultColor(),mode);
		}
	}




	glPopMatrix();

	// Double buffer
	SwapBuffers(g_pBLDC->m_hDC);
	ValidateRect(NULL);
}

void CKWResearchWorkView::RenderIcon(GLenum mode)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//this is important! it makes windows coordinates and opengl coordinates consistent
	gluOrtho2D( 0, viewport[2],viewport[3], 0);	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLineWidth(3);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor3f(1,0,0);
	glBegin(GL_QUADS);
	glVertex2d(40,40);
	glVertex2d(40,80);
	glVertex2d(80,80);
	glVertex2d(80,40);
	glEnd();

	glLineWidth(1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void CKWResearchWorkView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CKWResearchWorkDoc* pDoc=GetDocument();
	switch(nIDEvent)
	{
	case TIMER_PLANE_WAIT_TO_ROTATE:
		DBWindowWrite("Waiting to rotate...\n");
		if (pDoc->GetEditMode()==CREATION_MODE)
		{
			pDoc->GetMeshCreation().WaitAutoPlaneRotate();
		}
		break;
	case TIMER_PLANE_ROTATE:
		DBWindowWrite("Rotating...\n");
		if (pDoc->GetEditMode()==CREATION_MODE)
		{
			pDoc->GetMeshCreation().RotateSelectedPlane();
		}
		break;
	default:
	    break;
	}
	RedrawWindow();
	CView::OnTimer(nIDEvent);
}
