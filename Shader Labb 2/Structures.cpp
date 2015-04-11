#include "Structures.h"

Plane::Plane()
{
}

Plane::Plane( const Vector3f &aPoint1, const Vector3f &aPoint2, const Vector3f &aPoint3 )
{
	Init( aPoint1, aPoint2, aPoint3 );
}

void Plane::Init( const Vector3f aPoint1, const Vector3f aPoint2, const Vector3f aPoint3 )
{
//Vector3f normal = Vector3f( vertex2.position - vertex1.position ).Cross( vertex3.position - vertex1.position ).Normalize();
//normal = ( aPoint2 - aPoint1 ).Cross( aPoint3 - aPoint1 ).Normalize();
	normal = ( aPoint3 - aPoint2 ).Cross( aPoint2 - aPoint1 ).Normalize();
	position = aPoint1;
	d = normal.Dot( aPoint2 );
}

float Plane::Distance( const Vector3f aPoint ) const
{
	return d - aPoint.Dot( normal );
}

bool Plane::Inside( const Vector3f aPoint ) const
{
	return normal.Dot( ( aPoint - position ).Normalize() ) > 0;
}

Vector3f Plane::Intersect( const LineSegment &aSegment )
{
	float u = ( -normal.x * aSegment.start.x - normal.y * aSegment.start.y - normal.z * aSegment.start.z + d );
	u = u / ( -normal.x * ( aSegment.start.x - aSegment.end.x ) - normal.y * ( aSegment.start.y - aSegment.end.y ) - normal.z * (aSegment.start.z - aSegment.end.z ) );
	return aSegment.start + ( aSegment.end - aSegment.start ) * u;
}