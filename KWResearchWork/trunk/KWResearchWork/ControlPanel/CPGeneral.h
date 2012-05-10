#pragma once
#include "afxcmn.h"
#include "../KWResearchWorkDoc.h"
#include "../KWResearchWorkView.h"
#include "../resource.h"

class CControlPanel;
//class CKWResearchWorkDoc;
// CCPGeneral dialog

class CCPGeneral : public CDialog
{
	DECLARE_DYNAMIC(CCPGeneral)

public:
	CCPGeneral(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCPGeneral();

// Dialog Data
	enum { IDD = IDD_CP_General };

	CCPGeneral(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CKWResearchWorkDoc * pDoc;
	CControlPanel * pCP;


	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:

	void Init();


	CSliderCtrl GE_Slider_LightPos_X;
	CSliderCtrl GE_Slider_LightPos_Y;
	CSliderCtrl GE_Slider_LightPos_Z;

	CSliderCtrl GE_Slider_Color_Red;
	CSliderCtrl GE_Slider_Color_Green;
	CSliderCtrl GE_Slider_Color_Blue;

	int iRadioViewStyle;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedGeWireframe();
	afx_msg void OnBnClickedGeSmooth();
	afx_msg void OnBnClickedGeHybrid();
	afx_msg void OnBnClickedGePoints();
	afx_msg void OnBnClickedSubdivision();
	afx_msg void OnBnClickedGeSnapshot();
	afx_msg void OnBnClickedExpScePara();
	afx_msg void OnBnClickedImpScePara();
};
