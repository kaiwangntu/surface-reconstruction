#include "StdAfx.h"
#include "ImplicitMesher.h"

ImplicitMesher::ImplicitMesher(void)
{
}

ImplicitMesher::~ImplicitMesher(void)
{
}

void ImplicitMesher::ContourToMesh(vector<CurveNetwork> vecCurveNetwork,KW_Mesh& Mesh,vector<Point_3>& vecTestPoint)
{
	ContourToImpSurf(vecCurveNetwork,vecTestPoint);

	ImpSurfToMeshMarCub(Mesh);

//	ImpPolyhedron ImpPoly;
//	ImpSurfToMeshJDBois(ImpPoly);
}

void ImplicitMesher::ContourToImpSurf(vector<CurveNetwork> vecCurveNetwork,vector<Point_3>& vecTestPoint)
{
	//collect interpolation points & compute their normals to get outside points
	this->InterpoPoints.clear();
	vector<Vector_3> InterpoNorms;
	double dOffSet=0.2;
	vector<Point_3> PosNormPoints;
	for (unsigned int i=0;i<vecCurveNetwork.size();i++)
	{
		for (unsigned int j=0;j<vecCurveNetwork.at(i).Profile3D.size();j++)
		{
			for (unsigned int k=0;k<vecCurveNetwork.at(i).Profile3D.at(j).size();k++)
			{
				this->InterpoPoints.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k));
				int iLastInd,iNextInd;
				iLastInd=(k-1+vecCurveNetwork.at(i).Profile3D.at(j).size())%vecCurveNetwork.at(i).Profile3D.at(j).size();
				iNextInd=(k+1)%vecCurveNetwork.at(i).Profile3D.at(j).size();
				//compute the normal at this point
				Vector_3 PlaneNorm=vecCurveNetwork.at(i).plane.orthogonal_vector();
				Vector_3 TanVec=vecCurveNetwork.at(i).Profile3D.at(j).at(iNextInd)-vecCurveNetwork.at(i).Profile3D.at(j).at(iLastInd);
				Vector_3 NormVec=CGAL::cross_product(TanVec,PlaneNorm);
				NormVec=NormVec/sqrt(NormVec.squared_length());
				InterpoNorms.push_back(NormVec);
				PosNormPoints.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+NormVec*dOffSet);
				vecTestPoint.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+NormVec*dOffSet);
				//vecTestPoint.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+PlaneNorm*dOffSet);
			}
		}
	}

	HermiteRBF HRBF;
	HRBF.ComputeHRBF(this->InterpoPoints,InterpoNorms,this->WeightsAlpha,this->WeightsBeta,this->PolyNomA,this->PolyNomB);

	////collect interpolation points & compute their normals to get outside points
	//double dOffSet=1;
	//this->InterpoPoints.clear();
	//vector<Point_3> PosNormPoints;
	//for (unsigned int i=0;i<vecCurveNetwork.size();i++)
	//{
	//	for (unsigned int j=0;j<vecCurveNetwork.at(i).Profile3D.size();j++)
	//	{
	//		for (unsigned int k=0;k<vecCurveNetwork.at(i).Profile3D.at(j).size();k++)
	//		{
	//			this->InterpoPoints.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k));
	//			int iLastInd,iNextInd;
	//			iLastInd=(k-1+vecCurveNetwork.at(i).Profile3D.at(j).size())%vecCurveNetwork.at(i).Profile3D.at(j).size();
	//			iNextInd=(k+1)%vecCurveNetwork.at(i).Profile3D.at(j).size();
	//			//compute the normal at this point
	//			Vector_3 PlaneNorm=vecCurveNetwork.at(i).plane.orthogonal_vector();
	//			Vector_3 TanVec=vecCurveNetwork.at(i).Profile3D.at(j).at(iNextInd)-vecCurveNetwork.at(i).Profile3D.at(j).at(iLastInd);
	//			Vector_3 NormVec=CGAL::cross_product(TanVec,PlaneNorm);
	//			NormVec=NormVec/sqrt(NormVec.squared_length());
	//			PosNormPoints.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+NormVec*dOffSet);
	//			vecTestPoint.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+NormVec*dOffSet);
	//			vecTestPoint.push_back(vecCurveNetwork.at(i).Profile3D.at(j).at(k)+PlaneNorm*dOffSet);
	//		}
	//	}
	//}

	//this->InterpoPoints.push_back(Point_3(0,0,1));
	//this->InterpoPoints.push_back(Point_3(0,0,-1));
	//PosNormPoints.push_back(Point_3(0,0,1.2));
	//PosNormPoints.push_back(Point_3(0,0,-1.2));

	//vector<Point_3> NegNormPoints,ConstraintPoints;

	//RadialBasisFunc RBF;
	//RBF.ComputeRBF(this->InterpoPoints,PosNormPoints,NegNormPoints,
	//	ConstraintPoints,this->Weights,this->PolyNom);
}

void ImplicitMesher::ImpSurfToMeshMarCub(KW_Mesh& Mesh)
{
//	//transfer data from inside of class to global
//	RadialBasisFunc::CopyRadialBasisFunction(this->Weights,this->PolyNom,this->InterpoPoints);
//
//	MarCubRBF McRbf;
//	Polygonizer pol(&McRbf,.05, 30);//.05, 30
//	pol.march(false, 0.,0.,0.);//dotet
////	pol.march(true, this->InterpoPoints.front().x(),this->InterpoPoints.front().y(),
////		this->InterpoPoints.front().z());//dotet

	//transfer data from inside of class to global
	HermiteRBF::CopyHRBF(this->WeightsAlpha,this->WeightsBeta,this->PolyNomA,this->PolyNomB,this->InterpoPoints);

	MarCubHRBF McHRbf;
	Polygonizer pol(&McHRbf,.05, 30);//.05, 30
	pol.march(false, 0.,0.,0.);//dotet
	//	pol.march(true, this->InterpoPoints.front().x(),this->InterpoPoints.front().y(),
	//		this->InterpoPoints.front().z());//dotet

	Convert_MarCubPoly_To_CGALPoly<HalfedgeDS> triangle(&pol);
	Mesh.delegate(triangle);
}

void ImplicitMesher::ImpSurfToMeshJDBois(ImpPolyhedron& ImpPoly)
{
	//transfer data from inside of class to global
	RadialBasisFunc::CopyRadialBasisFunction(this->Weights,this->PolyNom,this->InterpoPoints);

	SMDT3 tr;            // 3D-Delaunay triangulation
	C2T3 c2t3 (tr);   // 2D-complex in 3D-Delaunay triangulation
	// defining the surface
	ImpSurface_3 surface(&RadialBasisFunc::RadialBasisFunction,// pointer to function
		SMDT3GTSphere_3(CGAL::ORIGIN, 2.)); // bounding sphere
	// Note that "2." above is the *squared* radius of the bounding sphere!

	// defining meshing criteria
	CGAL::Surface_mesh_default_criteria_3<SMDT3> criteria(30.,  // angular bound
		0.1,  // radius bound
		0.1); // distance bound
	// meshing surface
	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());

	//std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";

	CGAL::output_surface_facets_to_polyhedron(c2t3,ImpPoly);

	std::ofstream out("out.off");
	CGAL::output_surface_facets_to_off (out, c2t3);
}
