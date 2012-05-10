#pragma once
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"
#include "afxcmn.h"
#include "../resource.h"
#include "../customcontrols/sliderctrlex.h"
#include "afxwin.h"

class CControlPanel;

// CCPDeformation dialog

class CCPDeformation : public CDialog
{
	DECLARE_DYNAMIC(CCPDeformation)

public:
	CCPDeformation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPDeformation();

	CCPDeformation(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);


// Dialog Data
	enum { IDD = IDD_CP_Deformation };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	void Init();


	CSliderCtrl DE_Slider_ROI;
	int DE_IterNum;
	double DE_Lambda;

	//material related
	CSliderCtrlEx DE_Slider_Material;
	CStatic DE_Static_Material_Value;
	//way to compute material.0: auto harmonic. 1: manual specify
	CComboBox DE_MatSetWay;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedDeClearroi();
	afx_msg void OnBnClickedDeIterLambda();
	afx_msg void OnBnClickedDeDeform();
	afx_msg void OnBnClickedDeComputedualmesh();
	afx_msg void OnBnClickedDeComputeedgemesh();
	afx_msg void OnBnClickedDeInterpolation();
	afx_msg void OnBnClickedDeSetmat();

	//custom slider related
	void ItemUpdate(LPARAM data1, int sValue, BOOL IsDragging);
	static void sItemUpdate(CCPDeformation *obj, LPARAM data1, int sValue,BOOL IsDragging);
};
