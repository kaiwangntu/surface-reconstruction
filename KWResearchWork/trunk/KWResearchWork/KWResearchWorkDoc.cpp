// KWResearchWorkDoc.cpp : implementation of the CKWResearchWorkDoc class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "KWResearchWork.h"

#include "KWResearchWorkDoc.h"
#include "KWResearchWorkView.h"
#include "ControlPanel/ControlPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKWResearchWorkDoc

IMPLEMENT_DYNCREATE(CKWResearchWorkDoc, CDocument)

BEGIN_MESSAGE_MAP(CKWResearchWorkDoc, CDocument)
//	ON_COMMAND(ID_OPERATION_2DTO3D, &CKWResearchWorkDoc::OnOperation2dto3d)
//	ON_COMMAND(ID_OPERATION_3DTO2D, &CKWResearchWorkDoc::OnOperation3dto2d)
	ON_COMMAND(ID_CURVATURE_MEANCURVATURE, &CKWResearchWorkDoc::OnCurvatureMeancurvature)
	ON_COMMAND(ID_MODE_CREATION, &CKWResearchWorkDoc::OnModeCreation)
	ON_UPDATE_COMMAND_UI(ID_MODE_CREATION, &CKWResearchWorkDoc::OnUpdateModeCreation)
	ON_COMMAND(ID_MODE_DEFORMATION, &CKWResearchWorkDoc::OnModeDeformation)
	ON_UPDATE_COMMAND_UI(ID_MODE_DEFORMATION, &CKWResearchWorkDoc::OnUpdateModeDeformation)
	ON_COMMAND(ID_MODE_EXTRUSION, &CKWResearchWorkDoc::OnModeExtrusion)
	ON_UPDATE_COMMAND_UI(ID_MODE_EXTRUSION, &CKWResearchWorkDoc::OnUpdateModeExtrusion)
	ON_COMMAND(ID_MODE_CUTTING, &CKWResearchWorkDoc::OnModeCutting)
	ON_UPDATE_COMMAND_UI(ID_MODE_CUTTING, &CKWResearchWorkDoc::OnUpdateModeCutting)
	ON_COMMAND(ID_CURVATURE_NONE, &CKWResearchWorkDoc::OnCurvatureNone)
	ON_UPDATE_COMMAND_UI(ID_CURVATURE_NONE, &CKWResearchWorkDoc::OnUpdateCurvatureNone)
	ON_UPDATE_COMMAND_UI(ID_CURVATURE_MEANCURVATURE, &CKWResearchWorkDoc::OnUpdateCurvatureMeancurvature)
	ON_COMMAND(ID_MODE_SMOOTHING, &CKWResearchWorkDoc::OnModeSmoothing)
	ON_UPDATE_COMMAND_UI(ID_MODE_SMOOTHING, &CKWResearchWorkDoc::OnUpdateModeSmoothing)
	ON_COMMAND(ID_CURVATURE_GAUSSIANCURVATURE, &CKWResearchWorkDoc::OnCurvatureGaussiancurvature)
	ON_UPDATE_COMMAND_UI(ID_CURVATURE_GAUSSIANCURVATURE, &CKWResearchWorkDoc::OnUpdateCurvatureGaussiancurvature)
	ON_COMMAND(ID_VIEW_DUALMESH, &CKWResearchWorkDoc::OnViewDualmesh)
	ON_COMMAND(ID_OPERATION_DEFORMATION_COMPUTEDUALMESH, &CKWResearchWorkDoc::OnOperationDeformationComputedualmesh)
	ON_COMMAND(ID_VIEW_PRIMALMESH, &CKWResearchWorkDoc::OnViewPrimalmesh)
	ON_COMMAND(ID_VIEW_SELECT_MODE, &CKWResearchWorkDoc::OnViewSelectMode)
	ON_COMMAND(ID_SKETCH_MODE, &CKWResearchWorkDoc::OnSketchMode)
	ON_COMMAND(ID_HELP_TEST, &CKWResearchWorkDoc::OnHelpTest)
	ON_COMMAND(ID_MODE_TEST, &CKWResearchWorkDoc::OnModeTest)
	ON_UPDATE_COMMAND_UI(ID_MODE_TEST, &CKWResearchWorkDoc::OnUpdateModeTest)
	ON_COMMAND(ID_MODE_EDITING, &CKWResearchWorkDoc::OnModeEditing)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDITING, &CKWResearchWorkDoc::OnUpdateModeEditing)
