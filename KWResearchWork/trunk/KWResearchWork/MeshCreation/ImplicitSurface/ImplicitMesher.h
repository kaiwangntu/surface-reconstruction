#pragma once
#ifndef IMPLICIT_MESHER
#define IMPLICIT_MESHER

#include "../../GeometryAlgorithm.h"
#include "RadialBasisFunc.h"
#include "HermiteRBF.h"
#include "../MeshCreation_Struct_Def.h"

class ImplicitMesher
{
public:
	ImplicitMesher(void);
	~ImplicitMesher(void);

	//model mesh from contours, via implicit surface
	void ContourToMesh(vector<CurveNetwork> vecCurveNetwork,KW_Mesh& Mesh,vector<Point_3>& vecTestPoint);

private:
	//model implicit surface from contours
	void ContourToImpSurf(vector<CurveNetwork> vecCurveNetwork,vector<Point_3>& vecTestPoint);

	//Jean-Daniel Boissonnat's algorithm for meshing implicit surface
	void ImpSurfToMeshJDBois(ImpPolyhedron& ImpPoly);

	//marching cube algorithm for meshing implicit surface
	void ImpSurfToMeshMarCub(KW_Mesh& Mesh);
	
	//Weights: weight for radial basis functions
	//Poly: degree one polynomial: Poly[0]+Poly[1]*x+Poly[2]*y+Poly[3]*z
	//here may contain weights and polynomials of multiple parts
	//and the variables with similar names in class RadialBasisFunc only 
	//stores one part for the current computation
	//these parameteres may also be useful for further computation, such as CSG
	vector<double> Weights,PolyNom;

	//weights and polynomial parameters for Hermite RBF
	vector<double> WeightsAlpha;//num=j
	vector<vector<double>> WeightsBeta;//num=3j
	vector<double> PolyNomA;//num=3
	double PolyNomB;//num=1

	//InterpoPoints: points that the surface interpolates
	vector<Point_3> InterpoPoints;
};

/*Convert from Marching cube polygonnizer to CGAL*/
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Convert_MarCubPoly_To_CGALPoly : public CGAL::Modifier_base<HDS> {
public:
	Convert_MarCubPoly_To_CGALPoly(Polygonizer* modelIn) {model=modelIn;}//const
	void operator()( HDS& hds) {
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		B.begin_surface( model->no_vertices(),model->no_triangles());
		
		for (int i=0;i<model->no_vertices();i++)
		{
			B.add_vertex(Point_3(model->get_vertex(i).x,
				model->get_vertex(i).y,
				model->get_vertex(i).z));
		}
		for (int i=0;i<model->no_triangles();i++)
		{
			B.begin_facet();
			TRIANGLE t = model->get_triangle(i);
			B.add_vertex_to_facet(t.v0);
			B.add_vertex_to_facet(t.v1);
			B.add_vertex_to_facet(t.v2);
			B.end_facet();
		}
		B.end_surface();
	}
private:
	//You can only call functions marked const for the const objects!
	//since get_vertex(i) method of Polygonizer is not const function
	//so here do not use const
	Polygonizer* model;
};
/*Convert from Marching cube polygonnizer to CGAL*/

#endif