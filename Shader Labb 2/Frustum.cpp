#include "Frustum.h"

#define ANG2RAD 3.14159265358979323846/180.0

Frustum::Frustum()
{
	myPosition = Vector3f(0,0,0);
}

Frustum::Frustum( const float anAngle, const float aRatio, const float aNearDistance, const float aFarDistance )
{
	myPosition = Vector3f(0,0,0);
	myAngle = anAngle;
	myRatio = aRatio;
	myNearDistance = aNearDistance;
	myFarDistance = aFarDistance;
	float tang = tanf( static_cast< float >( ANG2RAD * myAngle * 0.5 ) );
	myNearHeight = myNearDistance * tang;
	myNearWidth = myNearHeight * myRatio;
	myFarHeight = myFarDistance * tang;
	myFarWidth = myFarHeight * myRatio;

	//// Near plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(0, 0, aNearPlane), Vector3f(0, 0, -1)));

	//// Far plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(0, 0, aFarPlane), Vector3f(0, 0, 1)));


	//// Right plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(), Vector3f(1, 0, 0) * Matrix33f::CreateRotateAroundY(-angle * ratio * 0.5f)));

	//// Left plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(), Vector3f(-1, 0, 0) * Matrix33f::CreateRotateAroundY(angle * ratio * 0.5f)));


	//// Top plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(), Vector3f(0, 1, 0) * Matrix33f::CreateRotateAroundX(angle * 0.5f)));

	//// Bottom plane
	//myPlaneVolume.AddPlane(Plane(Vector3f(), Vector3f(0, -1, 0) * Matrix33f::CreateRotateAroundX(-angle * 0.5f)));
}

Frustum::~Frustum()
{
}


void Frustum::Setup( const Vector3f aPosition, const Vector3f aForward, const Vector3f aSide, const Vector3f anUp, const Matrix33f aCameraSpace )
{
	myPosition = aPosition;
	Vector3f up = anUp * -1;
	Vector3f nearCenter = aPosition + aForward * myNearDistance;
	Vector3f farCenter = aPosition + aForward * myFarDistance;

	Vector3f nearTopLeft = nearCenter + up * myNearHeight - aSide * myNearWidth;
	Vector3f nearTopRight = nearCenter + up * myNearHeight + aSide * myNearWidth;
	Vector3f nearBottomLeft = nearCenter - up * myNearHeight - aSide * myNearWidth;
	Vector3f nearBottomRight = nearCenter - up * myNearHeight + aSide * myNearWidth;

	Vector3f farTopLeft = farCenter + up * myFarHeight - aSide * myFarWidth;
	Vector3f farTopRight = farCenter + up * myFarHeight + aSide * myFarWidth;
	Vector3f farBottomLeft = farCenter - up * myFarHeight - aSide * myFarWidth;
	Vector3f farBottomRight = farCenter - up * myFarHeight + aSide * myFarWidth;

	myPlanes[0].Init( nearTopRight, nearTopLeft, farTopLeft );
	myPlanes[1].Init( nearBottomLeft, nearBottomRight, farBottomRight);
	myPlanes[2].Init( nearTopLeft, nearBottomLeft, farBottomLeft);
	myPlanes[3].Init( nearBottomRight, nearTopRight,farBottomRight );
	myPlanes[4].Init( nearTopLeft, nearTopRight, nearBottomRight );
	myPlanes[5].Init( farTopRight, farTopLeft, farBottomLeft );

	CalculateFrustrumCorners(aCameraSpace);
}

bool Frustum::PointInside( const Vector3f aPoint ) const
{
	if( myPlanes[ 0 ].Inside( aPoint ) == false )
		return false;
	else if( myPlanes[ 1 ].Inside( aPoint ) == false )
		return false;
	else if( myPlanes[ 2 ].Inside( aPoint ) == false )
		return false;
	else if( myPlanes[ 3 ].Inside( aPoint ) == false )
		return false;
	else if( myPlanes[ 4 ].Inside( aPoint ) == false )
		return false;
	else if( myPlanes[ 5 ].Inside( aPoint ) == false )
		return false;
	else
		return true;
}

bool Frustum::SphereInside( const Vector3f aPoint, const float aRadius ) const
{
	float distance = 0;
	for( int index = 0; index < 6; index++ )
	{
		distance = myPlanes[ index ].Distance( aPoint );
		if( distance < -aRadius )
		{
			return false;
		}
	}
	return true;
}

