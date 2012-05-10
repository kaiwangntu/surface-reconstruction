#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"
#include "afxwin.h"

class CControlPanel;
// CCPCreation dialog

class CCPCreation : public CDialog
{
	DECLARE_DYNAMIC(CCPCreation)

public:
	CCPCreation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPCreation();

	CCPCreation(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);


// Dialog Data
	enum { IDD = IDD_CP_Creation };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;

	int iRadioModAlgo;

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	void Init();
	afx_msg void OnBnClickedCrReadcontour();
	afx_msg void OnBnClickedCrWritecontour();
	afx_msg void OnBnClickedCrAdjustcontourview();
	afx_msg void OnBnClickedCrGeneratemesh();
	afx_msg void OnBnClickedModAlgoTj();
	afx_msg void OnBnClickedModAlgoProg();
	//which cross section cylinder to show
	CComboBox CR_SINGLEPOLY;
	afx_msg void OnCbnDropdownCrComboSinglepoly();
	afx_msg void OnCbnSelendokCrComboSinglepoly();
	//which subspace to show
	CComboBox CR_SINGLESS;
	afx_msg void OnCbnDropdownCrComboSingless();
	afx_msg void OnCbnSelendokCrComboSingless();
	CComboBox CR_UNIONPOLY;
	afx_msg void OnCbnDropdownCrComboUnionpoly();
	afx_msg void OnCbnSelendokCrComboUnionpoly();
};
