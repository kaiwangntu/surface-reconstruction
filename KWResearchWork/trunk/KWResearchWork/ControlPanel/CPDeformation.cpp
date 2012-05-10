// CPDeformation.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPDeformation.h"


// CCPDeformation dialog

IMPLEMENT_DYNAMIC(CCPDeformation, CDialog)

CCPDeformation::CCPDeformation(CWnd* pParent /*=NULL*/)
	: CDialog(CCPDeformation::IDD, pParent)
	, DE_IterNum(0)
	, DE_Lambda(0)
{

}

CCPDeformation::~CCPDeformation()
{
}

CCPDeformation::CCPDeformation(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPDeformation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DE_SLIDER_ROI, DE_Slider_ROI);
	DDX_Text(pDX, IDC_DE_ITERNUM, DE_IterNum);
	DDX_Text(pDX, IDC_DE_LAMBDA, DE_Lambda);
	DDX_Control(pDX, IDC_DE_SLIDER_Material, DE_Slider_Material);
	DDX_Control(pDX, IDC_DE_Material_Value, DE_Static_Material_Value);
	DDX_Control(pDX, IDC_DE_COMBO_MatSetWay, DE_MatSetWay);
}


BEGIN_MESSAGE_MAP(CCPDeformation, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_DE_ClearROI, &CCPDeformation::OnBnClickedDeClearroi)
	ON_BN_CLICKED(IDC_DE_ITER_LAMBDA, &CCPDeformation::OnBnClickedDeIterLambda)
	ON_BN_CLICKED(IDC_DE_Deform, &CCPDeformation::OnBnClickedDeDeform)
	ON_BN_CLICKED(IDC_DE_ComputeDualMesh, &CCPDeformation::OnBnClickedDeComputedualmesh)
	ON_BN_CLICKED(IDC_DE_ComputeEdgeMesh, &CCPDeformation::OnBnClickedDeComputeedgemesh)
	ON_BN_CLICKED(IDC_DE_INTERPOLATION, &CCPDeformation::OnBnClickedDeInterpolation)
	ON_BN_CLICKED(IDC_DE_SetMat, &CCPDeformation::OnBnClickedDeSetmat)
END_MESSAGE_MAP()


// CCPDeformation message handlers
void CCPDeformation::Init()
{
	//render ref plane in deformation mode
	if (pDoc->GetMeshDeformation().bRenderRefPlane[0])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BASICPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BASICPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshDeformation().bRenderRefPlane[1])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_TANGENTIALPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_TANGENTIALPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshDeformation().bRenderRefPlane[2])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BINORMALPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BINORMALPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshDeformation().bRenderSphere)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_REFSPHERE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_REFSPHERE);
		m_Check->SetCheck(BST_UNCHECKED);
	}

	//slider for ROI
	DE_Slider_ROI.SetRange(0,100);
	for (int i=0;i<101;i++)
	{
		DE_Slider_ROI.SetTic(i);
	}
	int i=pDoc->GetMeshDeformation().GetSelectionRange()*100;
	DE_Slider_ROI.SetPos(i);

	pDoc->GetMeshDeformation().GetFlexibleDeformPara(this->DE_IterNum,this->DE_Lambda);

	if (pDoc->GetMeshDeformation().bRenderHandleNb)
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowHandle))->SetCheck(TRUE);
	}
	else
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowHandle))->SetCheck(FALSE);
	}
	if (pDoc->GetMeshDeformation().bRenderROI)
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowROI))->SetCheck(TRUE);
	}
	else
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowROI))->SetCheck(FALSE);
	}
	if (pDoc->GetMeshDeformation().bRenderAnchor)
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowAnchor))->SetCheck(TRUE);
	}
	else
	{
		((CButton*)this->GetDlgItem(IDC_DE_ShowAnchor))->SetCheck(FALSE);
	}

	((CButton*)this->GetDlgItem(IDC_DE_ShowMat))->SetCheck(FALSE);

	//slider for material
	DE_Slider_Material.SetRange(0,100);
	DE_Slider_Material.AddColor(0,100,RGB(255,0,0),RGB(0,0,255));//red 0,blue 1
//	DE_Slider_Material.AddColor(0,49,RGB(255,0,0),RGB(0,254,0));
//	DE_Slider_Material.AddColor(50,100,RGB(0,255,0),RGB(0,0,255));
	DE_Slider_Material.Refresh();	// force screen update of newly configured slider
	DE_Slider_Material.setCallback(sItemUpdate,this,(LPARAM)IDC_DE_SLIDER_Material);
	//set to default material value
	double dMaterial=pDoc->GetMeshDeformation().GetMaterial();
	DE_Slider_Material.SetPos((int)(dMaterial*100));

	//material specification method
	DE_MatSetWay.ResetContent();
	DE_MatSetWay.AddString(_T("Auto Uniform"));
	DE_MatSetWay.AddString(_T("Auto Harmonic"));
	DE_MatSetWay.AddString(_T("Learn Material"));
	DE_MatSetWay.AddString(_T("Export Material"));
	DE_MatSetWay.AddString(_T("Import Material"));
	DE_MatSetWay.SetCurSel(-1);

	pDoc->UpdateAllViews(NULL);
	UpdateData(FALSE);
}

