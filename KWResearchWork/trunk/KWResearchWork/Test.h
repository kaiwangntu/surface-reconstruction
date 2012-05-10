#pragma once

class CKWResearchWorkDoc;

class CTest
{
public:
	CTest(void);
	~CTest(void);

	void Init(CKWResearchWorkDoc* pDataIn);
	void Render(bool bSmoothView,GLdouble* modelview,GLdouble* projection,GLint* viewport);
	void InputCurvePoint2D(CPoint ProfilePoint);
	void Conver2DCurveTo3D(GLdouble* modelview,GLdouble* projection,GLint* viewport);

	void LineFilter();
	void LineResample();
	void LineSmooth();



protected:
	CKWResearchWorkDoc* pDoc;

	vector<CPoint> UserInput2DProfile;
	void Render2DProfile(GLdouble* modelview,GLdouble* projection,GLint* viewport);


};