bool Frustum::BoxInside( const OOBB &aBox ) const
{
	Matrix33f boxRotation = aBox.GetRotationMatrix();

	// TODO find out what INDEX3 is supposed to do
// 	Vector3f boxXRotation( boxRotation.myMatrix[ INDEX3( 0, 0 ) ], boxRotation.myMatrix[ INDEX3( 1, 0 ) ], boxRotation.myMatrix[ INDEX3( 2, 0 ) ] );
// 	Vector3f boxYRotation( boxRotation.myMatrix[ INDEX3( 0, 1 ) ], boxRotation.myMatrix[ INDEX3( 1, 1 ) ], boxRotation.myMatrix[ INDEX3( 2, 1 ) ] );
// 	Vector3f boxZRotation( boxRotation.myMatrix[ INDEX3( 0, 2 ) ], boxRotation.myMatrix[ INDEX3( 1, 2 ) ], boxRotation.myMatrix[ INDEX3( 2, 2 ) ] );

 	Vector3f boxXRotation( boxRotation.myMatrix[ 0 ], boxRotation.myMatrix[ 3 ], boxRotation.myMatrix[ 6 ] );
 	Vector3f boxYRotation( boxRotation.myMatrix[ 1 ], boxRotation.myMatrix[ 4 ], boxRotation.myMatrix[ 7 ] );
 	Vector3f boxZRotation( boxRotation.myMatrix[ 2 ], boxRotation.myMatrix[ 5 ], boxRotation.myMatrix[ 8 ] );

	for( unsigned int index = 0; index < 6; index++ )
	{
		Vector3f normal = myPlanes[ index ].normal;
		Vector3f boxNormal( boxXRotation.Dot( normal ), boxYRotation.Dot( normal ), boxZRotation.Dot( normal ) );
		if( myPlanes[ index ].Distance( aBox.GetPositiveVertex( boxNormal ) ) < 0 && myPlanes[ index ].Distance( aBox.GetNegativeVertex( boxNormal ) ) < 0 )
		{
			return false;
		}
	}

	return true;
}

float Frustum::GetAngle() const
{
	return myAngle;
}

float Frustum::GetRatio() const
{
	return myRatio;
}

float Frustum::GetNearDistance() const
{
	return myNearDistance;
}

float Frustum::GetFarDistance() const
{
	return myFarDistance;
}

CommonUtilities::StaticArray<Vector3f, 8> Frustum::GetFrustrumCorners()
{
	return myCorners;	
}

void Frustum::CalculateFrustrumCorners( const Matrix33f aCameraSpace )
{
	const float halfNearWidth = myNearWidth * 0.5f;
	const float halfNearHeight = myNearHeight * 0.5f;
	const float halfFarWidth = myFarWidth * 0.5f;
	const float halfFarHeight = myFarHeight * 0.5f;

	myCorners[NEAR_TOP_LEFT] = Vector3f( -halfNearWidth, halfNearHeight, myNearDistance);
	myCorners[NEAR_TOP_RIGHT] = Vector3f( halfNearWidth, halfNearHeight, myNearDistance);
	myCorners[NEAR_BOTTOM_RIGHT] = Vector3f( halfNearWidth, -halfNearHeight, myNearDistance);
	myCorners[NEAR_BOTTOM_LEFT] = Vector3f( -halfNearWidth, -halfNearHeight, myNearDistance);
	myCorners[FAR_TOP_LEFT] = Vector3f( -halfFarWidth, halfFarHeight, myFarDistance);
	myCorners[FAR_TOP_RIGHT] = Vector3f( halfFarWidth, halfFarHeight, myFarDistance);
	myCorners[FAR_BOTTOM_RIGHT] = Vector3f( halfFarWidth, -halfFarHeight, myFarDistance);
	myCorners[FAR_BOTTOM_LEFT] = Vector3f( -halfFarWidth, -halfFarHeight, myFarDistance);

	for (int cornerRotated = 0; cornerRotated < OUT_OF_BOUNDS_CORNER; cornerRotated++)
	{
		myCorners[cornerRotated] = myCorners[cornerRotated] * aCameraSpace; 
		myCorners[cornerRotated] += myPosition;
	}
}
