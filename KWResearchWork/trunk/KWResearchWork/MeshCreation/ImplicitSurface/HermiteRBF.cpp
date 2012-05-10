#include "StdAfx.h"
#include "HermiteRBF.h"

vector<double> HermiteRBF::WeightsAlpha(0);
vector<vector<double>> HermiteRBF::WeightsBeta(0);
vector<double> HermiteRBF::PolyNomA(0);
double HermiteRBF::PolyNomB(0);
vector<Point_3> HermiteRBF::InterpoPoints(0);

HermiteRBF::HermiteRBF(void)
{
}

HermiteRBF::~HermiteRBF(void)
{
}

bool HermiteRBF::ComputeHRBF(vector<Point_3> InterpoPoints,vector<Vector_3> InterpoNorm, vector<double>& WeightsAlpha,
							 vector<vector<double>>& WeightsBeta,vector<double>& PolyNomA,double& PolyNomB)
{
	vector<vector<double>> LeftHandMatrix;
	BuildLeftHandMatix(InterpoPoints,LeftHandMatrix);

	vector<double> RightHandVec;
	BuildRightHandVector(InterpoPoints,InterpoNorm,RightHandVec);

	//solve linear equation
	vector<vector<double> > RightHandSide;
	RightHandSide.push_back(RightHandVec);
	vector<vector<double> > Result;
	bool bResult=CMath::ComputeLSE(LeftHandMatrix,RightHandSide,Result);
	if (bResult)
	{
		//insert weights and polynomial
		for (unsigned int i=0;i<InterpoPoints.size();i++)
		{
			WeightsAlpha.push_back(Result.front().at(4*i));
			vector<double> CurrentBeta;
			CurrentBeta.push_back(Result.front().at(4*i+1));
			CurrentBeta.push_back(Result.front().at(4*i+2));
			CurrentBeta.push_back(Result.front().at(4*i+3));
			WeightsBeta.push_back(CurrentBeta);
		}
		PolyNomA.push_back(Result.front().at(Result.front().size()-4));
		PolyNomA.push_back(Result.front().at(Result.front().size()-3));
		PolyNomA.push_back(Result.front().at(Result.front().size()-2));
		PolyNomB=Result.front().back();
	}
	return bResult;
}

