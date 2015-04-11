#include "ComputedModel.h"

#define TGA_PI 3.14159265f
#define DTOR TGA_PI / 180.0f

ComputedModel::ComputedModel(void)
{
}

ComputedModel::~ComputedModel(void)
{
}

void ComputedModel::InitPolygon(Effect* aEffect)
{
	myEffect = aEffect;	
	myVertices.Init(3,1);
	myVerticeIndexes.Init(3,1);

	myVertices.Add( VertexPosCol( Vector3f(0.0f, 0.5f, 0.5f ), Vector4f(1,0,0,1), Vector3f(0,0,-1)));
	myVertices.Add( VertexPosCol( Vector3f(0.5f, -0.5f, 0.5f ), Vector4f(0,1,0,1), Vector3f(0,0,-1)));
	myVertices.Add( VertexPosCol( Vector3f(-0.5f, -0.5f, 0.5f ), Vector4f(0,0,1,1), Vector3f(0,0,-1)));

	myVerticeIndexes.Add(0);
	myVerticeIndexes.Add(1);
	myVerticeIndexes.Add(2);
}

const CommonUtilities::GrowingArray<VertexPosCol>& ComputedModel::GetVertexData()
{
	return myVertices;
}

Effect* ComputedModel::GetEffect()
{
	return myEffect;
}

void ComputedModel::InitCube( Effect* aEffect )
{
	myEffect = aEffect;
	myVertices.Init(24,1);
	myVerticeIndexes.Init(36,1);

	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) ));

	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, -1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, -1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, -1.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, -1.0f, 0.0f ) ));

	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( -1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( -1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( -1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( -1.0f, 0.0f, 0.0f ) ));

	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 1.0f, 0.0f, 0.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 1.0f, 0.0f, 0.0f ) ));

	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, -1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, -1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, -1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, -1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, -1.0f ) ));

	myVertices.Add( VertexPosCol( Vector3f( -1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, -1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( 1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) ));
	myVertices.Add( VertexPosCol( Vector3f( -1.0f, 1.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) ));


	myVerticeIndexes.Add(3);
	myVerticeIndexes.Add(1);
	myVerticeIndexes.Add(0);
	myVerticeIndexes.Add(2);
	myVerticeIndexes.Add(1);
	myVerticeIndexes.Add(3);
// 	3,1,0,
// 	2,1,3,

	myVerticeIndexes.Add(6);
	myVerticeIndexes.Add(4);
	myVerticeIndexes.Add(5);
	myVerticeIndexes.Add(7);
	myVerticeIndexes.Add(4);
	myVerticeIndexes.Add(6);
// 	6,4,5,
// 	7,4,6,

	myVerticeIndexes.Add(11);
	myVerticeIndexes.Add(9);
	myVerticeIndexes.Add(8);
	myVerticeIndexes.Add(10);
	myVerticeIndexes.Add(9);
	myVerticeIndexes.Add(11);
// 	11,9,8,
// 	10,9,11,
	
	myVerticeIndexes.Add(14);
	myVerticeIndexes.Add(12);
	myVerticeIndexes.Add(13);
	myVerticeIndexes.Add(15);
	myVerticeIndexes.Add(12);
	myVerticeIndexes.Add(14);
// 	14,12,13,
// 	15,12,14,

	myVerticeIndexes.Add(19);
	myVerticeIndexes.Add(17);
	myVerticeIndexes.Add(16);
	myVerticeIndexes.Add(18);
	myVerticeIndexes.Add(17);
	myVerticeIndexes.Add(19);
// 	19,17,16,
// 	18,17,19,

	myVerticeIndexes.Add(22);
	myVerticeIndexes.Add(20);
	myVerticeIndexes.Add(21);
	myVerticeIndexes.Add(23);
	myVerticeIndexes.Add(20);
	myVerticeIndexes.Add(22);
// 	22,20,21,
// 	23,20,22
}

