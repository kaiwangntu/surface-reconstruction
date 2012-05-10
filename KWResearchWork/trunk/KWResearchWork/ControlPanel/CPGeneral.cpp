// ControlPanel/CPGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPGeneral.h"


// CCPGeneral dialog

IMPLEMENT_DYNAMIC(CCPGeneral, CDialog)

CCPGeneral::CCPGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(CCPGeneral::IDD, pParent)
	, iRadioViewStyle(0)
{

}

CCPGeneral::~CCPGeneral()
{
}

CCPGeneral::CCPGeneral(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GE_SLIDER_LIGHTPOS_X, GE_Slider_LightPos_X);
	DDX_Control(pDX, IDC_GE_SLIDER_LIGHTPOS_Y, GE_Slider_LightPos_Y);
	DDX_Control(pDX, IDC_GE_SLIDER_LIGHTPOS_Z, GE_Slider_LightPos_Z);
	DDX_Control(pDX, IDC_GE_SLIDER_COLOR_RED, GE_Slider_Color_Red);
	DDX_Control(pDX, IDC_GE_SLIDER_COLOR_GREEN, GE_Slider_Color_Green);
	DDX_Control(pDX, IDC_GE_SLIDER_COLOR_BLUE, GE_Slider_Color_Blue);
	DDX_Radio(pDX, IDC_GE_WireFrame, iRadioViewStyle);
}


BEGIN_MESSAGE_MAP(CCPGeneral, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_GE_WireFrame, &CCPGeneral::OnBnClickedGeWireframe)
	ON_BN_CLICKED(IDC_GE_SMOOTH, &CCPGeneral::OnBnClickedGeSmooth)
	ON_BN_CLICKED(IDC_GE_Hybrid, &CCPGeneral::OnBnClickedGeHybrid)
	ON_BN_CLICKED(IDC_GE_Points, &CCPGeneral::OnBnClickedGePoints)
	ON_BN_CLICKED(IDC_GE_Subdivision, &CCPGeneral::OnBnClickedSubdivision)
	ON_BN_CLICKED(IDC_GE_SnapShot, &CCPGeneral::OnBnClickedGeSnapshot)
	ON_BN_CLICKED(IDC_GE_Exp_Sce_Para, &CCPGeneral::OnBnClickedExpScePara)
	ON_BN_CLICKED(IDC_GE_Imp_Sce_Para, &CCPGeneral::OnBnClickedImpScePara)
END_MESSAGE_MAP()


void CCPGeneral::Init()
{
	//light settings
	float* LightPos=pDoc->GetLightPos();
	GE_Slider_LightPos_X.SetRange(0,1000);
	GE_Slider_LightPos_X.SetPos(LightPos[0]*250+500);
	GE_Slider_LightPos_Y.SetRange(0,1000);
	GE_Slider_LightPos_Y.SetPos(LightPos[1]*250+500);
	GE_Slider_LightPos_Z.SetRange(0,1000);
	GE_Slider_LightPos_Z.SetPos(LightPos[2]*250+500);
	//mesh color settings
	vector<double> MeshColor=pDoc->GetDefaultColor();
	GE_Slider_Color_Red.SetRange(0,1000);
	GE_Slider_Color_Red.SetPos(MeshColor[0]*1000);
	GE_Slider_Color_Green.SetRange(0,1000);
	GE_Slider_Color_Green.SetPos(MeshColor[1]*1000);
	GE_Slider_Color_Blue.SetRange(0,1000);
	GE_Slider_Color_Blue.SetPos(MeshColor[2]*1000);
	//set view style
	this->iRadioViewStyle=pDoc->GetViewStyle();
	//show axis or not
	if (pDoc->IsAxisOn())
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_Axis);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_Axis);
		m_Check->SetCheck(BST_UNCHECKED);
	}

	if (pDoc->GetRenderPreMesh()==MESH_PREVIEW)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_PREMESH);
		m_Check->EnableWindow(TRUE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else if (pDoc->GetRenderPreMesh()==MESH_NOT_PREVIEW)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_PREMESH);
		m_Check->EnableWindow(TRUE);
		m_Check->SetCheck(BST_UNCHECKED);
	}


	UpdateData(FALSE);
}

