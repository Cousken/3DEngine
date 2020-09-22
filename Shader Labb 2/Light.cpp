#include "Light.h"
#include "Engine.h"
#include "Texture.h"
#include <D3DX10.h>
#include "RenderTarget.h"

Light::Light( const LightType aLightType /*= DIRECTIONAL_LIGHT_TYPE*/ )
{
	myIsInsideFrustrum = true;
	myType = aLightType;
	myCastShadowFlag = true;
	myProjectiveTexture = NULL;

	InitEffectTechnique(aLightType);

	myColor = Vector4f(0,0,0,0);
	Vector4f myDirection  = Vector4f(0,0,0,0);;
	Vector3f myOriginalDirection  = Vector3f(0,0,0);;
	Vector3f myPosition  = Vector3f(0,0,0);;
	float myMaxDistance = 0;
	float myFallofInnerAngle = 0;
	float myFallofOuterAngle = 0;

	Matrix33f myOrientation = Matrix33f::Identity();

	myShadowMap = new RenderTarget();
	bool success = myShadowMap->Init(DXGI_FORMAT_R32G32B32A32_FLOAT, 800);
	assert(success == true && "ERROR CREATING SHADOW MAP");
}

Light::~Light(void)
{
}

void Light::Update()
{
	Vector3f newDir = myOriginalDirection * myOrientation;
	this->SetLightDir(newDir);
}

Vector4f Light::GetCurrentLightDir()
{
	return myDirection;
}

Vector4f& Light::GetLightColor()
{
	return myColor;
}

const Vector3f& Light::GetLightDir()
{
	return myOriginalDirection;
}

void Light::Rotate( Matrix33f& aRotation )
{
	myOrientation = myOrientation * aRotation;
}

void Light::SetLightColor( Vector4f& aVector )
{
	myColor = aVector;
}

void Light::SetLightDir( Vector3f& aVector )
{
	Vector3f normVector = aVector.Normalize();
	myDirection.x = normVector.x;
	myDirection.y = normVector.y;
	myDirection.z = normVector.z;
	myDirection.w = 0.0f;

	D3DXMATRIX result;
	D3DXVECTOR3 eye;
	D3DXVECTOR3 at;
	D3DXVECTOR3 up;
	eye.x = myPosition.x;
	eye.y = myPosition.y;
	eye.z = myPosition.z;
	at.x = myPosition.x + myDirection.x;
	at.y = myPosition.y + myDirection.y;
	at.z = myPosition.z + myDirection.z;
	Vector3f upVector = Vector3f(myDirection.myX, myDirection.myY, myDirection.myZ) * Matrix33f::CreateRotateAroundX(-1.57f);
	/*up.x = upVector.x;
	up.y = upVector.y;
	up.z = upVector.z;*/
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	assert(D3DXMatrixLookAtLH( &result, &eye, &at, &up) != NULL && "ERROR CREATING LOOK AT MATRIX");

	Matrix44f inverseOrientation;
	for(int i = 0; i < 16; i++)
	{
		inverseOrientation.myData[i] = result[i];
	}
	Matrix44f orientation = inverseOrientation.TransposeInverse();
	myLightViewMatrix = orientation;
	myLightViewMatrix.SetPosition(myPosition);
}

void Light::SetMaxDistance( const float& aMaxDistance )
{
	myMaxDistance = aMaxDistance;
}

void Light::SetPosition( Vector3f& aPosition )
{
	myPosition = aPosition;
	myLightViewMatrix.SetPosition(myPosition);
}

float& Light::GetMaxDistance()
{
	return myMaxDistance;
}

Vector3f& Light::GetPosition()
{
	return myPosition;
}	

Light::LightType Light::GetType()
{
	return myType;
}

float& Light::GetInnerFallofAngle()
{
	return myFallofInnerAngle;
}

float& Light::GetOuterFallofAngle()
{
	return myFallofOuterAngle;
}

void Light::SetInnerFallofAngle( const float& aAngle )
{
	myFallofInnerAngle = aAngle;
}

void Light::SetOuterFallofAngle( const float& aAngle )
{
	myFallofOuterAngle = aAngle;
}

void Light::TurnOffShadow()
{
	myCastShadowFlag = false;
}	

void Light::TurnOnShadow()
{
	myCastShadowFlag = true;
}

bool Light::GetCastsShadow() const
{
	return myCastShadowFlag;
}

void Light::InitEffectTechnique( const LightType aLightType )
{
	if ( aLightType == DIRECTIONAL_LIGHT_TYPE )
	{
		myEffectTechnique = EffectTechniques::ADD_DIRECTIONAL;
	}
	else if ( aLightType == POINT_LIGHT_TYPE )
	{
		myEffectTechnique = EffectTechniques::ADD_POINT;
	}
	else
	{
		myEffectTechnique = EffectTechniques::ADD_AREA;
	}
}

const EffectTechniques::TechniqueType Light::GetEffectTechnique()
{
	return myEffectTechnique;
}

