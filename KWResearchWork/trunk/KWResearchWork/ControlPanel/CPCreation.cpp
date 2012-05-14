// CPCreation.cpp : implementation file
//

#include "stdafx.h"
#include "../KWResearchWork.h"
#include "CPCreation.h"
#include "ControlPanel.h"

// CCPCreation dialog

IMPLEMENT_DYNAMIC(CCPCreation, CDialog)

CCPCreation::CCPCreation(CWnd* pParent /*=NULL*/)
	: CDialog(CCPCreation::IDD, pParent)
	, iRadioModAlgo(0)
{

}

CCPCreation::~CCPCreation()
{
}

CCPCreation::CCPCreation(CKWResearchWorkDoc * docDataIn,CControlPanel * cpDataIn)
{
	this->pDoc=docDataIn;
	this->pCP=cpDataIn;
}

void CCPCreation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_MOD_ALGO_TJ, iRadioModAlgo);
	DDX_Control(pDX, IDC_CR_COMBO_SINGLEPOLY, CR_SINGLEPOLY);
	DDX_Control(pDX, IDC_CR_COMBO_SINGLESS, CR_SINGLESS);
	DDX_Control(pDX, IDC_CR_COMBO_UNIONPOLY, CR_UNIONPOLY);
}


BEGIN_MESSAGE_MAP(CCPCreation, CDialog)
	ON_BN_CLICKED(IDC_CR_ReadContour, &CCPCreation::OnBnClickedCrReadcontour)
	ON_BN_CLICKED(IDC_CR_WriteContour, &CCPCreation::OnBnClickedCrWritecontour)
	ON_BN_CLICKED(IDC_CR_AdjustContourView, &CCPCreation::OnBnClickedCrAdjustcontourview)
	ON_BN_CLICKED(IDC_CR_GenerateMesh, &CCPCreation::OnBnClickedCrGeneratemesh)
	ON_BN_CLICKED(IDC_MOD_ALGO_TJ, &CCPCreation::OnBnClickedModAlgoTj)
	ON_BN_CLICKED(IDC_MOD_ALGO_PROG, &CCPCreation::OnBnClickedModAlgoProg)
	ON_CBN_DROPDOWN(IDC_CR_COMBO_SINGLEPOLY, &CCPCreation::OnCbnDropdownCrComboSinglepoly)
	ON_CBN_SELENDOK(IDC_CR_COMBO_SINGLEPOLY, &CCPCreation::OnCbnSelendokCrComboSinglepoly)
	ON_CBN_DROPDOWN(IDC_CR_COMBO_SINGLESS, &CCPCreation::OnCbnDropdownCrComboSingless)
	ON_CBN_SELENDOK(IDC_CR_COMBO_SINGLESS, &CCPCreation::OnCbnSelendokCrComboSingless)
	ON_CBN_DROPDOWN(IDC_CR_COMBO_UNIONPOLY, &CCPCreation::OnCbnDropdownCrComboUnionpoly)
	ON_CBN_SELENDOK(IDC_CR_COMBO_UNIONPOLY, &CCPCreation::OnCbnSelendokCrComboUnionpoly)
END_MESSAGE_MAP()


// CCPCreation message handlers
void CCPCreation::Init()
{
	//render ref plane in creation mode
	if (pDoc->GetMeshCreation().GetRenderRefPlane()[0])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_BLUEPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_BLUEPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshCreation().GetRenderRefPlane()[1])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_GREENPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_GREENPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshCreation().GetRenderRefPlane()[2])
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_REDPLANE);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_REDPLANE);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshCreation().GetRenderCN())
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_CN);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_CN);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshCreation().GetRenderOnlyUserSketch())
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_US);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_US);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	if (pDoc->GetMeshCreation().GetCS2Surf()!=NULL)
	{
		//which subspace to render
		CR_SINGLESS.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_SINGLESS.AddString(_T(buffer));
		}
		CR_SINGLESS.AddString(_T("All"));
		CR_SINGLESS.AddString(_T("None"));
		CR_SINGLESS.SetCurSel(-1);
	}
	if (pDoc->GetMeshCreation().GetKW_CS2Surf()!=NULL)
	{
		//which single cylinder to render
		CR_SINGLEPOLY.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_SINGLEPOLY.AddString(_T(buffer));
		}
		CR_SINGLEPOLY.AddString(_T("All"));
		CR_SINGLEPOLY.AddString(_T("None"));
		CR_SINGLEPOLY.SetCurSel(-1);
		//which union cylinder to render
		CR_UNIONPOLY.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_UNIONPOLY.AddString(_T(buffer));
		}
		CR_UNIONPOLY.AddString(_T("All"));
		CR_UNIONPOLY.AddString(_T("None"));
		CR_UNIONPOLY.SetCurSel(-1);
		//which subspace to render
		CR_SINGLESS.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_SINGLESS.AddString(_T(buffer));
		}
		CR_SINGLESS.AddString(_T("All"));
		CR_SINGLESS.AddString(_T("None"));
		CR_SINGLESS.SetCurSel(-1);
	}
	if (pDoc->GetMeshCreation().GetAutoRotState())
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_AUTOROT);
		m_Check->SetCheck(BST_CHECKED);
	}
	else
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_AUTOROT);
		m_Check->SetCheck(BST_UNCHECKED);
	}
	//set view style
	this->iRadioModAlgo=pDoc->GetMeshCreation().GetSurfReconstAlgorithm();
	UpdateData(FALSE);
}

