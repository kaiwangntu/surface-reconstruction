// KWResearchWorkView.h : interface of the CKWResearchWorkView class
//


#pragma once

#include "ArcBall.h"
#include "RenderText.h"
#include "KWResearchWorkDoc.h"

class CMainFrame;

class CKWResearchWorkView : public CView
{
protected: // create from serialization only
//	CKWResearchWorkView();
	DECLARE_DYNCREATE(CKWResearchWorkView)

// Attributes
public:
	CKWResearchWorkDoc* GetDocument() const;

// Operations
public:
	CKWResearchWorkView();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CKWResearchWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	//if initilize of the view or not
	void Reset(bool bInitial);

	void saveSceneImage(const char* filename);//snapshot of current view

protected:
	CRenderText RenderText;

	//Arcball
	ArcBallT ArcBall;
	Matrix4fT Transform;
	Matrix3fT LastRot;
	Matrix3fT ThisRot;

	float g_fZoom,g_fTransX,g_fTransY;

	HCURSOR hCursor_Rotate;
	HCURSOR hCursor_Move;
	HCURSOR hCursor_Zoom;
	HCURSOR hCursor_PaintROI;
	HCURSOR hCursor_Smooth;
	HCURSOR hCursor_Pencil;

	GLint    viewport[4]; 
	GLdouble modelview[16]; 
	GLdouble projection[16]; 

	int g_iLastPosX,g_iLastPosY;
	void ScreenToGL(HPCPoint * point,Point3D * point3D,bool bInverse=false);//inverse means GLToScreen 

	//process the hits of left/right button in selection mode
	void ProcessMouseHit(CPoint point,int iButton);
	//get the name of the selected object(with smallest near z value or biggest number)
	int GetSelectName(GLint hits, GLuint buffer[]);

	//total render function
	void Render(GLenum mode);

	//render icons
	void RenderIcon(GLenum mode);

protected:
	// OpenGL specific
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	BOOL InitOpenGL();
	HGLRC m_hGLContext;
	int m_GLPixelIndex;
	CDC * g_pBLDC;



// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:

	GLfloat* GetArcballTrans() {return this->Transform.M;}
	void SetArcballTrans (GLfloat* Input) {memcpy_s(this->Transform.M,sizeof(this->Transform.M),Input,sizeof(GLfloat)*16);}

	void GetTranslatePara(float& OutPutX,float& OutPutY,float& OutPutZ) {OutPutX=g_fTransX;OutPutY=g_fTransY;OutPutZ=g_fZoom;}
	void SetTranslatePara(float InPutX,float InPutY,float InPutZ){g_fTransX=InPutX;g_fTransY=InPutY;g_fZoom=InPutZ;}

	GLint* GetViewport() {return this->viewport;}
	void SetViewport(GLint* Input) {memcpy_s(this->viewport,sizeof(this->viewport),Input,sizeof(GLint)*4);}

	GLdouble* GetModelview() {return this->modelview;}
	void SetModelview(GLdouble* Input) {memcpy_s(this->modelview,sizeof(this->modelview),Input,sizeof(GLdouble)*16);}
	
	GLdouble* GetProjection() {return this->projection;}
	void SetProjection(GLdouble* Input) {memcpy_s(this->projection,sizeof(this->projection),Input,sizeof(GLdouble)*16);}

	Matrix3fT& GetViewRotMat(){return this->ThisRot;}
	Matrix4fT& GetViewTransMat(){return this->Transform;}

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewSmoothshade();
	afx_msg void OnViewWireframe();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // debug version in KWResearchWorkView.cpp
inline CKWResearchWorkDoc* CKWResearchWorkView::GetDocument() const
   { return reinterpret_cast<CKWResearchWorkDoc*>(m_pDocument); }
#endif

