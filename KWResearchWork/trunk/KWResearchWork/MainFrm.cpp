// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "KWResearchWork.h"

#include "KWResearchWorkDoc.h"
#include "KWResearchWorkView.h"
#include "ControlPanel/ControlPanel.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_EXIT, &CMainFrame::OnFileExit)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))//IDR_MAINFRAME//IDR_TOOLBAR
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CreateToolBar();

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	//set the view->smooth checked
	CMenu*	mainmenu=this->GetMenu();
//	mainmenu->CheckMenuItem(ID_VIEW_3DAXISON,MF_CHECKED);	
//	mainmenu->CheckMenuItem(ID_VIEW_2DAXIS,MF_CHECKED);	
	mainmenu->CheckMenuItem(ID_VIEW_BFPLANE,MF_CHECKED);	
	mainmenu->CheckMenuItem(ID_VIEW_PRIMALMESH,MF_CHECKED);	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	m_wndSplitter.CreateView(0, 0, 
							RUNTIME_CLASS(CKWResearchWorkView), 
							CSize(100, 100), pContext);
	m_wndSplitter.CreateView(0, 1, 
							RUNTIME_CLASS(CControlPanel), 
							CSize(100, 100), pContext);

	CRect rect;
	GetWindowRect(&rect);
	m_wndSplitter.SetColumnInfo(0, rect.Width() *4/ 5 ,0);
	m_wndSplitter.SetColumnInfo(1, rect.Width() *1/ 5 ,0);
//	m_wndSplitter.SetColumnInfo(0, rect.Width() *20/ 20 ,0);
//	m_wndSplitter.SetColumnInfo(1, rect.Width() *0/ 20 ,0);
	m_wndSplitter.SetRowInfo(0, rect.Height(),0);
	m_wndSplitter.RecalcLayout();

	return TRUE;
//	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnFileExit()
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE,0,0);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	CView* pView=this->GetActiveView();
	if (pView!=NULL)
	{
		CKWResearchWorkDoc* pDoc=(CKWResearchWorkDoc*)pView->GetDocument();
		pDoc->UpdateAllViews(NULL);
	}
}

void CMainFrame::CreateToolBar(void)
{
	while(m_wndToolBar.GetToolBarCtrl().DeleteButton(0));

	int iToolBarSize=16;
	int iNumIcons=5;

	m_pImageList.Create(iToolBarSize, iToolBarSize, TRUE | ILC_COLOR32, iNumIcons, 0);
	HICON hIcon = NULL;

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_NewFile), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
	m_pImageList.Add(hIcon);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OpenFile), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
	m_pImageList.Add(hIcon);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_SaveFile), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
	m_pImageList.Add(hIcon);

//	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
//		MAKEINTRESOURCE(IDI_ViewMode), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
//	m_pImageList.Add(hIcon);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_SelMode), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
	m_pImageList.Add(hIcon);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_SketchMode), IMAGE_ICON, iToolBarSize, iToolBarSize, 0);
	m_pImageList.Add(hIcon);

	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_pImageList);


	TBBUTTON   tb;   
	tb.iBitmap=0;   
	tb.iString=NULL;   
	tb.fsState=TBSTATE_ENABLED;   
	tb.fsStyle=TBSTYLE_BUTTON;   
	tb.idCommand=ID_FILE_NEW;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);   


	tb.iBitmap=1;   
	tb.idCommand=ID_FILE_OPEN;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);  

	tb.iBitmap=2;   
	tb.idCommand=ID_FILE_SAVE;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);   

	TBBUTTON   tbsep;   
	tbsep.iBitmap   =   0;   
	tbsep.fsState   =   TBSTATE_ENABLED;   
	tbsep.fsStyle   =   TBSTYLE_SEP;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tbsep);   

	//tb.iBitmap=3;   
	//tb.idCommand=ID_VIEW_MODE;//ID_FILE_PRINT;//;   
	//tb.iString=NULL;   
	//m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);   

	tb.iBitmap=3;   
	tb.idCommand=ID_VIEW_SELECT_MODE;//ID_FILE_PRINT_PREVIEW;//;   
	tb.iString=NULL;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);   

	tb.iBitmap=4;   
	tb.idCommand=ID_SKETCH_MODE;//ID_FILE_PRINT_SETUP;//;   
	tb.iString=NULL;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tb);   

	tbsep.iBitmap   =   0;   
	tbsep.fsState   =   TBSTATE_ENABLED;   
	tbsep.fsStyle   =   TBSTYLE_SEP;   
	m_wndToolBar.GetToolBarCtrl().AddButtons(1,&tbsep);   

	m_wndToolBar.GetToolBarCtrl().AutoSize();

	//CToolTipCtrl*  aa  =  m_ToolBar.GetToolTips();  
	//aa->UpdateTipText("New File",&m_ToolBar,ID_FILE_NEW);
	//aa->UpdateTipText("Open File",&m_wndToolBar,ID_FILE_OPEN);
	//aa->UpdateTipText("Save File",&m_wndToolBar,ID_FILE_SAVE);
	//aa->UpdateTipText("3D to 2D",&m_wndToolBar,ID_OPERATION_3DTO2D);
	//aa->UpdateTipText("2D to 3D",&m_wndToolBar,ID_OPERATION_2DTO3D);
}