void HermiteRBF::BuildLeftHandMatix(vector<Point_3> InterpoPoints,vector<vector<double>>& LeftHandMatrix)
{
	//compute CubicBasisFunc,gradient and hessian between each pair of points
	vector<vector<double>> CBF;
	vector<vector<GeneralMatrix>> GradCBF;
	vector<vector<GeneralMatrix>> HesCBF;
	PreCompute(InterpoPoints,CBF,GradCBF,HesCBF);

	vector<double> ParaRow0,ParaRow1,ParaRow2,ParaRow3;//extra constraints on parameters
	//build left hand matrix
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		vector<double> FmGraRow;//f(i,j)-grad f(i,j)
		vector<double> GramHesRow[3];//grad f(i,j)-hessian f(i,j)
		for (unsigned int j=0;j<InterpoPoints.size();j++)
		{
			//f(i,j)-grad f(i,j)
			FmGraRow.push_back(CBF.at(i).at(j));
			FmGraRow.push_back(-GradCBF.at(i).at(j)[0][0]);
			FmGraRow.push_back(-GradCBF.at(i).at(j)[1][0]);
			FmGraRow.push_back(-GradCBF.at(i).at(j)[2][0]);
			//grad f(i,j)-hessian f(i,j)
			for (int k=0;k<3;k++)
			{
				GramHesRow[k].push_back(GradCBF.at(i).at(j)[k][0]);
				GramHesRow[k].push_back(-HesCBF.at(i).at(j)[k][0]);
				GramHesRow[k].push_back(-HesCBF.at(i).at(j)[k][1]);
				GramHesRow[k].push_back(-HesCBF.at(i).at(j)[k][2]);
			}
		}
		//polynomial part
		FmGraRow.push_back(InterpoPoints.at(i).x());
		FmGraRow.push_back(InterpoPoints.at(i).y());
		FmGraRow.push_back(InterpoPoints.at(i).z());
		FmGraRow.push_back(1);
		GramHesRow[0].push_back(1);GramHesRow[0].push_back(0);GramHesRow[0].push_back(0);GramHesRow[0].push_back(0);
		GramHesRow[1].push_back(0);GramHesRow[1].push_back(1);GramHesRow[1].push_back(0);GramHesRow[1].push_back(0);
		GramHesRow[2].push_back(0);GramHesRow[2].push_back(0);GramHesRow[2].push_back(1);GramHesRow[2].push_back(0);

		//extra parameter rows
		ParaRow0.push_back(1);ParaRow0.push_back(0);ParaRow0.push_back(0);ParaRow0.push_back(0);
		ParaRow1.push_back(InterpoPoints.at(i).x());
		ParaRow1.push_back(1);
		ParaRow1.push_back(0);
		ParaRow1.push_back(0);
		ParaRow2.push_back(InterpoPoints.at(i).y());
		ParaRow2.push_back(0);
		ParaRow2.push_back(1);
		ParaRow2.push_back(0);
		ParaRow3.push_back(InterpoPoints.at(i).z());
		ParaRow3.push_back(0);
		ParaRow3.push_back(0);
		ParaRow3.push_back(1);

		LeftHandMatrix.push_back(FmGraRow);
		LeftHandMatrix.push_back(GramHesRow[0]);
		LeftHandMatrix.push_back(GramHesRow[1]);
		LeftHandMatrix.push_back(GramHesRow[2]);
	}
	for (int i=0;i<4;i++)
	{
		ParaRow0.push_back(0);
		ParaRow1.push_back(0);
		ParaRow2.push_back(0);
		ParaRow3.push_back(0);
	}
	LeftHandMatrix.push_back(ParaRow0);
	LeftHandMatrix.push_back(ParaRow1);
	LeftHandMatrix.push_back(ParaRow2);
	LeftHandMatrix.push_back(ParaRow3);
}

void HermiteRBF::PreCompute(vector<Point_3> InterpoPoints,vector<vector<double>>& CBF,
							vector<vector<GeneralMatrix>>& GradCBF, vector<vector<GeneralMatrix>>& HesCBF)
{
	//built 3 n*n matrix
	//matrix[i][j] stores the cubic basis function,its gradient and its hessian between point i and point j
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		vector<double> CBFRow;
		vector<GeneralMatrix> GradCBFRow,HesCBFRow;
		for (unsigned int j=0;j<InterpoPoints.size();j++)
		{
			if (j<i)//don't compute, just copy from calculated result
			{
				//cubic basis function f(i,j)=f(j,i)
				CBFRow.push_back(CBF.at(j).at(i));
				//its gradient Grad f(i,j)=-Grad f(j,i)
				GradCBFRow.push_back(-GradCBF.at(j).at(i));
				//its hessian Hessian f(i,j)=Hessian f(j,i)
				HesCBFRow.push_back(HesCBF.at(j).at(i));
			}
			else if (j>=i)
			{
				CBFRow.push_back(CubicBasisFunc(InterpoPoints.at(i),InterpoPoints.at(j)));
				
				vector<double> CurrentGrad=GradCubicBasisFunc(InterpoPoints.at(i),InterpoPoints.at(j));
				GeneralMatrix GradMat(3,1);
				GradMat[0][0]=CurrentGrad.at(0);
				GradMat[1][0]=CurrentGrad.at(1);
				GradMat[2][0]=CurrentGrad.at(2);
				GradCBFRow.push_back(GradMat);
				
				vector<vector<double>> CurrentHes=HesCubicBasisFunc(InterpoPoints.at(i),InterpoPoints.at(j));
				GeneralMatrix HesMat(3,3);
				HesMat[0][0]=CurrentHes.at(0).at(0);HesMat[0][1]=CurrentHes.at(0).at(1);HesMat[0][2]=CurrentHes.at(0).at(2);
				HesMat[1][0]=CurrentHes.at(1).at(0);HesMat[1][1]=CurrentHes.at(1).at(1);HesMat[1][2]=CurrentHes.at(1).at(2);
				HesMat[2][0]=CurrentHes.at(2).at(0);HesMat[2][1]=CurrentHes.at(2).at(1);HesMat[2][2]=CurrentHes.at(2).at(2);
				HesCBFRow.push_back(HesMat);
			}
		}
		CBF.push_back(CBFRow);
		GradCBF.push_back(GradCBFRow);
		HesCBF.push_back(HesCBFRow);
	}
}