END_MESSAGE_MAP()


// CKWResearchWorkDoc construction/destruction

CKWResearchWorkDoc::CKWResearchWorkDoc()
{
	// TODO: add one-time construction code here
//	this->iEditMode=CREATION_MODE;
//	this->MeshCreation.Init(this);
//	this->iColorMode=COLOR_ORIGINAL;

//	float pos[4] = { 0, 0, 1, 0};
//	SetLightPos(pos);

	this->bShowDualMesh=false;
	this->bShowPrimalMesh=true;
}

CKWResearchWorkDoc::~CKWResearchWorkDoc()
{
}

BOOL CKWResearchWorkDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	CKWResearchWorkView* pView=(CKWResearchWorkView*)GetView(RUNTIME_CLASS(CKWResearchWorkView));
	if (pMain==NULL)
	{
		pView->Reset(true);
	}
	else
	{
		pView->Reset(false);
		CMenu* pMenu=pMain->GetMenu();
		pMenu->CheckMenuItem(ID_VIEW_3DAXISON,MF_UNCHECKED);	
		this->bAxis=false;
		this->bShowPrimalMesh=true;
		pMenu->CheckMenuItem(ID_VIEW_PRIMALMESH,MF_CHECKED);	
//		this->iViewStyle=1;
	}

	Mesh.clear();
	this->MeshEditing.Init(this);
	this->MeshDeformation.Init(this);
	this->MeshExtrusion.Init(this);
	this->MeshCutting.Init(this);
	this->MeshSmoothing.Init(this);
	this->MeshCreation.Init(this);
	this->Test.Init(this);
//	this->iEditMode=CREATION_MODE;
	OnModeCreation();


	float pos[4] = { 0, 0, 1, 0};
	SetLightPos(pos);

	this->iManipMode=VIEW_SELECTION_MODE;
	this->iRBSelName=NONE_SELECTED;
	this->iLBSelName=NONE_SELECTED;
	this->iColorMode=COLOR_ORIGINAL;
	vecDefaultColor.clear();
	vecDefaultColor.push_back(0.5);
	vecDefaultColor.push_back(0.9);
	vecDefaultColor.push_back(0.4);
	vecDefaultColor.push_back(1.0);
	this->iViewStyle=SMOOTH_VIEW;
	this->bAxis=false;
	this->iRenderPreMesh=MESH_PREVIEW;
	//init control panel(general tab)
	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPGeneral()!=NULL)
	{
		pCP->GetCPGeneral()->Init();
	}

	UpdateAllViews(NULL);

	return TRUE;
}




// CKWResearchWorkDoc serialization

void CKWResearchWorkDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		this->MeshEditing.Init(this);
		this->MeshDeformation.Init(this);
		this->MeshExtrusion.Init(this);
		this->MeshCutting.Init(this);
		this->MeshSmoothing.Init(this);
		this->MeshCreation.Init(this);
		this->Test.Init(this);
//		this->iEditMode=CREATION_MODE;
//		this->iEditMode=DEFORMATION_MODE;
		OnModeDeformation();

		this->iManipMode=VIEW_SELECTION_MODE;
		this->iRBSelName=NONE_SELECTED;
		this->iLBSelName=NONE_SELECTED;
		this->iColorMode=COLOR_ORIGINAL;
		this->iRenderPreMesh=MESH_EXIST_VIEW;

		UpdateAllViews(NULL);
	}
}


// CKWResearchWorkDoc diagnostics

