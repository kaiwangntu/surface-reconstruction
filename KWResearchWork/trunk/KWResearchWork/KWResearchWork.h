// KWResearchWork.h : main header file for the KWResearchWork application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKWResearchWorkApp:
// See KWResearchWork.cpp for the implementation of this class
//

class CKWResearchWorkApp : public CWinApp
{
public:
	CKWResearchWorkApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKWResearchWorkApp theApp;