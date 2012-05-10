#include "StdAfx.h"
#include "KW_CS2Surf.h"
#include "../config.h"
#include <sys/timeb.h>
#include <time.h>

KW_CS2Surf::KW_CS2Surf(void)
{
	this->vecTestPoint.clear();
	this->vecTestSeg.clear();
	this->vecTempCN.clear();
	this->vecResortFace.clear();
	this->vecSSMesh.clear();
	this->vecPOF.clear();
	this->vecSinglePoly.clear();
	this->vecUnionPoly.clear();
	this->vecvecSubPoint.clear();
	this->vecvecSubSurf.clear();
	this->vecvecvecFacePoint.clear();
	this->vecSubSurfSSId.clear();
	this->iRenderSinglePoly=CR_RENDER_NONE_SINGLE_CYLINDER;
	this->iRenderUnionPoly=CR_RENDER_NONE_UNION_CYLINDER;
	this->vecTestTri.clear();
	clearStlSubspaceInfo();
}

KW_CS2Surf::~KW_CS2Surf(void)
{
	this->vecTestPoint.clear();
	this->vecTestSeg.clear();
	this->vecTempCN.clear();
	this->vecResortFace.clear();
	this->vecSSMesh.clear();
	this->vecPOF.clear();
	this->vecSinglePoly.clear();
	this->vecUnionPoly.clear();
	this->vecvecSubPoint.clear();
	this->vecvecSubSurf.clear();
	this->vecvecvecFacePoint.clear();
	this->vecSubSurfSSId.clear();
	this->vecTestTri.clear();
	clearStlSubspaceInfo();
}

void KW_CS2Surf::Reset()
{
}

bool KW_CS2Surf::ctr2sufProc(vector<vector<Point_3> >& MeshBoundingProfile3D,vector<Point_3>& vecTestPoint)
{
	clock_t   start   =   clock();   
	DBWindowWrite( "Reconstruction begins...\n");

	//preprocess data first
	int** pctrvermark;

	if( !ispreproced || isComnCase)
	{
		pctrvermark = preProcData(isComnCase);
	}

	//partition
	//	cout<<"------------------ PARTITION ------------------"<<endl;
	//	cout<<"Starting............"<<endl;
	partitionStl();

	//test
	//return false;


	PutCNtoFace();

	//test
	//return false;

	CombineSS();

	//test
	//return false;

	GenInitMesh();

	//test
	return false;

 	PostProcMesh();

	clock_t   endt   =   clock();
	DBWindowWrite("Reconstruction finished,taking: %d ms\n",endt - start);

	return true;

	//process data
	//	cout<<"------------------ PUT CONTOUR INTO FACE ------------------"<<endl;
	putContourIntoFace( pctrvermark );

	
	//go through each subspace to generate mesh
	//	cout<<"-------- GENERATING MESH IN EACH SUBSPACE ------------"<<endl;
	//kw noted: vertices, edges and faces in each subspace
	//stitch them together finally
	floatvector* subMeshVer;
	intvector* subMeshEdge;
	intvector* subMeshFace;
	subMeshVer = new floatvector[ ssspacenum ];
	subMeshEdge = new intvector[ ssspacenum ];
	subMeshFace = new intvector[ ssspacenum ];

	//initialize registration informaiton for stitching
	sverreg = new intvector[ ssvernum ];
	sedgereg = new vector<intvector>[ ssedgenum ];
	sedgesubedgemark = new intvector[ ssedgenum ];
	sfacectrei = new intset[ ssfacenum ];
	sfacespaci = new int[ ssfacenum * 2 ];
	sfaceregface = new vector<intvector>[ ssfacenum ];
	sfaceregver = new intvector[ ssfacenum ];
	sfaceregedgever = new intvector[ ssfacenum ];
	for( int i = 0; i < ssfacenum*2; i ++)
	{
		sfacespaci[ i ] = -1;
	}
	for( int spacei = 0; spacei < ssspacenum; spacei ++)
	{
		for( int facej = 0; facej < ssspacefacenum[ spacei ]; facej ++)
		{
			int facei = ssspace[ spacei ][ facej ];
			if( sfacespaci[ 2 * facei ] == -1)
				sfacespaci[ 2 * facei ] = spacei;
			else
				sfacespaci[ 2*facei + 1 ] = spacei;
		}
	}

	//kw: here can use multi-thread to compute each submesh in parallel
	for( int i = 0; i < ssspacenum; i ++)
	{
				cout<<"--- subspace " << i <<endl;
		ctr2sufSubspaceProc( i, subMeshVer[ i ], subMeshEdge[ i ], subMeshFace[ i ]);

		//submeshedge is useless for stitching!
		subMeshEdge[ i ].clear();
	}
	delete []subMeshEdge;


	//stitch all the subspaces together
	stitchMesh(subMeshVer,subMeshFace);


	//clear the registration information for subspace vertex, edge and face
	//sverreg
	for( int i = 0; i < ssvernum; i ++)
		sverreg[ i ].clear();
	delete []sverreg;
	sverreg = NULL;

	//sedgereg
	for( int i= 0; i < ssedgenum; i ++)
	{
		for(unsigned int j = 0; j < sedgereg[ i ].size(); j ++)
			sedgereg[ i ][ j ].clear();
		sedgereg[ i ].clear();
	}
	delete []sedgereg;
	sedgereg = NULL;

	//sfacectrei, sfacespaci, sfaceregface, sfaceregver
	for( int i= 0; i < ssfacenum; i ++)
	{
		sfacectrei[ i ].clear();
		
	//	for( int j = 0; j < sfaceregface[ i ].size();  j ++ )
    //       sfaceregface[ i ][ j ].clear();
	//	sfaceregface[ i ].clear();
		sfaceregver[ i ].clear();
	}	
	delete []sfacectrei;
	delete []sfacespaci;
	//delete []sfaceregface;
	delete []sfaceregver;
	sfacectrei = NULL;
	sfacespaci = NULL;
	//sfaceregface = NULL;
	sfaceregver = NULL;

	//set the mesh
	//clear
	if( mesh!= NULL )
		delete mesh;

	//set
	mesh = new Mesh( mver, mface, ctrmedge, 
		center,  unitlen, PROCSIZE );
	
	////kw: get constrained vertices
	//set<int> ConstrVer;
	//for (unsigned int i=0;i<ctrmedge.size();i++)
	//{
	//	ConstrVer.insert(ctrmedge.at(i));
	//}
	//for (set<int>::iterator SetIter=ConstrVer.begin();SetIter!=ConstrVer.end();SetIter++)
	//{
	//	vecTestPoint.push_back(Point_3(mver.at(3*(*SetIter))*DIM/PROCSIZE,mver.at(3*(*SetIter)+1)*DIM/PROCSIZE,
	//		mver.at(3*(*SetIter)+2)*DIM/PROCSIZE));
	//}

	//clear
	mver.clear();
	mface.clear();
	ctrmedge.clear();

	return true;
}