BOOL CCPCreation::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	WORD   wID   =   LOWORD(   wParam   );  
	WORD   wNF   =   HIWORD(   wParam   );  
	if(wID ==IDC_CR_BLUEPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_BLUEPLANE);
		int iState=m_Check->GetCheck();
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[0]=true;
		} 
		else
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[0]=false;
		}
	}  
	else if(wID ==IDC_CR_GREENPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_GREENPLANE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[1]=true;
		} 
		else
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[1]=false;
		}
	}
	else if(wID ==IDC_CR_REDPLANE && wNF == BN_CLICKED)  
	{  
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_REDPLANE);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[2]=true;
		} 
		else
		{
			pDoc->GetMeshCreation().GetRenderRefPlane()[2]=false;
		}
	}
	else if (wID ==IDC_CR_CN && wNF == BN_CLICKED)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_CN);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().SetRenderCN(true);
		} 
		else
		{
			pDoc->GetMeshCreation().SetRenderCN(false);
		}
	}
	else if (wID ==IDC_CR_US && wNF == BN_CLICKED)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_US);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().SetRenderOnlyUserSketch(true);
		} 
		else
		{
			pDoc->GetMeshCreation().SetRenderOnlyUserSketch(false);
		}
	}
	else if (wID ==IDC_CR_AUTOROT && wNF == BN_CLICKED)
	{
		CButton*   m_Check=(CButton*)this->GetDlgItem(IDC_CR_AUTOROT);
		if (m_Check->GetCheck()==BST_CHECKED)
		{
			pDoc->GetMeshCreation().SetAutoRotState(true);
		} 
		else
		{
			pDoc->GetMeshCreation().SetAutoRotState(false);
		}
	}
	pDoc->UpdateAllViews((CView*)pCP);
	return CDialog::OnCommand(wParam, lParam);
}

void CCPCreation::OnBnClickedCrReadcontour()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_CR_ReadContour);
	pButton->EnableWindow(FALSE);

	CFileDialog dlg(TRUE,".contour","*.contour",OFN_HIDEREADONLY,
		"Contour Files(*.CONTOUR)|*.contour||",AfxGetMainWnd());   
	if(dlg.DoModal() == IDOK)         
	{
		BeginWaitCursor();
		CString strPath = dlg.GetPathName();       

		char* pFileName=new char[MAX_PATH];
		memset(pFileName,0x00,MAX_PATH);
		memcpy(pFileName,strPath,strPath.GetLength()+1);
		pDoc->GetMeshCreation().ReadContourFromFile(pFileName);
		EndWaitCursor();
	}

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPCreation::OnBnClickedCrWritecontour()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_CR_WriteContour);
	pButton->EnableWindow(FALSE);


	CFileDialog dlg(FALSE,".contour","*.contour",OFN_HIDEREADONLY,
		"Contour Files(*.CONTOUR)|*.contour||",AfxGetMainWnd());   
	if(dlg.DoModal() == IDOK)         
	{
		BeginWaitCursor();
		CString strPath = dlg.GetPathName();       

		char* pFileName=new char[MAX_PATH];
		memset(pFileName,0x00,MAX_PATH);
		memcpy(pFileName,strPath,strPath.GetLength()+1);
		pDoc->GetMeshCreation().WriteContourToFile(pFileName);
		EndWaitCursor();
	}

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPCreation::OnBnClickedCrAdjustcontourview()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_CR_AdjustContourView);
	pButton->EnableWindow(FALSE);

	if (!pDoc->GetMesh().empty())
	{
		return;
	}
	pDoc->GetMeshCreation().AdjustContourView();

	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPCreation::OnBnClickedCrGeneratemesh()
{
	// TODO: Add your control notification handler code here
	CWnd* pButton=this->GetDlgItem(IDC_CR_GenerateMesh);
	pButton->EnableWindow(FALSE);

//	pDoc->GetMeshCreation().GenerateMesh(pDoc->GetMesh(),pDoc->GetDefaultColor());
	pDoc->SetTitle("Untitled.obj");
	pDoc->SetModifiedFlag(TRUE);	


	//disable the preview checkbox
	pDoc->SetRenderPreMesh(MESH_EXIST_VIEW);
	//CWnd*   m_Check=(CCPGeneral*)(pCP->GetCPGeneral())->GetDlgItem(IDC_CR_PREMESH);
	//m_Check->EnableWindow(FALSE);
	
	//do not display the reference planes
	CButton*   m_CheckButton=(CButton*)this->GetDlgItem(IDC_CR_BLUEPLANE);
	m_CheckButton->SetCheck(BST_UNCHECKED);
	pDoc->GetMeshCreation().GetRenderRefPlane()[0]=false;
	m_CheckButton=(CButton*)this->GetDlgItem(IDC_CR_REDPLANE);
	m_CheckButton->SetCheck(BST_UNCHECKED);
	pDoc->GetMeshCreation().GetRenderRefPlane()[1]=false;
	m_CheckButton=(CButton*)this->GetDlgItem(IDC_CR_GREENPLANE);
	m_CheckButton->SetCheck(BST_UNCHECKED);
	pDoc->GetMeshCreation().GetRenderRefPlane()[2]=false;



	pDoc->UpdateAllViews((CView*)pCP);
	pButton->EnableWindow(TRUE);
}

