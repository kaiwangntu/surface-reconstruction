// ControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "ControlPanel.h"
#include "../KWResearchWorkDoc.h"

// CControlPanel

IMPLEMENT_DYNCREATE(CControlPanel, CFormView)

CControlPanel::CControlPanel()
	: CFormView(CControlPanel::IDD)
{
}

CControlPanel::~CControlPanel()
{
}

void CControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CP_TAB, CP_TAB);
}

BEGIN_MESSAGE_MAP(CControlPanel, CFormView)
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_CP_TAB, &CControlPanel::OnTcnSelchangeCpTab)
END_MESSAGE_MAP()


// CControlPanel diagnostics

#ifdef _DEBUG
void CControlPanel::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlPanel::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlPanel message handlers

int CControlPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	pCPGeneral=NULL;
	pCPCreation=NULL;
	pCPEditing=NULL;
	pCPDeformation=NULL;
	pCPExtrusion=NULL;
	pCPCutting=NULL;
	pCPSmoothing=NULL;
	pCPTest=NULL;

	return 0;
}

void CControlPanel::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CKWResearchWorkDoc* pDoc=(CKWResearchWorkDoc*)GetDocument();

	//dialog initial
	if (CP_TAB.GetItemCount()==0)
	{
		TCITEM tcItem;
		tcItem.mask = TCIF_TEXT;
		tcItem.pszText = _T("General");
		CP_TAB.InsertItem(0,&tcItem);
		tcItem.pszText = _T("Creation");
		CP_TAB.InsertItem(1,&tcItem);
		tcItem.pszText = _T("Editing");
		CP_TAB.InsertItem(2,&tcItem);
		tcItem.pszText = _T("Deformation");
		CP_TAB.InsertItem(3,&tcItem);
		tcItem.pszText = _T("Extrusion");
		CP_TAB.InsertItem(4,&tcItem);
		tcItem.pszText = _T("Cutting");
		CP_TAB.InsertItem(5,&tcItem);
		tcItem.pszText = _T("Smoothing");
		CP_TAB.InsertItem(6,&tcItem);
		tcItem.pszText = _T("Test");
		CP_TAB.InsertItem(7,&tcItem);

		pCPGeneral=new CCPGeneral(pDoc,this);
		pCPGeneral->Create(IDD_CP_General,GetDlgItem(IDC_CP_TAB)); 

		pCPCreation=new CCPCreation(pDoc,this);
		pCPCreation->Create(IDD_CP_Creation,GetDlgItem(IDC_CP_TAB)); 

		pCPEditing=new CCPEditing(pDoc,this);
		pCPEditing->Create(IDD_CP_Editing,GetDlgItem(IDC_CP_TAB)); 

		pCPDeformation=new CCPDeformation(pDoc,this);
		pCPDeformation->Create(IDD_CP_Deformation,GetDlgItem(IDC_CP_TAB)); 

		pCPExtrusion=new CCPExtrusion(pDoc,this);
		pCPExtrusion->Create(IDD_CP_Extrusion,GetDlgItem(IDC_CP_TAB)); 

		pCPCutting=new CCPCutting(pDoc,this);
		pCPCutting->Create(IDD_CP_Cutting,GetDlgItem(IDC_CP_TAB)); 

		pCPSmoothing=new CCPSmoothing(pDoc,this);
		pCPSmoothing->Create(IDD_CP_Smoothing,GetDlgItem(IDC_CP_TAB)); 

		pCPTest=new CCPTest(pDoc,this);
		pCPTest->Create(IDD_CP_Test,GetDlgItem(IDC_CP_TAB)); 

		CRect rs; 
		CP_TAB.GetClientRect(&rs); 

		pCPGeneral->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPCreation->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPEditing->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPDeformation->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPExtrusion->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPCutting->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPSmoothing->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);
		pCPTest->MoveWindow(rs.left,rs.top+40,rs.right-rs.left,rs.bottom-rs.top-40);

		CP_TAB.SetCurSel(0);  
		pCPGeneral->ShowWindow(true); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false); 
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
	}

	pCPGeneral->Init();
	pCPCreation->Init();
	pCPEditing->Init();
	pCPDeformation->Init();
	pCPExtrusion->Init();
	pCPCutting->Init();
	pCPSmoothing->Init();
	pCPTest->Init();

}

void CControlPanel::OnTcnSelchangeCpTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CKWResearchWorkDoc* pDoc=(CKWResearchWorkDoc*)GetDocument();
	int CurSel = CP_TAB.GetCurSel(); 
	switch(CurSel) 
	{ 
	case 0: 
		pCPGeneral->ShowWindow(true); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
		break; 
	case 1: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(true); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
		if (pDoc!=NULL)
		{
			pDoc->OnModeCreation();
		}
		break; 
	case 2: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(true);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
		if (pDoc!=NULL)
		{
			pDoc->OnModeEditing();
		}
		break; 
	case 3: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(true); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
		if (pDoc!=NULL)
		{
			pDoc->OnModeDeformation();
		}
		break; 
	case 4: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(true); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false); 
		if (pDoc!=NULL)
		{
			pDoc->OnModeExtrusion();
		}
		break; 
	case 5: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(true); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(false);
		if (pDoc!=NULL)
		{
			pDoc->OnModeCutting();
		}
		break; 
	case 6: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(true); 
		pCPTest->ShowWindow(false);
		if (pDoc!=NULL)
		{
			pDoc->OnModeSmoothing();
		}
		break; 
	case 7: 
		pCPGeneral->ShowWindow(false); 
		pCPCreation->ShowWindow(false); 
		pCPEditing->ShowWindow(false);
		pCPDeformation->ShowWindow(false); 
		pCPExtrusion->ShowWindow(false); 
		pCPCutting->ShowWindow(false); 
		pCPSmoothing->ShowWindow(false); 
		pCPTest->ShowWindow(true); 
		if (pDoc!=NULL)
		{
			pDoc->OnModeTest();
		}
		break; 
	default: 
		break; 
	}  


	*pResult = 0;
}
