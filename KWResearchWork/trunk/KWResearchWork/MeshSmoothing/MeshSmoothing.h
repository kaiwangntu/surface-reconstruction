#pragma once
#include "../OBJHandle.h"
#include "../PaintingOnMesh.h"

class CKWResearchWorkDoc;

class CMeshSmoothing
{
public:
	CMeshSmoothing(void);
	~CMeshSmoothing(void);

	void Init(CKWResearchWorkDoc* pDataIn);

	void InputCurvePoint2D(CPoint Point2D);

	//find roi by painting strokes
	void PaintROIVertices(KW_Mesh& Mesh,GLdouble* modelview,GLdouble* projection,GLint* viewport);

	void ClearROI();

	void Render(bool bSmoothView,GLdouble* modelview,GLdouble* projection,GLint* viewport,GLenum mode);

	void BilateralSmooth(KW_Mesh& Mesh);

private:
	CKWResearchWorkDoc* pDoc;

	vector<CPoint> CurvePoint2D;//user drawn 2D curve(screen coordinate)

	vector<Vertex_handle> ROIVertices;

	void RenderCurvePoint2D(GLdouble* modelview,GLdouble* projection,GLint* viewport);
	void RenderROI();
};