void HermiteRBF::BuildRightHandVector(vector<Point_3> InterpoPoints,vector<Vector_3> InterpoNorm,vector<double>& RightHandVec)
{
	assert(InterpoPoints.size()==InterpoNorm.size());
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		RightHandVec.push_back(0);
		RightHandVec.push_back(InterpoNorm.at(i).x());
		RightHandVec.push_back(InterpoNorm.at(i).y());
		RightHandVec.push_back(InterpoNorm.at(i).z());
	}
	for (int i=0;i<4;i++)
	{
		RightHandVec.push_back(0);
	}
}

void HermiteRBF::CopyHRBF(vector<double> WeightsAlphaIn,vector<vector<double>> WeightsBetaIn,
						  vector<double> PolyNomAIn,double PolyNomBIn, vector<Point_3> InterpoPointsIn)
{
	WeightsAlpha=WeightsAlphaIn;
	WeightsBeta=WeightsBetaIn;
	PolyNomA=PolyNomAIn;
	PolyNomB=PolyNomBIn;
	InterpoPoints=InterpoPointsIn;
}

double HermiteRBF::CubicBasisFunc(Point_3 Pi,Point_3 Pj)
{
	if (Pi==Pj)
	{
		return 0;
	}
	double dDiff=sqrt((Pi.x()-Pj.x())*(Pi.x()-Pj.x())+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())+
		(Pi.z()-Pj.z())*(Pi.z()-Pj.z()));
	dDiff=pow(dDiff,3);
	return dDiff;
}

double HermiteRBF::CubicBasisFunc(SMDT3GTPoint_3 Pi,Point_3 Pj)
{
	if (Pi.x()==Pj.x() && Pi.y()==Pj.y() && Pi.z()==Pj.z())
	{
		return 0;
	}
	double dDiff=sqrt((Pi.x()-Pj.x())*(Pi.x()-Pj.x())+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())+
		(Pi.z()-Pj.z())*(Pi.z()-Pj.z()));
	dDiff=pow(dDiff,3);
	return dDiff;
}

vector<double> HermiteRBF::GradCubicBasisFunc(Point_3 Pi,Point_3 Pj)
{
	vector<double> GradVec;
	if (Pi==Pj)
	{
		GradVec.insert(GradVec.begin(),3,0);
		return GradVec;
	}
	double dDiff=sqrt((Pi.x()-Pj.x())*(Pi.x()-Pj.x())+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())+
		(Pi.z()-Pj.z())*(Pi.z()-Pj.z()));
	GradVec.push_back(3*dDiff*(Pi.x()-Pj.x()));
	GradVec.push_back(3*dDiff*(Pi.y()-Pj.y()));
	GradVec.push_back(3*dDiff*(Pi.z()-Pj.z()));
	return GradVec;
}

vector<double> HermiteRBF::GradCubicBasisFunc(SMDT3GTPoint_3 Pi,Point_3 Pj)
{
	vector<double> GradVec;
	if (Pi.x()==Pj.x() && Pi.y()==Pj.y() && Pi.z()==Pj.z())
	{
		GradVec.insert(GradVec.begin(),3,0);
		return GradVec;
	}
	double dDiff=sqrt((Pi.x()-Pj.x())*(Pi.x()-Pj.x())+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())+
		(Pi.z()-Pj.z())*(Pi.z()-Pj.z()));
	GradVec.push_back(3*dDiff*(Pi.x()-Pj.x()));
	GradVec.push_back(3*dDiff*(Pi.y()-Pj.y()));
	GradVec.push_back(3*dDiff*(Pi.z()-Pj.z()));
	return GradVec;
}

