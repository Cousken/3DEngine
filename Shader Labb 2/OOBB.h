#ifndef PEDOBEAR_OOBB_HEADER_
#define PEDOBEAR_OOBB_HEADER_

#include "Vector.h"
#include "debugHeader.h"
#include "Matrix.h"

class OOBB
{
public:
	OOBB( const Vector3f aCenter, const Vector3f aWidth );
	~OOBB();

	void SetPosition( const Vector3f aPosition );
	void SetPosition( const float anX, const float anY, const float anZ );
	Vector3f GetPosition() const;

	void SetRotation( const Vector3f aRotation );
	void SetRotation( const float anX, const float anY, const float anZ );
	Vector3f GetRotation() const;

	Matrix33f GetRotationMatrix() const;

	Vector3f GetPositiveVertex( const Vector3f aNormal ) const;
	Vector3f GetNegativeVertex( const Vector3f aNormal ) const;

	const Vector3f &GetMinPoint() const;
	const Vector3f &GetMaxPoint() const;

private:
	Vector3f myPosition;
	Vector3f myWidth;
	Vector3f myMaxPoint;
	Vector3f myMinPoint;
	Vector3f myRotation;
};

#endif // PEDOBEAR_OOBB_HEADER_