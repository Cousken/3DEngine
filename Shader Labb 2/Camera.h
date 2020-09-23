#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Matrix.h"
#include "Vector.h"
#include "Frustum.h"
#include "debugHeader.h"

class Camera
{
public:
	Camera(void);

	void ResetAxleVectors();

	~Camera(void);

	void Translate(Vector3f aMovement);
	void Rotate(Matrix33f aRotation);

	Matrix44f& GetProjection();
	Matrix44f& GetOrientation();
	Matrix44f& GetInverseOrientation();
	Matrix44f& GetPrevInverseOrientation();
	Matrix44f& GetPrevProjecion();

	void GetPosition(Vector3f& aPosition);
	Vector3f GetPositionNotRetarded();
	float GetNearPlane() const;
	float GetFarPlane() const;

	void SetOrientation(Matrix44f& aOrientation);
	void SetOrientationNotRetarded(Matrix44f& anOrientation);
	void SetRotation(Matrix33f aRotation);
	void ApplyOrientation(Matrix44f& aOrientation);
	void SetPosition(Vector3f& aPosition);
	Frustum& GetFrustrum();

//protected:

	void UpdateMatrixes();
	Frustum myFrustrum;

	Matrix44f myProjectionMatrix;
	Matrix44f myOrientation;
	Matrix44f myInverseOrientation;

	Vector3f myUpAxle;
	Vector3f myForwardAxle;
	Vector3f myRightAxle;
	Vector3f myPosition;
	float myNearPlane;
	float myFarPlane;
};

#endif