// CCPGeneral message handlers

void CCPGeneral::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar==NULL)
	{
		return;
	}

	float* LightPos=pDoc->GetLightPos();
	int j=0;
	vector<double> vecNewColor;
	if (!pDoc->GetMesh().empty())
	{
		vecNewColor=pDoc->GetDefaultColor();
	}

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_GE_SLIDER_LIGHTPOS_X:
		j=GE_Slider_LightPos_X.GetPos();
		LightPos[0]=(float)(j-500)/250.0;
		pDoc->SetLightPos(LightPos);
		pDoc->UpdateAllViews((CView*)pCP);
		break;
	case IDC_GE_SLIDER_LIGHTPOS_Y:
		j=GE_Slider_LightPos_Y.GetPos();
		LightPos[1]=(float)(j-500)/250.0;
		pDoc->SetLightPos(LightPos);
		pDoc->UpdateAllViews((CView*)pCP);
		break;
	case IDC_GE_SLIDER_LIGHTPOS_Z:
		j=GE_Slider_LightPos_Z.GetPos();
		LightPos[2]=(float)(j-500)/250.0;
		pDoc->SetLightPos(LightPos);
		pDoc->UpdateAllViews((CView*)pCP);
		break;
	case IDC_GE_SLIDER_COLOR_RED:
		j=GE_Slider_Color_Red.GetPos();

		if (!pDoc->GetMesh().empty())
		{
			vecNewColor.at(0)=(float)j/1000.0;
			pDoc->SetDefaultColor(vecNewColor);
//			GeometryAlgorithm::SetUniformMeshColor(pDoc->GetMesh(),vecNewColor);	
//			pDoc->GetMesh().SetRenderInfo(false,false,false,false,true);
		}

		pDoc->UpdateAllViews((CView*)pCP);
		break;
	case IDC_GE_SLIDER_COLOR_GREEN:
		j=GE_Slider_Color_Green.GetPos();

		if (!pDoc->GetMesh().empty())
		{
			vecNewColor.at(1)=(float)j/1000.0;
			pDoc->SetDefaultColor(vecNewColor);
//			GeometryAlgorithm::SetUniformMeshColor(pDoc->GetMesh(),vecNewColor);	
//			pDoc->GetMesh().SetRenderInfo(false,false,false,false,true);
		}

		pDoc->UpdateAllViews((CView*)pCP);
		break;
	case IDC_GE_SLIDER_COLOR_BLUE:
		j=GE_Slider_Color_Blue.GetPos();

		if (!pDoc->GetMesh().empty())
		{
			vecNewColor.at(2)=(float)j/1000.0;
			pDoc->SetDefaultColor(vecNewColor);
//			GeometryAlgorithm::SetUniformMeshColor(pDoc->GetMesh(),vecNewColor);	
//			pDoc->GetMesh().SetRenderInfo(false,false,false,false,true);
		}

		pDoc->UpdateAllViews((CView*)pCP);
		break;
	default:
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCPGeneral::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	WORD   wID   =   LOWORD(   wParam   );  
	WORD   wNF   =   HIWORD(   wParam   );  
	if(wID ==IDC_GE_Axis && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_Axis);
		int iState=m_Check->GetCheck();
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->RenderAxis(true);
		} 
		else
		{
			pDoc->RenderAxis(false);
		}
	} 
	else if (wID ==IDC_GE_PREMESH && wNF == BN_CLICKED)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_GE_PREMESH);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->SetRenderPreMesh(MESH_PREVIEW);
		} 
		else
		{
			pDoc->SetRenderPreMesh(MESH_NOT_PREVIEW);
		}
	}

	pDoc->UpdateAllViews((CView*)pCP);
	return CDialog::OnCommand(wParam, lParam);
}

