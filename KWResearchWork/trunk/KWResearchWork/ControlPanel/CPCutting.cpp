// CPCutting.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPCutting.h"


// CCPCutting dialog

IMPLEMENT_DYNAMIC(CCPCutting, CDialog)

CCPCutting::CCPCutting(CWnd* pParent /*=NULL*/)
	: CDialog(CCPCutting::IDD, pParent)
{

}

CCPCutting::~CCPCutting()
{
}

CCPCutting::CCPCutting(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPCutting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCPCutting, CDialog)
END_MESSAGE_MAP()


// CCPCutting message handlers
void CCPCutting::Init()
{

}