#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"

class CControlPanel;

// CCPTest dialog

class CCPTest : public CDialog
{
	DECLARE_DYNAMIC(CCPTest)

public:
	CCPTest(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPTest();

	CCPTest(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);

// Dialog Data
	enum { IDD = IDD_CP_Test };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	DECLARE_MESSAGE_MAP()

public:
	void Init();
	afx_msg void OnBnClickedTeFilter();
	afx_msg void OnBnClickedTeResample();
	afx_msg void OnBnClickedTeSmooth();
};