void CCPGeneral::OnBnClickedGeWireframe()
{
	// TODO: Add your control notification handler code here
	pDoc->SetViewStyle(WIREFRAME_VIEW);//0 wireframe, 1 smooth, 2 hybrid, 3 points
	iRadioViewStyle=0;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPGeneral::OnBnClickedGeSmooth()
{
	// TODO: Add your control notification handler code here
	pDoc->SetViewStyle(SMOOTH_VIEW);//0 wireframe, 1 smooth, 2 hybrid, 3 points
	iRadioViewStyle=1;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPGeneral::OnBnClickedGeHybrid()
{
	// TODO: Add your control notification handler code here
	pDoc->SetViewStyle(HYBRID_VIEW);//0 wireframe, 1 smooth, 2 hybrid, 3 points
	iRadioViewStyle=2;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPGeneral::OnBnClickedGePoints()
{
	// TODO: Add your control notification handler code here
	pDoc->SetViewStyle(POINTS_VIEW);//0 wireframe, 1 smooth, 2 hybrid, 3 points
	iRadioViewStyle=3;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPGeneral::OnBnClickedSubdivision()
{
	// TODO: Add your control notification handler code here
	if (!pDoc->GetMesh().empty())
	{
		//OBJHandle::LoopSubDivision(pDoc->GetMesh());
		OBJHandle::CCSubDivision(pDoc->GetMesh());
		CString  strTitle=pDoc->GetTitle();
		strTitle=strTitle+"*";
		pDoc->SetTitle(strTitle);
		pDoc->SetModifiedFlag(TRUE);
	}

	pDoc->UpdateAllViews(NULL);
}

void CCPGeneral::OnBnClickedGeSnapshot()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_GE_SnapShot);
	pButton->EnableWindow(FALSE);

	CFileDialog dlg(FALSE,".bmp","*.bmp",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"BMP Files(*.BMP)|*.bmp||",AfxGetMainWnd());    //构造文件另存为对话
	if(dlg.DoModal() == IDOK)         //判断是否按下"保存"按钮
	{
		BeginWaitCursor();
		CString strPath = dlg.GetPathName();       //获得文件保存路径
		if(strPath.Right(4) != ".bmp")       //判断文件扩展名
			strPath += ".bmp";        //设置文件扩展名

		CKWResearchWorkView* pView=(CKWResearchWorkView*)pDoc->GetView(RUNTIME_CLASS(CKWResearchWorkView));
		pView->saveSceneImage(strPath);

		EndWaitCursor();
	}
	
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPGeneral::OnBnClickedExpScePara()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_GE_Exp_Sce_Para);
	pButton->EnableWindow(FALSE);
	//export to file
	CFileDialog dlg(FALSE,".sce","*.sce",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"Scene Files(*.sce)|*.sce||",AfxGetMainWnd());    //构造文件另存为对话
	if(dlg.DoModal() == IDOK)         //判断是否按下"保存"按钮
	{
		CString strPath = dlg.GetPathName();       //获得文件保存路径
		if(strPath.Right(4) != ".sce")       //判断文件扩展名
			strPath += ".sce";        //设置文件扩展名
		FILE* pFile;
		fopen_s(&pFile,strPath,"w");
		//light position
		fprintf_s(pFile,"#light position\n");
		fprintf_s(pFile,"%f %f %f %f\n",pDoc->GetLightPos()[0],pDoc->GetLightPos()[1],
			pDoc->GetLightPos()[2],pDoc->GetLightPos()[3]);

		CKWResearchWorkView* pView=(CKWResearchWorkView*)pDoc->GetView(RUNTIME_CLASS(CKWResearchWorkView));
		//arcball transformation
		fprintf_s(pFile,"#arball transformation\n");
		for (int i=0;i<16;i++)
		{
			fprintf_s(pFile,"%f ",pView->GetArcballTrans()[i]);
		}
		fprintf_s(pFile,"\n");
		//translate parameter
		fprintf_s(pFile,"#translation parameter\n");
		float OutPutX,OutPutY,OutPutZ;
		pView->GetTranslatePara(OutPutX,OutPutY,OutPutZ);
		fprintf_s(pFile,"%f %f %f\n",OutPutX,OutPutY,OutPutZ);
		//viewport
		fprintf_s(pFile,"#viewport\n");
		for (int i=0;i<4;i++)
		{
			fprintf_s(pFile,"%d ",pView->GetViewport()[i]);
		}
		fprintf_s(pFile,"\n");
		//modelview
		fprintf_s(pFile,"#modelview\n");
		for (int i=0;i<16;i++)
		{
			fprintf_s(pFile,"%f ",pView->GetModelview()[i]);
		}
		fprintf_s(pFile,"\n");
		//projection
		fprintf_s(pFile,"#projection\n");
		for (int i=0;i<16;i++)
		{
			fprintf_s(pFile,"%f ",pView->GetProjection()[i]);
		}
		fclose(pFile);
	}

	
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPGeneral::OnBnClickedImpScePara()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_GE_Imp_Sce_Para);
	pButton->EnableWindow(FALSE);
	//import and set
	CFileDialog dlg(TRUE,".sce","*.sce",OFN_HIDEREADONLY,
		"Scene Files(*.sce)|*.sce||",AfxGetMainWnd());   
	if(dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();       //获得文件保存路径
		FILE* pFile;
		fopen_s(&pFile,strPath,"r");
		//light position
		char buf[MAX_PATH];
		fgets(buf, sizeof(buf), pFile);
		float LightPos[4];
		fscanf_s(pFile,"%f %f %f %f\n",&LightPos[0],&LightPos[1],&LightPos[2],&LightPos[3]);
		pDoc->SetLightPos(LightPos);
		GE_Slider_LightPos_X.SetPos(LightPos[0]*250+500);
		GE_Slider_LightPos_Y.SetPos(LightPos[1]*250+500);
		GE_Slider_LightPos_Z.SetPos(LightPos[2]*250+500);

		CKWResearchWorkView* pView=(CKWResearchWorkView*)pDoc->GetView(RUNTIME_CLASS(CKWResearchWorkView));
		//arcball transformation
		fgets(buf, sizeof(buf), pFile);
		GLfloat M[16];
		for (int i=0;i<16;i++)
		{
			fscanf_s(pFile,"%f\n",&M[i]);
		}
		pView->SetArcballTrans(M);

		//translate parameter
		fgets(buf, sizeof(buf), pFile);
		float OutPutX,OutPutY,OutPutZ;
		fscanf_s(pFile,"%f %f %f\n",&OutPutX,&OutPutY,&OutPutZ);
		pView->SetTranslatePara(OutPutX,OutPutY,OutPutZ);

		//viewport
		fgets(buf, sizeof(buf), pFile);
		GLint viewport[4];
		for (int i=0;i<4;i++)
		{
			fscanf_s(pFile,"%d\n",&viewport[i]);
		}
		pView->SetViewport(viewport);

		//modelview
		fgets(buf, sizeof(buf), pFile);
		GLdouble modelview[16];
		for (int i=0;i<16;i++)
		{
			fscanf_s(pFile,"%lf\n",&modelview[i]);
		}
		pView->SetModelview(modelview);

		//projection
		fgets(buf, sizeof(buf), pFile);
		GLdouble projection[16];
		for (int i=0;i<16;i++)
		{
			fscanf_s(pFile,"%lf\n",&projection[i]);
		}
		pView->SetProjection(projection);
		fclose(pFile);
	}

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}
