#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"

class CControlPanel;

// CCPCutting dialog

class CCPCutting : public CDialog
{
	DECLARE_DYNAMIC(CCPCutting)

public:
	CCPCutting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPCutting();

	CCPCutting(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);


// Dialog Data
	enum { IDD = IDD_CP_Cutting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	DECLARE_MESSAGE_MAP()

public:

	void Init();
};
