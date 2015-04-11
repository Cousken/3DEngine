#include "Camera.h"
#include <D3DX10.h>
#include "Engine.h"

Camera::Camera( void )
{
	myPosition = Vector3f(0,0,0);
	myNearPlane = 0.1f;
	myFarPlane = 200.0f;

	const float fovX = ( float )D3DX_PI * 0.5f;
	const float ratio = static_cast<float>(Engine::GetInstance()->GetScreeenWidth()) /
		static_cast<float>(Engine::GetInstance()->GetScreenHeight());
	const float fovY = fovX / ratio;

	myFrustrum = Frustum(90, ratio, myNearPlane, myFarPlane);

	ResetAxleVectors();
	UpdateMatrixes();	

	D3DXMATRIX  projection;
	D3DXMatrixPerspectiveFovLH( &projection, fovY, ratio, myNearPlane, myFarPlane );
	myProjectionMatrix.Init(static_cast<float*>(projection));
}

Camera::~Camera(void)
{
}

Matrix44f& Camera::GetProjection()
{
	return myProjectionMatrix;
}

Matrix44f& Camera::GetOrientation()
{
	return myOrientation;
}

void Camera::SetOrientation( Matrix44f& aOrientation )
{
	ResetAxleVectors();
	myPosition = Vector3f(0,0,0);
	myUpAxle = myUpAxle * aOrientation.Get33();
	myForwardAxle = myForwardAxle * aOrientation.Get33();
	myRightAxle = myRightAxle * aOrientation.Get33();
	myPosition = myPosition * aOrientation.Get33();
	UpdateMatrixes();
}

void Camera::SetRotation( Matrix33f aRotation )
{
	ResetAxleVectors();
	myUpAxle = myUpAxle * aRotation;
	myForwardAxle = myForwardAxle * aRotation;
	myRightAxle = myRightAxle * aRotation;
	UpdateMatrixes();
}

void Camera::SetPosition( Vector3f& aPosition )
{	
	myPosition = aPosition;
	UpdateMatrixes();
}

void Camera::GetPosition( Vector3f& aPosition )
{
	aPosition = myPosition;
}

Vector3f Camera::GetPositionNotRetarded()
{
	// HAHA Vackert Jaffe :)
	return myPosition;
}

Matrix44f& Camera::GetInverseOrientation()
{
	return myInverseOrientation;
}

void Camera::Translate( Vector3f aMovement )
{
	Vector3f newPosition = myPosition + aMovement;
	SetPosition(newPosition);
}

void Camera::Rotate( Matrix33f aRotation )
{
	Matrix33f temp = myOrientation * aRotation;
	SetRotation(temp);
}

void Camera::UpdateMatrixes()
{
	myOrientation.myData[0] = myRightAxle.x;
	myOrientation.myData[1] = myRightAxle.y;
	myOrientation.myData[2] = myRightAxle.z;

	myOrientation.myData[4] = myUpAxle.x;
	myOrientation.myData[5] = myUpAxle.y;
	myOrientation.myData[6] = myUpAxle.z;
	
	myOrientation.myData[8] = myForwardAxle.x;
	myOrientation.myData[9] = myForwardAxle.y;
	myOrientation.myData[10] = myForwardAxle.z;

	myOrientation.SetPosition(myPosition);

	myInverseOrientation = myOrientation.Inverse();

	myFrustrum.Setup(myPosition, myForwardAxle, myRightAxle, myUpAxle, myOrientation);
}

void Camera::ResetAxleVectors()
{
	myForwardAxle = Vector3f (0,0,1);
	myUpAxle = Vector3f(0,1,0);
	myRightAxle = Vector3f (1,0,0);
	
}

void Camera::ApplyOrientation( Matrix44f& aOrientation )
{
	Matrix44f temp = myOrientation * aOrientation;
	SetOrientation(temp);
}

float Camera::GetNearPlane() const
{
	return myNearPlane;
}

float Camera::GetFarPlane() const
{
	return myFarPlane;
}

Frustum& Camera::GetFrustrum()
{
	return myFrustrum;
}