BOOL CCPDeformation::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	WORD   wID   =   LOWORD(   wParam   );  
	WORD   wNF   =   HIWORD(   wParam   );  
	if(wID ==IDC_DE_BASICPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BASICPLANE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[0]=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[0]=false;
		}
	}
	else if(wID ==IDC_DE_TANGENTIALPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_TANGENTIALPLANE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[1]=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[1]=false;
		}
	}
	else if(wID ==IDC_DE_BINORMALPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_BINORMALPLANE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[2]=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderRefPlane[2]=false;
		}
	}
	else if(wID ==IDC_DE_REFSPHERE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_REFSPHERE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderSphere=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderSphere=false;
		}
	}
	else if(wID ==IDC_DE_ShowHandle && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_ShowHandle);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderHandleNb=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderHandleNb=false;
		}
	}
	else if(wID ==IDC_DE_ShowROI && wNF == BN_CLICKED)
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_ShowROI);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderROI=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderROI=false;
		}
	}
	else if(wID ==IDC_DE_ShowAnchor && wNF == BN_CLICKED)
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_ShowAnchor);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshDeformation().bRenderAnchor=true;
		} 
		else
		{
			pDoc->GetMeshDeformation().bRenderAnchor=false;
		}
	}
	else if(wID ==IDC_DE_ShowMat && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_DE_ShowMat);
		if (m_Check->GetCheck()==BST_CHECKED)//show the material
		{
			pDoc->SetColorMode(COLOR_DEFORMATION_MATERIAL);
		} 
		else//show the default color
		{
			pDoc->SetColorMode(COLOR_ORIGINAL);
		}
	}
	pDoc->UpdateAllViews((CView*)pCP);
	return CDialog::OnCommand(wParam, lParam);
}

void CCPDeformation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar==NULL)
	{
		return;
	}

	int j=0;
	double i=0;

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_DE_SLIDER_ROI:
		j=DE_Slider_ROI.GetPos();
		i=j*0.01;
		pDoc->GetMeshDeformation().SetSelectionRange(i,pDoc->GetMesh());
		break;
	default:
		break;
	}

	pDoc->UpdateAllViews((CView*)pCP);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CCPDeformation::OnBnClickedDeClearroi()
{
	// TODO: Add your control notification handler code here
	this->DE_Slider_ROI.SetPos(0);
	pDoc->GetMeshDeformation().SetSelectionRange(0,pDoc->GetMesh());
	pDoc->UpdateAllViews((CView*)pCP);
}

void CCPDeformation::OnBnClickedDeIterLambda()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	pDoc->GetMeshDeformation().SetFlexibleDeformPara(this->DE_IterNum,this->DE_Lambda);
}

void CCPDeformation::OnBnClickedDeDeform()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_DE_Deform);
	pButton->EnableWindow(FALSE);

	vector<Point_3> emptyVec;
	pDoc->SetTestPoints(emptyVec);
	if (pDoc->GetMeshDeformation().SetModifiedPointsPos(pDoc->GetMesh(),pDoc->GetTestPointsRef()))
	{
		CString  strTitle=pDoc->GetTitle();
		strTitle=strTitle+"*";
		pDoc->SetTitle(strTitle);
		pDoc->SetModifiedFlag(TRUE);
	}
	
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPDeformation::OnBnClickedDeComputedualmesh()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_DE_ComputeDualMesh);
	pButton->EnableWindow(FALSE);

	if (pDoc->GetEditMode()==DEFORMATION_MODE)
	{
		pDoc->GetMeshDeformation().ComputeDualMesh(pDoc->GetMesh());
	} 
	
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPDeformation::OnBnClickedDeComputeedgemesh()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_DE_ComputeEdgeMesh);
	pButton->EnableWindow(FALSE);

	if (pDoc->GetEditMode()==DEFORMATION_MODE)
	{
		pDoc->GetMeshDeformation().ComputeEdgeMesh(pDoc->GetMesh());
	}
	
	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPDeformation::OnBnClickedDeInterpolation()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_DE_INTERPOLATION);
	pButton->EnableWindow(FALSE);

	if (pDoc->GetEditMode()==DEFORMATION_MODE)
	{
		pDoc->GetMeshDeformation().DeformInterpolation(pDoc->GetMesh());
	}

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPDeformation::sItemUpdate(CCPDeformation *obj, LPARAM data1,int sValue, BOOL IsDragging)
{
	CCPDeformation *me = (CCPDeformation *)obj;
	me->ItemUpdate(data1, sValue, IsDragging);
}

void CCPDeformation::ItemUpdate(LPARAM data1, int sValue,BOOL /* IsDragging */)
{
	double slope1 = 0.01;//0.05;
	double intercept1 = 0;
	CString val;

	switch(data1)
	{
	case IDC_DE_SLIDER_Material:
		val.Format(_T("%6.2lf"), (slope1 * double(sValue)) + intercept1);
		DE_Static_Material_Value.SetWindowText(val);
		//change material value
		pDoc->GetMeshDeformation().SetMaterial(slope1 * double(sValue));		
		break;
	default:
		break;
	}
}

void CCPDeformation::OnBnClickedDeSetmat()
{
	// TODO: Add your control notification handler code here
	switch (this->DE_MatSetWay.GetCurSel())
	{
	case 0:
		pDoc->GetMeshDeformation().SetAutoUniMat();
		break;
	case 1:
		pDoc->GetMeshDeformation().SetAutoHarMat();
		break;
	case 2:
		pDoc->GetMeshDeformation().LearnMat();
		break;
	case 3:
		pDoc->GetMeshDeformation().ExportMat();
		break;
	case 4:
		pDoc->GetMeshDeformation().ImportMat();
		break;
	default:
		break;
	}
	pDoc->UpdateAllViews((CView*)pCP);
}
