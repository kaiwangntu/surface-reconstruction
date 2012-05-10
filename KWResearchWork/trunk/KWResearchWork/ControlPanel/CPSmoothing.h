#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"

class CControlPanel;

// CCPSmoothing dialog

class CCPSmoothing : public CDialog
{
	DECLARE_DYNAMIC(CCPSmoothing)

public:
	CCPSmoothing(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPSmoothing();

	CCPSmoothing(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);


// Dialog Data
	enum { IDD = IDD_CP_Smoothing };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	DECLARE_MESSAGE_MAP()

public:
	void Init();

	afx_msg void OnBnClickedSmBilateralsmooth();
	afx_msg void OnBnClickedSmLaplaciansmooth();
	afx_msg void OnBnClickedSmTaubinlambdamusmooth();
};
