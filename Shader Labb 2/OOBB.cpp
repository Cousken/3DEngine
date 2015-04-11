#include "OOBB.h"

OOBB::OOBB( const Vector3f aCorner, const Vector3f aWidth )
{
	myRotation = Vector3f(0,0,0);
	myPosition = aCorner;
	myWidth = aWidth;
	myMinPoint = myPosition - myWidth - myRotation * myWidth;
	myMaxPoint = myPosition + myWidth + myRotation * myWidth;
}

OOBB::~OOBB()
{
}

void OOBB::SetPosition( const Vector3f aPosition )
{
	myPosition = aPosition;
	myMinPoint = myPosition - myWidth - myRotation * myWidth;
	myMaxPoint = myPosition + myWidth + myRotation * myWidth;
}

void OOBB::SetPosition( const float anX, const float anY, const float anZ )
{
	myPosition.x = anX;
	myPosition.y = anY;
	myPosition.z = anZ;
	myMinPoint = myPosition - myWidth - myRotation * myWidth;
	myMaxPoint = myPosition + myWidth + myRotation * myWidth;
}

Vector3f OOBB::GetPosition() const
{
	return myPosition;
}

void OOBB::SetRotation( const Vector3f aRotation )
{
	myRotation = aRotation;
	myMinPoint = myPosition - myWidth - myRotation * myWidth;
	myMaxPoint = myPosition + myWidth + myRotation * myWidth;
}

void OOBB::SetRotation( const float anX, const float anY, const float anZ )
{
	myRotation.x = anX;
	myRotation.y = anY;
	myRotation.z = anZ;
	myMinPoint = myPosition - myWidth - myRotation * myWidth;
	myMaxPoint = myPosition + myWidth + myRotation * myWidth;
}

Vector3f OOBB::GetRotation() const
{
	return myRotation;
}

Matrix33f OOBB::GetRotationMatrix() const
{
	Matrix33f matrixRotation = Matrix33f::CreateRotateAroundX( myRotation.x );
	matrixRotation = Matrix33f::CreateRotateAroundY( myRotation.y ) * matrixRotation;
	matrixRotation = Matrix33f::CreateRotateAroundZ( myRotation.z ) * matrixRotation;
	return matrixRotation;
}

Vector3f OOBB::GetPositiveVertex( const Vector3f aNormal ) const
{
	Vector3f positiveVertex = myMinPoint;

	if( aNormal.x >= 0 )
		positiveVertex.x = myMaxPoint.x;
	if( aNormal.y >= 0 )
		positiveVertex.y = myMaxPoint.y;
	if( aNormal.z >= 0 )
		positiveVertex.z = myMaxPoint.z;

	return positiveVertex;
}

Vector3f OOBB::GetNegativeVertex( const Vector3f aNormal ) const
{
	Vector3f negativeVertex = myMaxPoint;
	if( aNormal.x >= 0 )
		negativeVertex.x = myMinPoint.x;
	if( aNormal.y >= 0 )
		negativeVertex.y = myMinPoint.y;
	if( aNormal.z >= 0 )
		negativeVertex.z = myMinPoint.z;

	return negativeVertex;
}

const Vector3f &OOBB::GetMinPoint() const
{
	return myMinPoint;
}

const Vector3f &OOBB::GetMaxPoint() const
{
	return myMaxPoint;
}