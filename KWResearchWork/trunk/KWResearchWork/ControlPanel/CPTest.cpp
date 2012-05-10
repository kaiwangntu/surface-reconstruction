// CPTest.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPTest.h"


// CCPTest dialog

IMPLEMENT_DYNAMIC(CCPTest, CDialog)

CCPTest::CCPTest(CWnd* pParent /*=NULL*/)
	: CDialog(CCPTest::IDD, pParent)
{

}

CCPTest::~CCPTest()
{
}

CCPTest::CCPTest(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCPTest, CDialog)
	ON_BN_CLICKED(IDC_TE_Filter, &CCPTest::OnBnClickedTeFilter)
	ON_BN_CLICKED(IDC_TE_Resample, &CCPTest::OnBnClickedTeResample)
	ON_BN_CLICKED(IDC_TE_Smooth, &CCPTest::OnBnClickedTeSmooth)
END_MESSAGE_MAP()


// CCPTest message handlers
void CCPTest::Init()
{

}

void CCPTest::OnBnClickedTeFilter()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetEditMode()==TEST_MODE)
	{
		pDoc->GetTest().LineFilter();
	}
	pDoc->UpdateAllViews((CView*)pCP);
}

void CCPTest::OnBnClickedTeResample()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetEditMode()==TEST_MODE)
	{
		pDoc->GetTest().LineResample();
	}
	pDoc->UpdateAllViews((CView*)pCP);
}

void CCPTest::OnBnClickedTeSmooth()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetEditMode()==TEST_MODE)
	{
		pDoc->GetTest().LineSmooth();
	}
	pDoc->UpdateAllViews((CView*)pCP);
}
