// CPSmoothing.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPSmoothing.h"


// CCPSmoothing dialog

IMPLEMENT_DYNAMIC(CCPSmoothing, CDialog)

CCPSmoothing::CCPSmoothing(CWnd* pParent /*=NULL*/)
	: CDialog(CCPSmoothing::IDD, pParent)
{

}

CCPSmoothing::~CCPSmoothing()
{
}

CCPSmoothing::CCPSmoothing(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPSmoothing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCPSmoothing, CDialog)
	ON_BN_CLICKED(IDC_SM_BilateralSmooth, &CCPSmoothing::OnBnClickedSmBilateralsmooth)
	ON_BN_CLICKED(IDC_SM_LaplacianSmooth, &CCPSmoothing::OnBnClickedSmLaplaciansmooth)
	ON_BN_CLICKED(IDC_SM_TaubinLambdaMuSmooth, &CCPSmoothing::OnBnClickedSmTaubinlambdamusmooth)
END_MESSAGE_MAP()


// CCPSmoothing message handlers
void CCPSmoothing::Init()
{

}

void CCPSmoothing::OnBnClickedSmBilateralsmooth()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_SM_LaplacianSmooth);
	pButton->EnableWindow(FALSE);

	pDoc->GetMeshSmoothing().BilateralSmooth(pDoc->GetMesh());

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPSmoothing::OnBnClickedSmLaplaciansmooth()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_SM_LaplacianSmooth);
	pButton->EnableWindow(FALSE);
	BeginWaitCursor();

	if (!pDoc->GetMesh().empty())
	{
		GeometryAlgorithm::LaplacianSmooth(10,0.3,pDoc->GetMesh());
	}

	EndWaitCursor();
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPSmoothing::OnBnClickedSmTaubinlambdamusmooth()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_SM_TaubinLambdaMuSmooth);
	pButton->EnableWindow(FALSE);
	BeginWaitCursor();

	if (!pDoc->GetMesh().empty())
	{
		GeometryAlgorithm::TaubinLambdaMuSmooth(10,0.5,-0.53,pDoc->GetMesh());
	}

	EndWaitCursor();
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}
