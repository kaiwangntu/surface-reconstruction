#pragma once
#ifndef HERMITE_RBF_H
#define HERMITE_RBF_H

#include "../../GeometryAlgorithm.h"
#include "polygonizer.h"

class HermiteRBF
{
public:

	friend class MarCubHRBF;

	HermiteRBF(void);
	~HermiteRBF(void);

	//compute the coefficients for rbf
	bool ComputeHRBF(vector<Point_3> InterpoPoints,vector<Vector_3> InterpoNorm,
		vector<double>& WeightsAlpha,vector<vector<double>>& WeightsBeta,vector<double>& PolyNomA,double& PolyNomB);

	//copy data from ImplicitMesher to static members of this class
	static void CopyHRBF(vector<double> WeightsAlphaIn,vector<vector<double>> WeightsBetaIn,vector<double> PolyNomAIn,double PolyNomBIn,
		vector<Point_3> InterpoPointsIn);

	//static function pointer, useful for converting implicit surface to mesh in ImplicitMesher
	static SMDT3GTFT HRBF(SMDT3GTPoint_3 p);


protected:

	//precompute CubicBasisFunc,gradient and hessian between each pair of points
	//to avoid duplicated computations
	void PreCompute(vector<Point_3> InterpoPoints,vector<vector<double>>& CBF,vector<vector<GeneralMatrix>>& GradCBF,
		vector<vector<GeneralMatrix>>& HesCBF);
	//build the matrix to solve the linear system to compute the coefficients
	void BuildLeftHandMatix(vector<Point_3> InterpoPoints,vector<vector<double>>& LeftHandMatrix);
	//get the right hand vector of the linear system
	void BuildRightHandVector(vector<Point_3> InterpoPoints,vector<Vector_3> InterpoNorm,vector<double>& RightHandVec);

	//return the value of cubic basis function ||Xi-Xj||^3
	static double CubicBasisFunc(Point_3 Pi,Point_3 Pj);
	static double CubicBasisFunc(SMDT3GTPoint_3 Pi,Point_3 Pj);
	//return the value of the gradient of cubic basis function
	static vector<double> GradCubicBasisFunc(Point_3 Pi,Point_3 Pj);
	static vector<double> GradCubicBasisFunc(SMDT3GTPoint_3 Pi,Point_3 Pj);
	//return the value of the Hessian of cubic basis function
	static vector<vector<double>> HesCubicBasisFunc(Point_3 Pi,Point_3 Pj);

	//static data for Hermite RBF
	static vector<double> WeightsAlpha;//num=j
	static vector<vector<double>> WeightsBeta;//num=3j
	static vector<double> PolyNomA;//num=3
	static double PolyNomB;//num=1
	static vector<Point_3> InterpoPoints;
};

//designed for the marching cube method of class Polygonizer
class MarCubHRBF: public ImplicitFunction
{
	float eval (float x, float y, float z);
};


#endif