#ifdef _DEBUG
void CKWResearchWorkDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKWResearchWorkDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CKWResearchWorkDoc commands
BOOL CKWResearchWorkDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	bool bScale,bCenter;
	bScale=bCenter=true;
	if (MessageBox(NULL,"Scale the model size?","",MB_YESNO)==IDNO)
	{
		bScale=false;
	}
	if (MessageBox(NULL,"Center the model?","",MB_YESNO)==IDNO)
	{
		bCenter=false;
	}

//	OBJHandle::glmReadOBJ((char *)lpszPathName,this->Mesh,bScale,bCenter);
	OBJHandle::glmReadOBJNew((char *)lpszPathName,this->Mesh,bScale,bCenter,this->vecDefaultColor);
	if(this->Mesh.empty())
	{
		AfxMessageBox("Read File Error!");
		return FALSE;
	}

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}

BOOL CKWResearchWorkDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	if (this->Mesh.empty())
	{
		AfxMessageBox("Save File Error!");
		return FALSE;
	}

	std::ofstream out(lpszPathName,ios_base::out | ios_base::trunc);
	if(!out)  
	{  
		AfxMessageBox("Create File Error!");
	}

	print_polyhedron_wavefront(out,Mesh);

	SetModifiedFlag(FALSE);
	return TRUE;

//	return CDocument::OnSaveDocument(lpszPathName);
}

CView* CKWResearchWorkDoc::GetView(CRuntimeClass* pClass)
{
	CView* pView;
	POSITION pos=GetFirstViewPosition();

	while(pos!=NULL){
		pView=GetNextView(pos);
		if(pView->IsKindOf(pClass))
			break;
	}

	if(!pView->IsKindOf(pClass)){
		AfxMessageBox("Connt Locate the View.");
		return NULL;
	}

	return pView;
}

vector<vector<Point_3> >& CKWResearchWorkDoc::GettestvecvecNewEdgeVertexPos()
{
	return this->testvecvecNewEdgeVertexPos;
}

bool CKWResearchWorkDoc::JudgeEditPlane()
{
	if (this->MeshDeformation.GetDeformCurvePoints().empty())
	{
		return false;
	}
	return true;
}

int CKWResearchWorkDoc::GetCurrentDesiredPointsPos(vector<HPCPoint> & DesiredPointsPosition)
{
	return this->MeshDeformation.GetCurrentDesiredPointsPos(DesiredPointsPosition);
}

//void CKWResearchWorkDoc::OnOperation2dto3d()
//{
//	// TODO: Add your command handler code here
//	CKWResearchWorkView* pView=(CKWResearchWorkView*)GetView(RUNTIME_CLASS(CKWResearchWorkView));
//
//	if (JudgeEditPlane())
//	{
//		SetModifiedPointsPos();
//	} 
//	else
//	{
//		//vector<HPCPoint> DesiredPointsPosition;
//		//if (openGLControl2D->SetModifiedPointsPos(DesiredPointsPosition)>0)
//		//{
//		//	openGLControl3D->SetModifiedPointsPos(DesiredPointsPosition);
//		//}
//	}
//	UpdateAllViews(NULL);
//}
//
//void CKWResearchWorkDoc::OnOperation3dto2d()
//{
//	// TODO: Add your command handler code here
//	//vector<HPCPoint> DesiredPointsPosition;
//	//if (openGLControl3D->GetCurrentDesiredPointsPos(DesiredPointsPosition)>0)
//	//{
//	//	openGLControl2D->GetCurrentDesiredPointsPos(DesiredPointsPosition);
//	//}
//	//openGLControl3D->RedrawWindow();
//	//openGLControl2D->RedrawWindow();
//}

void CKWResearchWorkDoc::OnModeCreation()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==CREATION_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=CREATION_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPCreation()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(CREATION_MODE+1);//1,tab for creation
	}

	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeCreation(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==CREATION_MODE);//if iMode==CREATION_MODE, show the radio dot here
}

