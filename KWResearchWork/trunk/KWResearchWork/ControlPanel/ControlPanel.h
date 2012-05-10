#pragma once
#include "afxcmn.h"
#include "CPGeneral.h"
#include "CPCreation.h"
#include "CPEditing.h"
#include "CPDeformation.h"
#include "CPExtrusion.h"
#include "CPCutting.h"
#include "CPSmoothing.h"
#include "CPTest.h"
#include "../resource.h"
// CControlPanel form view

class CControlPanel : public CFormView
{
	DECLARE_DYNCREATE(CControlPanel)

protected:
	CControlPanel();           // protected constructor used by dynamic creation
	virtual ~CControlPanel();
public:
	enum { IDD = IDD_ControlPanel };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnInitialUpdate();


	CTabCtrl& GetCPTab(){return this->CP_TAB;}
	CCPGeneral* GetCPGeneral(){return this->pCPGeneral;}
	CCPCreation* GetCPCreation(){return this->pCPCreation;}
	CCPEditing* GetCPEditing(){return this->pCPEditing;}
	CCPDeformation* GetCPDeformation(){return this->pCPDeformation;}
	CCPExtrusion* GetCPExtrusion(){return this->pCPExtrusion;}
	CCPCutting* GetCPCutting(){return this->pCPCutting;}
	CCPSmoothing* GetCPSmoothing(){return this->pCPSmoothing;}
	CCPTest* GetCPTest(){return this->pCPTest;}

protected:
	
	// tab for all modes
	CTabCtrl CP_TAB;
	CCPGeneral* pCPGeneral;
	CCPCreation* pCPCreation;
	CCPEditing* pCPEditing;
	CCPDeformation* pCPDeformation;
	CCPExtrusion* pCPExtrusion;
	CCPCutting* pCPCutting;
	CCPSmoothing* pCPSmoothing;
	CCPTest* pCPTest;
public:
	afx_msg void OnTcnSelchangeCpTab(NMHDR *pNMHDR, LRESULT *pResult);
};


