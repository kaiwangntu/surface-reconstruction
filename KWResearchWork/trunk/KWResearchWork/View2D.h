#pragma once


// CView2D view

class CView2D : public CView
{
	DECLARE_DYNCREATE(CView2D)

public:
	CView2D();           // protected constructor used by dynamic creation
	virtual ~CView2D();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	int GetCurrentDesiredPointsPos(vector<HPCPoint> DesiredPointsPosition);//return num of DesiredPoints
	int SetModifiedPointsPos(vector<HPCPoint> & DesiredPointsPosition);//return num of DesiredPoints
	void IfAxisOn(bool bAxisOn);

protected:
	// OpenGL specific
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	BOOL InitOpenGL();
	HGLRC m_hGLContext;
	int m_GLPixelIndex;
	CDC * g_pBLDC;

protected:
	bool bAxis;//true for on,false for off

	HCURSOR hCursor_Rotate;
	double g_fSpinZ;
	int g_iLastPosZ;

	GLint    viewport[4]; 
	GLdouble modelview[16]; 
	GLdouble projection[16]; 
	void ScreenToGL(HPCPoint * point,Point3D * point3D,bool bInverse=false);//inverse means GLToScreen 

	vector<HPCPoint> PointsFrom3D;
	vector<HPCPoint> NormalizedPointsFrom3D;
	double dScale;
	double dCenterX,dCenterY;
	void Normalize3DPoints();//normalize PointsFrom3D to make the x,y of them between 0 & 1

	bool bStrokeDrawing;//true when user starts stroke,false  stop
	//	vector<HPCPoint> UserCurvePoints;
	vector<CPoint> UserCurvePoints;

	vector<HPCPoint> ModifiedPointsTo3D;
	bool FindModifiedPointsPosition();//find the Modified 2D position of the points

	vector<HPCPoint> RestoredModifiedPointsTo3D;
	bool Restore3DPoints();//Inverse transformation to Normalize3DPoints()

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnView2daxis();
};