void CCPCreation::OnBnClickedModAlgoTj()
{
	// TODO: Add your control notification handler code here
	pDoc->GetMeshCreation().SetSurfReconstAlgorithm(TaoJuSurfReconstAlgo);
	iRadioModAlgo=TaoJuSurfReconstAlgo;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPCreation::OnBnClickedModAlgoProg()
{
	// TODO: Add your control notification handler code here
	pDoc->GetMeshCreation().SetSurfReconstAlgorithm(ProgSurfReconstAlgo);
	iRadioModAlgo=ProgSurfReconstAlgo;
	UpdateData(FALSE);
	pDoc->UpdateAllViews(NULL);
}

void CCPCreation::OnCbnDropdownCrComboSinglepoly()
{
	// TODO: Add your control notification handler code here
	//which cylinder to render
	CR_SINGLEPOLY.ResetContent();
	for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum();i++)
	{
		char buffer[65];
		_itoa_s( i,buffer,65,10);
		CR_SINGLEPOLY.AddString(_T(buffer));
	}
	CR_SINGLEPOLY.AddString(_T("All"));
	CR_SINGLEPOLY.AddString(_T("None"));
	//CR_SINGLEPOLY.SetCurSel(-1);

	//which cylinder has been selected
	if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSinglePolyIndex()==CR_RENDER_NONE_SINGLE_CYLINDER)
	{
		CR_SINGLEPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum()+1);
	}
	else if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSinglePolyIndex()==CR_RENDER_ALL_SINGLE_CYLINDER)
	{
		CR_SINGLEPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum());
	}
	else
	{
		CR_SINGLEPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSinglePolyIndex());
	}
}

void CCPCreation::OnCbnSelendokCrComboSinglepoly()
{
	// TODO: Add your control notification handler code here
	if (this->CR_SINGLEPOLY.GetCurSel()<=pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum()-1)
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSinglePolyIndex(this->CR_SINGLEPOLY.GetCurSel());
	}
	else if (this->CR_SINGLEPOLY.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum())
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSinglePolyIndex(CR_RENDER_ALL_SINGLE_CYLINDER);
	}
	else if (this->CR_SINGLEPOLY.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetCSCylinderNum()+1)
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSinglePolyIndex(CR_RENDER_NONE_SINGLE_CYLINDER);
	}
	pDoc->UpdateAllViews((CView*)pCP);
}


void CCPCreation::OnCbnDropdownCrComboSingless()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetMeshCreation().GetSurfReconstAlgorithm()==TaoJuSurfReconstAlgo
		&& pDoc->GetMeshCreation().GetCS2Surf()!=NULL)
	{
		//which subspace to render
		CR_SINGLESS.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_SINGLESS.AddString(_T(buffer));
		}
		CR_SINGLESS.AddString(_T("All"));
		CR_SINGLESS.AddString(_T("None"));
		//CR_SINGLESS.SetCurSel(-1);

		//which subspace has been selected
		if (pDoc->GetMeshCreation().GetCS2Surf()->GetRenderSingleSSIndex()==CR_RENDER_NONE_SS)
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum()+1);
		}
		else if (pDoc->GetMeshCreation().GetCS2Surf()->GetRenderSingleSSIndex()==CR_RENDER_ALL_SS)
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum());
		}
		else
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetCS2Surf()->GetRenderSingleSSIndex());
		}
	}
	else if (pDoc->GetMeshCreation().GetSurfReconstAlgorithm()==ProgSurfReconstAlgo
		&& pDoc->GetMeshCreation().GetKW_CS2Surf()!=NULL)
	{
		//which subspace to render
		CR_SINGLESS.ResetContent();
		for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum();i++)
		{
			char buffer[65];
			_itoa_s( i,buffer,65,10);
			CR_SINGLESS.AddString(_T(buffer));
		}
		CR_SINGLESS.AddString(_T("All"));
		CR_SINGLESS.AddString(_T("None"));
		//CR_SINGLESS.SetCurSel(-1);

		//which subspace has been selected
		if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSingleSSIndex()==CR_RENDER_NONE_SS)
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum()+1);
		}
		else if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSingleSSIndex()==CR_RENDER_ALL_SS)
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum());
		}
		else
		{
			CR_SINGLESS.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderSingleSSIndex());
		}
	}
}

