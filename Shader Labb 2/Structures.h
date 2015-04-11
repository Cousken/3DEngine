#ifndef PEDOBEAR_STRUCTURES_HEADER_
#define PEDOBEAR_STRUCTURES_HEADER_

#include "Vector.h"


struct LineSegment
{
	Vector3f start;
	Vector3f end;
};

struct Plane
{
	Plane();
	Plane( const Vector3f &aPoint1, const Vector3f &aPoint2, const Vector3f &aPoint3 );

	void Init( const Vector3f aPoint1, const Vector3f aPoint2, const Vector3f aPoint3 );
	float Distance( const Vector3f aPoint ) const;
	bool Inside( const Vector3f aPoint ) const;
	Vector3f Intersect( const LineSegment &aSegment );

	Vector3f position;
	Vector3f normal;
	float d;
};

#endif // PEDOBEAR_STRUCTURES_HEADER_