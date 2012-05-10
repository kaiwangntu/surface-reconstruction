// CPExtrusion.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPExtrusion.h"


// CCPExtrusion dialog

IMPLEMENT_DYNAMIC(CCPExtrusion, CDialog)

CCPExtrusion::CCPExtrusion(CWnd* pParent /*=NULL*/)
	: CDialog(CCPExtrusion::IDD, pParent)
{

}

CCPExtrusion::~CCPExtrusion()
{
}

CCPExtrusion::CCPExtrusion(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPExtrusion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCPExtrusion, CDialog)
	ON_BN_CLICKED(IDC_EX_Extrude, &CCPExtrusion::OnBnClickedExExtrude)
END_MESSAGE_MAP()


// CCPExtrusion message handlers
void CCPExtrusion::Init()
{

}
void CCPExtrusion::OnBnClickedExExtrude()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_EX_Extrude);
	pButton->EnableWindow(FALSE);
	BeginWaitCursor();

	if (!pDoc->GetMeshExtrusion().ExtrudeMesh(pDoc->GetMesh(),pDoc->GettestvecvecNewEdgeVertexPos()))
	{
		return;
	}
	pDoc->GettestvecvecNewEdgeVertexPos().clear();

	CString  strTitle=pDoc->GetTitle();
	strTitle=strTitle+"*";
	pDoc->SetTitle(strTitle);
	pDoc->SetModifiedFlag(TRUE);

	EndWaitCursor();
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}