void CCPCreation::OnCbnSelendokCrComboSingless()
{
	// TODO: Add your control notification handler code here
	if (pDoc->GetMeshCreation().GetSurfReconstAlgorithm()==TaoJuSurfReconstAlgo
		&& pDoc->GetMeshCreation().GetCS2Surf()!=NULL)
	{
		if (this->CR_SINGLESS.GetCurSel()<=pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum()-1)
		{
			pDoc->GetMeshCreation().GetCS2Surf()->SetRenderSingleSSIndex(this->CR_SINGLESS.GetCurSel());
		}
		else if (this->CR_SINGLESS.GetCurSel()==pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum())
		{
			pDoc->GetMeshCreation().GetCS2Surf()->SetRenderSingleSSIndex(CR_RENDER_ALL_SS);
		}
		else if (this->CR_SINGLESS.GetCurSel()==pDoc->GetMeshCreation().GetCS2Surf()->GetSSNum()+1)
		{
			pDoc->GetMeshCreation().GetCS2Surf()->SetRenderSingleSSIndex(CR_RENDER_NONE_SS);
		}
	}
	else if (pDoc->GetMeshCreation().GetSurfReconstAlgorithm()==ProgSurfReconstAlgo
		&& pDoc->GetMeshCreation().GetKW_CS2Surf()!=NULL)
	{
		if (this->CR_SINGLESS.GetCurSel()<=pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum()-1)
		{
			pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSingleSSIndex(this->CR_SINGLESS.GetCurSel());
		}
		else if (this->CR_SINGLESS.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum())
		{
			pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSingleSSIndex(CR_RENDER_ALL_SS);
		}
		else if (this->CR_SINGLESS.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetSSNum()+1)
		{
			pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderSingleSSIndex(CR_RENDER_NONE_SS);
		}
	}
	pDoc->UpdateAllViews((CView*)pCP);
}

void CCPCreation::OnCbnDropdownCrComboUnionpoly()
{
	// TODO: Add your control notification handler code here
	//which cylinder to render
	CR_UNIONPOLY.ResetContent();
	for (int i=0;i<pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum();i++)
	{
		char buffer[65];
		_itoa_s( i,buffer,65,10);
		CR_UNIONPOLY.AddString(_T(buffer));
	}
	CR_UNIONPOLY.AddString(_T("All"));
	CR_UNIONPOLY.AddString(_T("None"));
	//CR_UNIONPOLY.SetCurSel(-1);

	//which cylinder has been selected
	if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderUNPolyIndex()==CR_RENDER_NONE_UNION_CYLINDER)
	{
		CR_UNIONPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum()+1);
	}
	else if (pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderUNPolyIndex()==CR_RENDER_ALL_UNION_CYLINDER)
	{
		CR_UNIONPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum());
	}
	else
	{
		CR_UNIONPOLY.SetCurSel(pDoc->GetMeshCreation().GetKW_CS2Surf()->GetRenderUNPolyIndex());
	}
}

void CCPCreation::OnCbnSelendokCrComboUnionpoly()
{
	// TODO: Add your control notification handler code here
	if (this->CR_UNIONPOLY.GetCurSel()<=pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum()-1)
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderUNPolyIndex(this->CR_UNIONPOLY.GetCurSel());
	}
	else if (this->CR_UNIONPOLY.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum())
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderUNPolyIndex(CR_RENDER_ALL_UNION_CYLINDER);
	}
	else if (this->CR_UNIONPOLY.GetCurSel()==pDoc->GetMeshCreation().GetKW_CS2Surf()->GetUNCylinderNum()+1)
	{
		pDoc->GetMeshCreation().GetKW_CS2Surf()->SetRenderUNPolyIndex(CR_RENDER_NONE_UNION_CYLINDER);
	}
	pDoc->UpdateAllViews((CView*)pCP);
}