void ComputedModel::InitSphere( Effect* aEffect )
{
	myEffect = aEffect;
	myVertices.Init(24,1);
	myVerticeIndexes.Init(36,1);
	
	float dtheta = 40;
	float dphi = 40;
	int n;
	float theta,phi;
	Vector3f p[4];

	for (theta=-90;theta<=90-dtheta;theta+=dtheta) {
		for (phi=0;phi<=360-dphi;phi+=dphi) {
			n = 0;
			p[n].x = cos(theta*DTOR) * cos(phi*DTOR);
			p[n].y = cos(theta*DTOR) * sin(phi*DTOR);
			p[n].z = sin(theta*DTOR);
			n++;
			p[n].x = cos((theta+dtheta)*DTOR) * cos(phi*DTOR);
			p[n].y = cos((theta+dtheta)*DTOR) * sin(phi*DTOR);
			p[n].z = sin((theta+dtheta)*DTOR);
			n++;
			p[n].x = cos((theta+dtheta)*DTOR) * cos((phi+dphi)*DTOR);
			p[n].y = cos((theta+dtheta)*DTOR) * sin((phi+dphi)*DTOR);
			p[n].z = sin((theta+dtheta)*DTOR);
			n++;
			if (theta > -90 && theta < 90) {
				p[n].x = cos(theta*DTOR) * cos((phi+dphi)*DTOR);
				p[n].y = cos(theta*DTOR) * sin((phi+dphi)*DTOR);
				p[n].z = sin(theta*DTOR);
				n++;
			}

			myVertices.Add( VertexPosCol( p[0], Vector4f( p[0].x,p[0].y,p[0].z, 1.0f ), Vector3f(0,0,0) ));
			myVertices.Add( VertexPosCol( p[1], Vector4f( p[1].x,p[1].y,p[1].z, 1.0f ), Vector3f(0,0,0) ));
			myVertices.Add( VertexPosCol( p[2], Vector4f( p[2].x,p[2].y,p[2].z, 1.0f ), Vector3f(0,0,0) ));

// 			myVerticeIndexes.Add(myVertices.Count()-1);
// 			myVerticeIndexes.Add(myVertices.Count()-2);
// 			myVerticeIndexes.Add(myVertices.Count()-3);
// 
// // 			if (myVertices.Count() >= 5)

// // 			{

// 			myVerticeIndexes.Add(myVertices.Count()-6);
// 			myVerticeIndexes.Add(myVertices.Count()-3);
// 			myVerticeIndexes.Add(myVertices.Count()-2);

			//}
			

		}
	}

	const int nbrVertx = myVertices.Count();
	for ( int polyI = 0; polyI < nbrVertx/3; polyI++)
	{
		myVerticeIndexes.Add( (polyI*3) + 2 );
		myVerticeIndexes.Add( (polyI*3) + 1 );
		myVerticeIndexes.Add( (polyI*3) + 0 );
	}
}

const CommonUtilities::GrowingArray<unsigned int>& ComputedModel::GetVertextIndexes()
{
	return myVerticeIndexes;
}

void ComputedModel::InitCone( Effect* aEffect )
{
	float height = 1.0f;
	int steps = 40;
	float radius = 3.0f;
	
	myEffect = aEffect;
	myVertices.Init(24,1);
	myVerticeIndexes.Init(36,1);

	Vector3f position( 0, 0, 0 );
	VertexPosCol top( position, Vector4f(position.x, position.y, position.x, 1.0f), Vector3f(0,0,0) );
	myVertices.Add(top);
	int topIndex = myVertices.Count()-1;

	position.y -= height;
	VertexPosCol bottom( position, Vector4f(position.x, position.y, position.x, 1.0f), Vector3f(0,0,0) );
	myVertices.Add(bottom);
	int bottomIndex = myVertices.Count()-1;

	const float stepDegrees = 2.0f * TGA_PI / static_cast<float>(steps); 
	position.z += radius;
	VertexPosCol firstVertex( position, Vector4f(position.x, position.y, position.x, 1.0f), Vector3f(0,0,0) );
	myVertices.Add(firstVertex);
	int firstCircleVertexIndex = myVertices.Count()-1;

	Matrix33f rotation (Matrix33f::CreateRotateAroundY(stepDegrees));
	for ( int newVertex = 0; newVertex < (steps-1); newVertex++ )
	{
		position = position * rotation;
		VertexPosCol circleVertex( position, Vector4f(position.x, position.y, position.x, 1.0f), Vector3f(0,0,0) );
		myVertices.Add(circleVertex);

		myVerticeIndexes.Add( myVertices.Count()-1 );
		myVerticeIndexes.Add( myVertices.Count()-2 );
		myVerticeIndexes.Add( topIndex );

		myVerticeIndexes.Add( myVertices.Count()-2 );
		myVerticeIndexes.Add( myVertices.Count()-1 );		
		myVerticeIndexes.Add( bottomIndex );
	}
	
	myVerticeIndexes.Add( firstCircleVertexIndex );
	myVerticeIndexes.Add( myVertices.Count()-1 );
	myVerticeIndexes.Add( topIndex );

	myVerticeIndexes.Add( firstCircleVertexIndex );
	myVerticeIndexes.Add( bottomIndex );	
	myVerticeIndexes.Add( myVertices.Count()-1 );
	

}
