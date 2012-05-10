#include "StdAfx.h"
#include "RadialBasisFunc.h"

vector<double> RadialBasisFunc::RBFWeights(0);
vector<double> RadialBasisFunc::RBFPolyNom(0);
vector<Point_3> RadialBasisFunc::InterpoPoints(0);

RadialBasisFunc::RadialBasisFunc(void)
{
	this->iBasisFuncType=BASIS_FUNC_CUBIC;
}

RadialBasisFunc::~RadialBasisFunc(void)
{
}

bool RadialBasisFunc::ComputeRBF(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints,
								 vector<Point_3> ConstrPoints,vector<double>& Weights,vector<double>& Poly)
{
	vector<vector<double>> LeftHandMatrix;
	BuildLeftHandMatix(InterpoPoints,PosNormalPoints,NegNormalPoints,ConstrPoints,LeftHandMatrix);
	vector<double> RightHandVec;
	BuildRightHandVector(InterpoPoints,PosNormalPoints,NegNormalPoints,ConstrPoints,RightHandVec);

	//solve linear equation
	vector<vector<double> > RightHandSide;
	RightHandSide.push_back(RightHandVec);
	vector<vector<double> > Result;
	bool bResult=CMath::ComputeLSE(LeftHandMatrix,RightHandSide,Result);
	if (bResult)
	{
		Weights.insert(Weights.end(),Result.front().begin(),Result.front().begin()+InterpoPoints.size());
		Poly.insert(Poly.end(),Result.front().begin()+InterpoPoints.size(),Result.front().end());
	}
	return bResult;
}


double RadialBasisFunc::CubicBasisFunc(Point_3 Xi,Point_3 Xj)
{
	double dDiff=sqrt(CGAL::squared_distance(Xi,Xj));
	dDiff=pow(dDiff,3);
	return dDiff;
}

double RadialBasisFunc::CubicBasisFunc(SMDT3GTPoint_3 Xi,Point_3 Xj)
{
	double dDiff=sqrt((Xi.x()-Xj.x())*(Xi.x()-Xj.x())+(Xi.y()-Xj.y())*(Xi.y()-Xj.y())+
		(Xi.z()-Xj.z())*(Xi.z()-Xj.z()));
	dDiff=pow(dDiff,3);
	return dDiff;
}

void RadialBasisFunc::BuildLeftHandMatix(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints, 
										 vector<Point_3> ConstrPoints,vector<vector<double>>& LeftHandMatrix)
{
	//for all interpolation points
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		vector<double> CurrentRow;
		//subpart of the radial basis function
		for (unsigned int j=0;j<InterpoPoints.size();j++)
		{
			if (j==i)
			{
				CurrentRow.push_back(0);
			}
			else if (j<i)//since the submatrix is symmetric,no need to compute
			{
				CurrentRow.push_back(LeftHandMatrix.at(j).at(i));
			}
			else//j>i,compute
			{
				switch(this->iBasisFuncType)
				{
				case BASIS_FUNC_CUBIC:
					CurrentRow.push_back(CubicBasisFunc(InterpoPoints.at(i),InterpoPoints.at(j)));
					break;
				default:
					break;
				}
			}
		}
		//subpart of the degree one polynomial
		CurrentRow.push_back(1);
		CurrentRow.push_back(InterpoPoints.at(i).x());
		CurrentRow.push_back(InterpoPoints.at(i).y());
		CurrentRow.push_back(InterpoPoints.at(i).z());

		LeftHandMatrix.push_back(CurrentRow);
	}
	//for all outside normal points
	for (unsigned int i=0;i<PosNormalPoints.size();i++)
	{
		vector<double> CurrentRow;
		//subpart of the radial basis function
		for (unsigned int j=0;j<InterpoPoints.size();j++)
		{
			CurrentRow.push_back(CubicBasisFunc(PosNormalPoints.at(i),InterpoPoints.at(j)));
		}
		//subpart of the degree one polynomial
		CurrentRow.push_back(1);
		CurrentRow.push_back(PosNormalPoints.at(i).x());
		CurrentRow.push_back(PosNormalPoints.at(i).y());
		CurrentRow.push_back(PosNormalPoints.at(i).z());

		LeftHandMatrix.push_back(CurrentRow);
	}
	//for all inside normal points
	for (unsigned int i=0;i<NegNormalPoints.size();i++)
	{
		vector<double> CurrentRow;
		//subpart of the radial basis function
		for (unsigned int j=0;j<InterpoPoints.size();j++)
		{
			CurrentRow.push_back(CubicBasisFunc(NegNormalPoints.at(i),InterpoPoints.at(j)));
		}
		//subpart of the degree one polynomial
		CurrentRow.push_back(1);
		CurrentRow.push_back(NegNormalPoints.at(i).x());
		CurrentRow.push_back(NegNormalPoints.at(i).y());
		CurrentRow.push_back(NegNormalPoints.at(i).z());

		LeftHandMatrix.push_back(CurrentRow);
	}
	//constraint points
	//...
	//the sum of all weights is 0
	vector<double> SumWeights;
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		SumWeights.push_back(1);
	}
	for (int i=0;i<4;i++)
	{
		SumWeights.push_back(0);
	}
	LeftHandMatrix.push_back(SumWeights);
	//the sum of multiplications of each weight and corresponding x(y,z) is 0
	vector<double> SumMultipX,SumMultipY,SumMultipZ;
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		SumMultipX.push_back(InterpoPoints.at(i).x());
		SumMultipY.push_back(InterpoPoints.at(i).y());
		SumMultipZ.push_back(InterpoPoints.at(i).z());
	}
	for (int i=0;i<4;i++)
	{
		SumMultipX.push_back(0);
		SumMultipY.push_back(0);
		SumMultipZ.push_back(0);
	}
	LeftHandMatrix.push_back(SumMultipX);
	LeftHandMatrix.push_back(SumMultipY);
	LeftHandMatrix.push_back(SumMultipZ);
}

