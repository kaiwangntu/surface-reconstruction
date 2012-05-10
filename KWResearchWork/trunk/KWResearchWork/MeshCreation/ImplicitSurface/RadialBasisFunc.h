#pragma once
#ifndef RADIAL_BASIS_FUNCTION_H
#define RADIAL_BASIS_FUNCTION_H

#include "../../GeometryAlgorithm.h"
#include "polygonizer.h"

const int BASIS_FUNC_CUBIC=0;

class RadialBasisFunc
{
public:
	
	friend class MarCubRBF;
	
	RadialBasisFunc(void);
	~RadialBasisFunc(void);

	//compute the coefficients for rbf
	bool ComputeRBF(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints,
		vector<Point_3> ConstrPoints,vector<double>& Weights,vector<double>& Poly);

	//static function pointer, useful for converting implicit surface to mesh in ImplicitMesher
	static SMDT3GTFT RadialBasisFunction (SMDT3GTPoint_3 p);

	//copy data from ImplicitMesher to static members of this class
	static void CopyRadialBasisFunction(vector<double> InputWeights,vector<double> InputPolyNom,vector<Point_3> InputInterpoPoints);
	
protected:
	int iBasisFuncType;

	static double CubicBasisFunc(Point_3 Xi,Point_3 Xj);
	static double CubicBasisFunc(SMDT3GTPoint_3 Xi,Point_3 Xj);

	//build the matrix to solve the linear system to compute the coefficients
	void BuildLeftHandMatix(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints,
		vector<Point_3> ConstrPoints,vector<vector<double>>& LeftHandMatrix);
	//get the right hand vector of the linear system
	void BuildRightHandVector(vector<Point_3> InterpoPoints,vector<Point_3> PosNormalPoints,vector<Point_3> NegNormalPoints,
		vector<Point_3> ConstrPoints,vector<double>& RightHandVec);

	//static data for RadialBasisFunction
	static vector<double> RBFWeights,RBFPolyNom;
	static vector<Point_3> InterpoPoints;
};

//designed for the marching cube method of class Polygonizer
class MarCubRBF: public ImplicitFunction
{
	float eval (float x, float y, float z);
};


#endif