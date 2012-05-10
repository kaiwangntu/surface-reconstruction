#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"

class CControlPanel;


// CCPEditing dialog

class CCPEditing : public CDialog
{
	DECLARE_DYNAMIC(CCPEditing)

public:
	CCPEditing(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPEditing();

	CCPEditing(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);

// Dialog Data
	enum { IDD = IDD_CP_Editing };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;

	DECLARE_MESSAGE_MAP()

public:

	void Init();

};