const Matrix44f Light::GetViewMatrix( const Vector3f aLightPosition, const Vector3f aTarget, const Vector3f aUpVector )
{
	Matrix44f result = Matrix44f::Identity();
	if ( myType == DIRECTIONAL_LIGHT_TYPE || myType == SPOT_LIGHT_TYPE )
	{
		D3DXMATRIX rotation(&result.myData[0]);
		D3DXVECTOR3 position(aLightPosition.x,aLightPosition.y,aLightPosition.z);
		D3DXVECTOR3 target( aTarget.x, aTarget.y, aTarget.z );
		D3DXVECTOR3 up(aUpVector.x, aUpVector.y, aUpVector.z);

		D3DXMatrixLookAtLH( &rotation, &position, &target, &up);

		result.ConvertD3D(&rotation[0]);
		result = result * myOrientation;
	}
	else
	{	
		assert( false && "LIGHT IS NOT DIRECTIONAL/Spot");
	}

	return result;
}


const WorldViewProjectionMatrixes Light::GetWorldViewProjectionMatrixes(CommonUtilities::StaticArray<Vector3f, 8> someCameraCorners, Vector3f aCameraPosition)
{
	Matrix44f lightRotation = GetViewMatrix();

	// Transform the positions of the corners into the direction of the light

	for (int cornerTransformed = 0; cornerTransformed < someCameraCorners.GetSize(); cornerTransformed++)
	{
		someCameraCorners[cornerTransformed] = someCameraCorners[cornerTransformed] * lightRotation;

	}

	
	// Find the smallest box around the points
	AABB lightBox;
	lightBox.CreateFromFrustrum(someCameraCorners);
	Vector3f boxSize = lightBox.myMaxPoint - lightBox.myMinPoint;
	Vector3f halfBoxSize = boxSize * 0.5f;

	// The position of the light should be in the center of the back
	// pannel of the box. 
 	Vector3f lightPosition = lightBox.myMinPoint + halfBoxSize;
	lightPosition.z = lightBox.myMinPoint.z;

	// We need the position back in world coordinates so we transform 
	// the light position by the inverse of the lights rotation
	lightPosition = lightPosition * lightRotation.Inverse();

	// Create the view matrix for the light
	Vector3f direction( myDirection.x, myDirection.y, myDirection.z );
	Vector3f up = (direction * Matrix33f::CreateRotateAroundX( -D3DX_PI * 0.5f ));
	Matrix44f lightView = GetViewMatrix( lightPosition, (lightPosition-direction), up );

	// Create the projection matrix for the light
	// The projection is orthographic since we are using a directional light
	Matrix44f lightProjection;
	D3DXMATRIX  projection;
	D3DXMatrixOrthoLH( &projection, boxSize.x, boxSize.y, -boxSize.z, boxSize.z );
	lightProjection.Init(projection);

	WorldViewProjectionMatrixes result(lightRotation, lightView, lightProjection, lightPosition);

	return result;
}

const Matrix44f Light::GetProjectionMatrix()
{
	float myNearPlane = 0.1f;
	float myFarPlane = myMaxDistance;//+ 10.0f ;

	//float yFov = myFallofOuterAngle * 2.0f;
	float yFov = 1.57f;//acos(myFallofOuterAngle) * 2.0f;

	D3DXMATRIX  projection;
	D3DXMatrixPerspectiveFovLH( &projection, yFov, 1.0f, myNearPlane, myFarPlane );
	Matrix44f result;
	result.Init(static_cast<float*>(projection));
	return result;
}

bool Light::LoadProjectiveTexture( const std::string aFileName )
{
	myProjectiveTexture = Engine::GetInstance()->GetTextureContainer().GetTexture(aFileName, false);
	if ( myProjectiveTexture == NULL )
	{
		return false;
	}
	return true;
}

void Light::SetProjectionTexture()
{
	if ( myProjectiveTexture != NULL )
	{
		if ( myType == SPOT_LIGHT_TYPE || myType == DIRECTIONAL_LIGHT_TYPE )
		{
			Engine::GetInstance()->GetInstance()->GetEffectInput().SetProjectionTexture2D( myProjectiveTexture->GetTexture() );
		}
		else if ( myType == POINT_LIGHT_TYPE )
		{
			Engine::GetInstance()->GetInstance()->GetEffectInput().SetProjectionTextureCube( myProjectiveTexture->GetTexture() );
		}
	}	
}

Matrix44f Light::GetViewMatrix()
{
	Vector3f toTarget(myDirection.x, myDirection.y, myDirection.z);
// 	toTarget.Normalize();
// 	toTarget *= myMaxDistance;
	Vector3f target = myPosition + toTarget;
	Vector3f direction(myDirection.x, myDirection.y, myDirection.z);
	Vector3f up = (direction * Matrix33f::CreateRotateAroundX( -D3DX_PI * 0.5f ));

	Matrix44f view = GetViewMatrix(myPosition, target, up);
// 	Matrix44f projection = GetProjectionMatrix();
// 	Matrix44f result = view * projection;
	return view;
}