void CKWResearchWorkDoc::OnModeEditing()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==EDITING_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=EDITING_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPEditing()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(EDITING_MODE+1);//2,tab for editing
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeEditing(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==EDITING_MODE);
}

void CKWResearchWorkDoc::OnModeDeformation()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==DEFORMATION_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=DEFORMATION_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPDeformation()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(DEFORMATION_MODE+1);//3,tab for deformation
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeDeformation(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==DEFORMATION_MODE);
}

void CKWResearchWorkDoc::OnModeExtrusion()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==EXTRUSION_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=EXTRUSION_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPExtrusion()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(EXTRUSION_MODE+1);//4,tab for extrusion
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeExtrusion(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==EXTRUSION_MODE);
}

void CKWResearchWorkDoc::OnModeCutting()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==CUTTING_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=CUTTING_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPCutting()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(CUTTING_MODE+1);//5,tab for cutting
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeCutting(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==CUTTING_MODE);
}

void CKWResearchWorkDoc::OnModeSmoothing()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==SMOOTHING_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=SMOOTHING_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPSmoothing()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(SMOOTHING_MODE+1);//6,tab for smoothing
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeSmoothing(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==SMOOTHING_MODE);
}

void CKWResearchWorkDoc::OnModeTest()
{
	// TODO: Add your command handler code here
	if (this->iEditMode==TEST_MODE)
	{
		return;
	}
	else
	{
		this->iEditMode=TEST_MODE;
	}

	CControlPanel* pCP=(CControlPanel*)(this->GetView(RUNTIME_CLASS(CControlPanel)));
	if (pCP->GetCPTest()!=NULL)
	{
		pCP->GetCPTab().SetCurFocus(TEST_MODE+1);//7,tab for test
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateModeTest(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iEditMode==TEST_MODE);
}

void CKWResearchWorkDoc::OnCurvatureNone()
{
	// TODO: Add your command handler code here
	if (this->iColorMode==COLOR_ORIGINAL)
	{
		return;
	}
	else
	{
		this->iColorMode=COLOR_ORIGINAL;
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateCurvatureNone(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iColorMode==COLOR_ORIGINAL);
}

void CKWResearchWorkDoc::OnCurvatureMeancurvature()
{
	// TODO: Add your command handler code here
	GeometryAlgorithm::ComputeMeshMeanCurvature(this->Mesh);
	if (this->iColorMode==COLOR_MEAN_CURVATURE)
	{
		return;
	}
	else
	{
		this->iColorMode=COLOR_MEAN_CURVATURE;
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnCurvatureGaussiancurvature()
{
	// TODO: Add your command handler code here
	GeometryAlgorithm::ComputeMeshGaussianCurvature(this->Mesh);
	if (this->iColorMode==COLOR_GAUSSIAN_CURVATURE)
	{
		return;
	}
	else
	{
		this->iColorMode=COLOR_GAUSSIAN_CURVATURE;
	}
	UpdateAllViews(NULL);
}

void CKWResearchWorkDoc::OnUpdateCurvatureMeancurvature(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iColorMode==COLOR_MEAN_CURVATURE);
}

void CKWResearchWorkDoc::OnUpdateCurvatureGaussiancurvature(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI-> SetRadio(iColorMode==COLOR_GAUSSIAN_CURVATURE);
}

void CKWResearchWorkDoc::OnViewDualmesh()
{
	// TODO: Add your command handler code here
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	if (pMain!=NULL)
	{
		CMenu*	mainmenu=pMain->GetMenu();
		if (mainmenu->GetMenuState(ID_VIEW_DUALMESH,MF_BYCOMMAND)==MF_CHECKED)
		{
			mainmenu->CheckMenuItem(ID_VIEW_DUALMESH,MF_UNCHECKED);	
			this->bShowDualMesh=false;
		} 
		else
		{
			mainmenu->CheckMenuItem(ID_VIEW_DUALMESH,MF_CHECKED);	
			this->bShowDualMesh=true;
		}
		UpdateAllViews(NULL);
	}
}

void CKWResearchWorkDoc::OnViewPrimalmesh()
{
	// TODO: Add your command handler code here
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	if (pMain!=NULL)
	{
		CMenu*	mainmenu=pMain->GetMenu();
		if (mainmenu->GetMenuState(ID_VIEW_PRIMALMESH,MF_BYCOMMAND)==MF_CHECKED)
		{
			mainmenu->CheckMenuItem(ID_VIEW_PRIMALMESH,MF_UNCHECKED);	
			this->bShowPrimalMesh=false;
		} 
		else
		{
			mainmenu->CheckMenuItem(ID_VIEW_PRIMALMESH,MF_CHECKED);	
			this->bShowPrimalMesh=true;
		}
		UpdateAllViews(NULL);
	}
}

void CKWResearchWorkDoc::OnOperationDeformationComputedualmesh()
{
	// TODO: Add your command handler code here
	//if in deformation mode
	if (this->iEditMode==DEFORMATION_MODE)
	{
		this->MeshDeformation.ComputeDualMesh(this->Mesh);
	}
}

void CKWResearchWorkDoc::OnOperationDeformationComputeEdgemesh()
{
	//if in deformation mode
	if (this->iEditMode==DEFORMATION_MODE)
	{
		this->MeshDeformation.ComputeEdgeMesh(this->Mesh);
	}
}

void CKWResearchWorkDoc::OnViewSelectMode()
{
	this->iManipMode=VIEW_SELECTION_MODE;
}

void CKWResearchWorkDoc::OnSketchMode()
{
	this->iManipMode=SKETCH_MODE;
}


void CKWResearchWorkDoc::OnHelpTest()
{
	// TODO: Add your command handler code here
	Point_3 Pnt0(3999,1924,-2024);
	Point_3 Pnt1(703,222,-1092);
	Point_3 Pnt2(-533,1360,2564);
	Triangle_3 Tri(Pnt0,Pnt1,Pnt2);
	Plane_3 Pln0=Tri.supporting_plane();
	DBWindowWrite("%f %f %f %f\n",Pln0.a(),Pln0.b(),Pln0.c(),Pln0.d());


	vector<int> vecInitNum;
	vecInitNum.push_back(1);
	vecInitNum.push_back(9);
	vecInitNum.push_back(8);
	vecInitNum.push_back(1);
	vecInitNum.push_back(0);
	vecInitNum.push_back(9);
	vecInitNum.push_back(5);
	set<int> setNum;
	for (unsigned int i=0;i<vecInitNum.size();i++)
	{
		pair<set<int>::iterator, bool> pairResult;
		pairResult=setNum.insert(vecInitNum.at(i));
	}



	vector<Point_3> SamplePoints;
	GeometryAlgorithm::SampleCircle(Point_3(0,0,0),0.3,20,SamplePoints);
	FILE* pfile=fopen("circle0.contour","w");
	for (unsigned int i=0;i<SamplePoints.size();i++)
	{
		fprintf(pfile,"%.3f %.3f %.3f\n",SamplePoints.at(i).x(),SamplePoints.at(i).y(),SamplePoints.at(i).z());
	}
	fclose(pfile);




	Polygon_2 PolyTest;
	PolyTest.push_back(Point_2(0,0));
	PolyTest.push_back(Point_2(1,1));
	PolyTest.push_back(Point_2(2,0));
	PolyTest.push_back(Point_2(2,2));
	PolyTest.push_back(Point_2(0,2));

	Point_2 ResultPoint;
	bool bResult=GeometryAlgorithm::GetArbiPointInPolygon(PolyTest,ResultPoint);
	DBWindowWrite("result point: %f %f\n",ResultPoint.x(),ResultPoint.y());



	SparseMatrix LHMatrix(2);
	LHMatrix.m = 3;
	LHMatrix[0][0] = 2;
	LHMatrix[0][1] = -1;
	LHMatrix[0][2] = 1;
	LHMatrix[1][1] = 1;
	LHMatrix[1][2] = 1;

	SparseMatrix LHMatrixAT(LHMatrix.NCols());
	CMath MathCompute;
	MathCompute.TAUCSFactorize(LHMatrix,LHMatrixAT);

	vector<vector<double>> RightMatB,ResultMat;
	vector<double> BRow;
	BRow.push_back(5);BRow.push_back(3);
	RightMatB.push_back(BRow);
	BRow.clear();
	BRow.push_back(10);BRow.push_back(1);
	RightMatB.push_back(BRow);
	BRow.clear();


	MathCompute.TAUCSComputeLSE(LHMatrixAT,RightMatB,ResultMat);

	return;

	int iVer=this->Mesh.size_of_vertices();
	int iEdge=this->Mesh.size_of_halfedges();
	int iFacet=this->Mesh.size_of_facets();

	Polygon_2 BoundingPolygon0;
	Polygon_2 BoundingPolygon1;

	bool bSimple0=BoundingPolygon0.is_simple();
	bool bSimple1=BoundingPolygon1.is_simple();
	bool bConvex0=BoundingPolygon0.is_convex();
	bool bConvex1=BoundingPolygon1.is_convex();
	bool bOrien0=BoundingPolygon0.is_clockwise_oriented();
	bool bOrien1=BoundingPolygon1.is_clockwise_oriented();

	BoundingPolygon0.reverse_orientation();
	BoundingPolygon1.reverse_orientation();
	//float?
	bool bIntersect=CGAL::do_intersect(BoundingPolygon0,BoundingPolygon1);

	bool bCW=BoundingPolygon0.is_clockwise_oriented();
	bool bConvex=BoundingPolygon0.is_convex();


	Plane_3 plane(1,1,1,0);
	vector<vector<Point_3>> IntersectCurves;
	int iNum=GeometryAlgorithm::GetMeshPlaneIntersection(this->Mesh,plane,IntersectCurves);



	CMath CMathTest;
	CMathTest.testTAUCS();



//	Vector_3 vec0(Point_3(0,0,1),Point_3(0,0,0));
	Vector_3 vec0(Point_3(0,0,0),Point_3(0,0,1));
//	Vector_3 vec0(0,0,1);
//	Vector_3 vec1(0,-1,-1);
	Vector_3 vec1(Point_3(0,0,0),Point_3(0,-1,-1));
	double dAngle=GeometryAlgorithm::GetAngleBetweenTwoVectors3d(vec0,vec1);


	vector<double> number;
	number.push_back(2);
	number.push_back(4);
	number.push_back(4);
	number.push_back(4);
	number.push_back(5);
	number.push_back(5);
	number.push_back(7);
	number.push_back(9);
	double dderi=GeometryAlgorithm::GetDerivation(number);





	Point_3 center(0,0,0);
	Sphere_3 sphere(center,1);
	Line_3 line(Point_3(0,2,1),Point_3(0,2,-1));
	vector<Point_3> points;
	GeometryAlgorithm::GetLineSphereIntersection(line,sphere,points);



	vector<Point_3> Group0,Group1;
	vector<Int_Int_Pair> GroupResult;

	Group0.push_back(Point_3(3,3,2));
	Group0.push_back(Point_3(5,3,2));
	Group0.push_back(Point_3(2,3,2));
	Group0.push_back(Point_3(6,3,2));
	Group0.push_back(Point_3(4,3,2));
	Group0.push_back(Point_3(1,3,2));

	Group1.push_back(Point_3(3,4,2));
	Group1.push_back(Point_3(1,4,2));
	Group1.push_back(Point_3(6,4,2));
	Group1.push_back(Point_3(2,4,2));
	Group1.push_back(Point_3(5,4,2));
	Group1.push_back(Point_3(4,4,2));

	vector<Point_3> vecMidPoint;
	GeometryAlgorithm::GroupNearestPoints(Group0,Group1,GroupResult,vecMidPoint);//

	vector<Point_3> OriginalCurve;
	OriginalCurve.push_back(Point_3(-1,0,2));
	OriginalCurve.push_back(Point_3(1,0,2));
	OriginalCurve.push_back(Point_3(1,0,0));
	OriginalCurve.push_back(Point_3(-1,0,0));
	vector<Point_3> NewCurve=OriginalCurve;
	vector<int> HandleInd;
	HandleInd.push_back(0);
	HandleInd.push_back(1);
	vector<Point_3> NewPos;
	NewPos.push_back(Point_3(-1,0,3));
	NewPos.push_back(Point_3(1,0,3));
//	CCurveDeform::ClosedCurveNaiveLaplacianDeform(NewCurve,HandleInd,NewPos,1);




	vector<vector<float>> MatrixA,MatrixB,Result;
	vector<float> CurrentRow;
	CurrentRow.push_back(2);CurrentRow.push_back(0);CurrentRow.push_back(1);
	MatrixA.push_back(CurrentRow);
	CurrentRow.clear();
	CurrentRow.push_back(3);CurrentRow.push_back(1);CurrentRow.push_back(2);
	MatrixA.push_back(CurrentRow);
	CurrentRow.clear();
	CurrentRow.push_back(0);CurrentRow.push_back(1);CurrentRow.push_back(0);
	MatrixA.push_back(CurrentRow);
	CurrentRow.clear();

	CurrentRow.push_back(1);CurrentRow.push_back(3);CurrentRow.push_back(0);
	MatrixB.push_back(CurrentRow);
	CurrentRow.clear();
	CurrentRow.push_back(2);CurrentRow.push_back(0);CurrentRow.push_back(2);
	MatrixB.push_back(CurrentRow);
	CurrentRow.clear();
	CurrentRow.push_back(1);CurrentRow.push_back(0);CurrentRow.push_back(1);
	MatrixB.push_back(CurrentRow);
	CurrentRow.clear();



	int iANonZeroSize=0;
	for (unsigned int i=0;i<MatrixA.size();i++)
	{
		for (unsigned int j=0;j<MatrixA.front().size();j++)
		{
			if (MatrixA.at(i).at(j)!=0)
			{
				iANonZeroSize++;
			}
		}
	}
	KW_SparseMatrix A(MatrixA.size(),MatrixA.front().size(),iANonZeroSize);
	for (unsigned int i=0;i<MatrixA.size();i++)
	{
		for (unsigned int j=0;j<MatrixA.front().size();j++)
		{
			if (MatrixA.at(i).at(j)!=0)
			{
				A.fput(i,j,MatrixA.at(i).at(j));
			}
		}
	}

	int iBNonZeroSize=0;
	for (unsigned int i=0;i<MatrixB.size();i++)
	{
		for (unsigned int j=0;j<MatrixB.front().size();j++)
		{
			if (MatrixB.at(i).at(j)!=0)
			{
				iBNonZeroSize++;
			}
		}
	}
	KW_SparseMatrix B(MatrixB.size(),MatrixB.front().size(),iBNonZeroSize);
	for (unsigned int i=0;i<MatrixB.size();i++)
	{
		for (unsigned int j=0;j<MatrixB.front().size();j++)
		{
			if (MatrixB.at(i).at(j)!=0)
			{
				B.fput(i,j,MatrixB.at(i).at(j));
			}
		}
	}


	KW_SparseMatrix C(A.m,B.n,0);
	KW_SparseMatrix::KW_multiply(A,B,C);

	int iIndex=0;
	Result.clear();
	for (int i=0;i<C.m;i++)
	{
		vector<float> CurrentRow;
		for (int j=0;j<C.n;j++)
		{
			iIndex=0;
			while (iIndex<C.vol)
			{
				if (C.indx[iIndex]==i&&C.jndx[iIndex]==j)
				{
					break;
				}
				iIndex++;
			}
			if (iIndex!=C.vol)
			{
				CurrentRow.push_back(C.array[iIndex]);
			}
			else
			{
				CurrentRow.push_back(0);
			}
		}
		Result.push_back(CurrentRow);
	}
}








