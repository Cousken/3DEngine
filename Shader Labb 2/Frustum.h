#ifndef PEDOBEAR_FRUSTUM_HEADER_
#define PEDOBEAR_FRUSTUM_HEADER_

#include "Structures.h"
#include "OOBB.h"
#include "Matrix.h"
#include "StaticArray.h"


class Frustum
{
public:
	Frustum();
	Frustum( const float anAngle, const float aRatio, const float aNearDistance, const float aFarDistance );
	~Frustum();

	void Setup( const Vector3f aPosition, const Vector3f aForward, const Vector3f aSide, const Vector3f anUp, const Matrix33f aCameraSpace );

	bool PointInside( const Vector3f aPoint ) const;
	bool SphereInside( const Vector3f aPoint, const float aRadius ) const;
	bool BoxInside( const OOBB &aBox ) const;

	float GetAngle() const;
	float GetRatio() const;
	float GetNearDistance() const;
	float GetFarDistance() const;

	CommonUtilities::StaticArray<Vector3f, 8> GetFrustrumCorners();

private:
	enum Corners
	{
		NEAR_TOP_LEFT,
		NEAR_TOP_RIGHT,
		NEAR_BOTTOM_RIGHT,
		NEAR_BOTTOM_LEFT,
		FAR_TOP_LEFT,
		FAR_TOP_RIGHT,
		FAR_BOTTOM_RIGHT,
		FAR_BOTTOM_LEFT,
		OUT_OF_BOUNDS_CORNER
	};
	void CalculateFrustrumCorners(const Matrix33f aCameraSpace);

	Vector3f myPosition;
	float myAngle;
	float myRatio;
	float myNearDistance;
	float myFarDistance;
	float myNearHeight;
	float myNearWidth;
	float myFarHeight;
	float myFarWidth;
	Plane myPlanes[ 6 ];

	CommonUtilities::StaticArray<Vector3f, 8> myCorners;
};


#endif // PEDOBEAR_FRUSTUM_HEADER_