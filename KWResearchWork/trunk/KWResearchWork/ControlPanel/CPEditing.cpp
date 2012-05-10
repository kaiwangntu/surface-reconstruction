// CPEditing.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPEditing.h"


// CCPEditing dialog

IMPLEMENT_DYNAMIC(CCPEditing, CDialog)

CCPEditing::CCPEditing(CWnd* pParent /*=NULL*/)
	: CDialog(CCPEditing::IDD, pParent)
{

}

CCPEditing::~CCPEditing()
{
}

CCPEditing::CCPEditing(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPEditing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCPEditing, CDialog)
END_MESSAGE_MAP()


// CCPEditing message handlers
void CCPEditing::Init()
{

}
