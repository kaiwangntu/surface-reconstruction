#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"

class CControlPanel;

// CCPExtrusion dialog

class CCPExtrusion : public CDialog
{
	DECLARE_DYNAMIC(CCPExtrusion)

public:
	CCPExtrusion(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPExtrusion();

	CCPExtrusion(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);


// Dialog Data
	enum { IDD = IDD_CP_Extrusion };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	DECLARE_MESSAGE_MAP()

public:
	
	void Init();
	afx_msg void OnBnClickedExExtrude();
};