vector<vector<double>> HermiteRBF::HesCubicBasisFunc(Point_3 Pi,Point_3 Pj)
{
	vector<vector<double>> HessianMat;
	vector<double> Row0,Row1,Row2;
	if (Pi==Pj)
	{
		Row0.insert(Row0.begin(),3,0);
		Row1.insert(Row1.begin(),3,0);
		Row2.insert(Row2.begin(),3,0);
		HessianMat.push_back(Row0);
		HessianMat.push_back(Row1);
		HessianMat.push_back(Row2);
		return HessianMat;
	}
	double dDiff=sqrt((Pi.x()-Pj.x())*(Pi.x()-Pj.x())+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())+
		(Pi.z()-Pj.z())*(Pi.z()-Pj.z()));
	Row0.push_back(3*(dDiff+(Pi.x()-Pj.x())*(Pi.x()-Pj.x())/dDiff));
	Row0.push_back(3*(Pi.x()-Pj.x())*(Pi.y()-Pj.y())/dDiff);
	Row0.push_back(3*(Pi.x()-Pj.x())*(Pi.z()-Pj.z())/dDiff);
	Row1.push_back(Row0.at(1));
	Row1.push_back(3*(dDiff+(Pi.y()-Pj.y())*(Pi.y()-Pj.y())/dDiff));
	Row1.push_back(3*(Pi.y()-Pj.y())*(Pi.z()-Pj.z())/dDiff);
	Row2.push_back(Row0.at(2));
	Row2.push_back(Row1.at(2));
	Row2.push_back(3*(dDiff+(Pi.z()-Pj.z())*(Pi.z()-Pj.z())/dDiff));
	HessianMat.push_back(Row0);
	HessianMat.push_back(Row1);
	HessianMat.push_back(Row2);
	return HessianMat;
}

SMDT3GTFT HermiteRBF::HRBF(SMDT3GTPoint_3 p)
{
	double dResult=0;
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		dResult=dResult+WeightsAlpha.at(i)*CubicBasisFunc(p,InterpoPoints.at(i));
		vector<double> vecGrad=GradCubicBasisFunc(p,InterpoPoints.at(i));
		double dGradSum=vecGrad.at(0)*WeightsBeta.at(i).at(0)+
			vecGrad.at(1)*WeightsBeta.at(i).at(1)+
			vecGrad.at(2)*WeightsBeta.at(i).at(2);
		dResult=dResult-dGradSum;
	}
	dResult=dResult+PolyNomA.at(0)*p.x()+PolyNomA.at(1)*p.y()+PolyNomA.at(2)*p.z()+PolyNomB;
	return dResult;
}

float MarCubHRBF::eval(float x, float y, float z)
{
	Point_3 p(x,y,z);
	double dResult=0;
	for (unsigned int i=0;i<HermiteRBF::InterpoPoints.size();i++)
	{
		dResult=dResult+HermiteRBF::WeightsAlpha.at(i)*HermiteRBF::CubicBasisFunc(p,HermiteRBF::InterpoPoints.at(i));
		vector<double> vecGrad=HermiteRBF::GradCubicBasisFunc(p,HermiteRBF::InterpoPoints.at(i));
		double dGradSum=vecGrad.at(0)*HermiteRBF::WeightsBeta.at(i).at(0)+
			vecGrad.at(1)*HermiteRBF::WeightsBeta.at(i).at(1)+
			vecGrad.at(2)*HermiteRBF::WeightsBeta.at(i).at(2);
		dResult=dResult-dGradSum;
	}
	dResult=dResult+HermiteRBF::PolyNomA.at(0)*p.x()+HermiteRBF::PolyNomA.at(1)*p.y()
		+HermiteRBF::PolyNomA.at(2)*p.z()+HermiteRBF::PolyNomB;
	return dResult;
}
