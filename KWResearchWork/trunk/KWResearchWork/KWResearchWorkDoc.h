// KWResearchWorkDoc.h : interface of the CKWResearchWorkDoc class
//


#pragma once

#include "OBJHandle.h"
#include "MeshCreation/MeshCreation.h"
#include "PaintingOnMesh.h"
#include "MeshEditing.h"
#include "MeshDeformation/MeshDeformation.h"
#include "MeshExtrusion/MeshExtrusion.h"
#include "MeshCutting/MeshCutting.h"
#include "MeshSmoothing/MeshSmoothing.h"
#include "Test.h"
#include "CurveDeform.h"


class CMainFrame;

class CKWResearchWorkDoc : public CDocument
{
protected: // create from serialization only
	CKWResearchWorkDoc();
	DECLARE_DYNCREATE(CKWResearchWorkDoc)

// Attributes
public:
// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

// Implementation
public:
	virtual ~CKWResearchWorkDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
	//manipulation mode:0 view,1 select,2 sketch
	int iManipMode;

	//name of the right button selected object, bigger near Z value&number has higher priority
	int iRBSelName;
	//name of the left button selected object, bigger near Z value&number has higher priority
	int iLBSelName;


	int iEditMode;
	int iColorMode;
	float LightPos[4];
	vector<double> vecDefaultColor;

	KW_Mesh Mesh;

	//render
	int	iViewStyle;//0 wireframe, 1 smooth, 2 hybrid, 3 points
	bool bAxis;//true for on,false for off
	bool bShowDualMesh;//show dual mesh
	bool bShowPrimalMesh;//show primal mesh

	//how to render the previewed mesh
	int iRenderPreMesh;

	//total editing class
	//used to judge which editing operation to perform according to type of input sketch
	CMeshEditing MeshEditing;

	//deformation
	CMeshDeformation MeshDeformation;
	//extrusion
	CMeshExtrusion MeshExtrusion;
	vector<vector<Point_3> > testvecvecNewEdgeVertexPos;
	//cutting
	CMeshCutting MeshCutting;
	//smoothing
	CMeshSmoothing MeshSmoothing;
	//test
	CTest Test;
	vector<Point_3> testCentroidPoint;
	vector<Point_3> testmovedCentroidPoint;
	vector<Facet_handle> testfhRefineTri;
	//creation 
	CMeshCreation MeshCreation;

	vector<Point_3> testpoints;

protected:
	//judge on which plane does the edit take,the 3D plane has priority
	//true for 3D plane,false for 2D
	bool JudgeEditPlane();

	//provide for 2D Edit
	int GetCurrentDesiredPointsPos(vector<HPCPoint> & DesiredPointsPosition);//return num of DesiredPoints

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	//get the point to view
	CView* GetView(CRuntimeClass* pClass);

	int GetViewStyle(){return this->iViewStyle;}
	void SetViewStyle(int iInput){this->iViewStyle=iInput;}

	vector<double> GetDefaultColor() {return this->vecDefaultColor;}
	void SetDefaultColor(vector<double> DataIn){this->vecDefaultColor=DataIn;}


	bool IsAxisOn(){return this->bAxis;}
	void RenderAxis(bool bRender) {this->bAxis=bRender;}//true: Render, false: not
	bool IsDualMeshShown(){return this->bShowDualMesh;}
	bool IsPrimalMeshShown(){return this->bShowPrimalMesh;}

	void SetRenderPreMesh(int iInput) {this->iRenderPreMesh=iInput;}
	int GetRenderPreMesh() {return this->iRenderPreMesh;}

	void SetManipMode(int iInput) {this->iManipMode=iInput;}
	int GetManipMode() {return iManipMode;}

	void SetRBSelName(int iInput) {this->iRBSelName=iInput;}
	int GetRBSelName() {return this->iRBSelName;}

	void SetLBSelName(int iInput) {this->iLBSelName=iInput;}
	int GetLBSelName() {return this->iLBSelName;}

	int GetEditMode(){return iEditMode;}

	int GetColorMode(){return iColorMode;}
	void SetColorMode(int iDataIn) {this->iColorMode=iDataIn;}

	float* GetLightPos(){return this->LightPos;}
	void SetLightPos(float* DataIn){memcpy(this->LightPos,DataIn,sizeof(float)*4);}

	KW_Mesh& GetMesh() {return this->Mesh;}
	CMeshEditing& GetMeshEditing() {return this->MeshEditing;}
	CMeshDeformation& GetMeshDeformation() {return this->MeshDeformation;}
	CMeshExtrusion& GetMeshExtrusion() {return this->MeshExtrusion;}
	vector<vector<Point_3> >& GettestvecvecNewEdgeVertexPos();
	CMeshCutting& GetMeshCutting() {return this->MeshCutting;}
	CMeshSmoothing& GetMeshSmoothing() {return this->MeshSmoothing;}
	CMeshCreation& GetMeshCreation() {return this->MeshCreation;}
	CTest& GetTest() {return this->Test;}


	vector<Point_3>& GetTestPointsRef() {return this->testpoints;};
	void SetTestPoints(vector<Point_3> DataIn) {this->testpoints=DataIn;};

	afx_msg void OnCurvatureMeancurvature();
	afx_msg void OnCurvatureGaussiancurvature();
//	afx_msg void OnOperation2dto3d();
//	afx_msg void OnOperation3dto2d();
	afx_msg void OnModeCreation();
	afx_msg void OnUpdateModeCreation(CCmdUI *pCmdUI);
	afx_msg void OnModeDeformation();
	afx_msg void OnUpdateModeDeformation(CCmdUI *pCmdUI);
	afx_msg void OnModeExtrusion();
	afx_msg void OnUpdateModeExtrusion(CCmdUI *pCmdUI);
	afx_msg void OnModeCutting();
	afx_msg void OnUpdateModeCutting(CCmdUI *pCmdUI);
	afx_msg void OnModeSmoothing();
	afx_msg void OnUpdateModeSmoothing(CCmdUI *pCmdUI);
	afx_msg void OnCurvatureNone();
	afx_msg void OnUpdateCurvatureNone(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCurvatureMeancurvature(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCurvatureGaussiancurvature(CCmdUI *pCmdUI);
	afx_msg void OnViewDualmesh();
	afx_msg void OnOperationDeformationComputedualmesh();
	afx_msg void OnOperationDeformationComputeEdgemesh();
	afx_msg void OnViewPrimalmesh();

	afx_msg void OnViewSelectMode();
	afx_msg void OnSketchMode();

	afx_msg void OnHelpTest();
	afx_msg void OnModeTest();
	afx_msg void OnUpdateModeTest(CCmdUI *pCmdUI);
	afx_msg void OnModeEditing();
	afx_msg void OnUpdateModeEditing(CCmdUI *pCmdUI);
};