void RadialBasisFunc::BuildRightHandVector(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints, 
										   vector<Point_3> ConstrPoints,vector<double>& RightHandVec)
{
	//interpolation points, equal to 0
	for (unsigned int i=0;i<InterpoPoints.size();i++)
	{
		RightHandVec.push_back(0);
	}
	//normal points:1 for outside and -1 for inside
	for (unsigned int i=0;i<PosNormalPoints.size();i++)
	{
		RightHandVec.push_back(1);
	}
	for (unsigned int i=0;i<NegNormalPoints.size();i++)
	{
		RightHandVec.push_back(-1);
	}
	//the sum of all weights is 0
	RightHandVec.push_back(0);
	//the sum of multiplications of each weight and corresponding x(y,z) is 0
	for (int i=0;i<3;i++)
	{
		RightHandVec.push_back(0);
	}
	//constraint points: 1
	for (unsigned int i=0;i<ConstrPoints.size();i++)
	{
		RightHandVec.push_back(0);
	}
}

SMDT3GTFT RadialBasisFunc::RadialBasisFunction(SMDT3GTPoint_3 p)
{
	double dResult=0;
	for (unsigned int i=0;i<RBFWeights.size();i++)
	{
		dResult+=RBFWeights.at(i)*CubicBasisFunc(p,InterpoPoints.at(i));
	}
	dResult=dResult+RBFPolyNom[0]+RBFPolyNom[1]*p.x()+RBFPolyNom[2]*p.y()+RBFPolyNom[3]*p.z();
	
	return dResult;
}

void RadialBasisFunc::CopyRadialBasisFunction(vector<double> InputWeights,vector<double> InputPolyNom,vector<Point_3> InputInterpoPoints)
{
	RadialBasisFunc::RBFWeights=InputWeights;
	RadialBasisFunc::RBFPolyNom=InputPolyNom;
	RadialBasisFunc::InterpoPoints=InputInterpoPoints;
}

float MarCubRBF::eval(float x, float y, float z)
{
	Point_3 p(x,y,z);
	double dResult=0;
	for (unsigned int i=0;i<RadialBasisFunc::RBFWeights.size();i++)
	{
		dResult+=RadialBasisFunc::RBFWeights.at(i)*RadialBasisFunc::CubicBasisFunc(p,RadialBasisFunc::InterpoPoints.at(i));
	}
	dResult=dResult+RadialBasisFunc::RBFPolyNom[0]+RadialBasisFunc::RBFPolyNom[1]*p.x()
		+RadialBasisFunc::RBFPolyNom[2]*p.y()+RadialBasisFunc::RBFPolyNom[3]*p.z();
	return dResult;
}


