#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vector.h"
#include "Matrix.h"
#include "EffectTechnique.h"
#include "StaticArray.h"
#include <string>

struct WorldViewProjectionMatrixes
{
	WorldViewProjectionMatrixes( Matrix44f aWorldMatrix, Matrix44f aViewMatrix, Matrix44f aProjectionMatrix, Vector3f aLightPosition)
	{
		myWorldMatrix = aWorldMatrix;
		myViewMatrix = aViewMatrix;
		myProjectionMatrix = aProjectionMatrix;
		myLightPosition = aLightPosition;
	}

	Matrix44f myWorldMatrix;
	Matrix44f myViewMatrix;
	Matrix44f myProjectionMatrix;
	Vector3f myLightPosition;
};

struct AABB
{
	void CreateFromFrustrum( CommonUtilities::StaticArray< Vector3f, 8 > somePoints)
	{
		myMinPoint = somePoints[0];
		myMaxPoint = somePoints[1];

		for (int pointChecked = 0; pointChecked < 8; pointChecked++)
		{
			if ( somePoints[pointChecked].x < myMinPoint.x )
			{
				myMinPoint.x = somePoints[pointChecked].x;
			}
			if ( somePoints[pointChecked].y < myMinPoint.y )
			{
				myMinPoint.y = somePoints[pointChecked].y;
			}
			if ( somePoints[pointChecked].z < myMinPoint.z )
			{
				myMinPoint.z = somePoints[pointChecked].z;
			}

			if ( somePoints[pointChecked].x > myMaxPoint.x )
			{
				myMaxPoint.x = somePoints[pointChecked].x;
			}
			if ( somePoints[pointChecked].y > myMaxPoint.y )
			{
				myMaxPoint.y = somePoints[pointChecked].y;
			}
			if ( somePoints[pointChecked].z > myMaxPoint.z )
			{
				myMaxPoint.z = somePoints[pointChecked].z;
			}
		}
	}

	Vector3f myMinPoint;
	Vector3f myMaxPoint;
};

class Texture;
class RenderTarget;

class Light
{
public:
	enum LightType
	{
		DIRECTIONAL_LIGHT_TYPE = 0,
		POINT_LIGHT_TYPE = 1,
		SPOT_LIGHT_TYPE = 2,
		OUT_OF_BOUND_LIGHT_TYPE
	};

	Light(const LightType aLightType = DIRECTIONAL_LIGHT_TYPE);

	void InitEffectTechnique( const LightType aLightType );

	~Light(void);

	Vector4f GetCurrentLightDir();
	Vector4f& GetLightColor();
	Vector3f& GetPosition();
	float& GetMaxDistance();
	float& GetInnerFallofAngle();
	float& GetOuterFallofAngle();
	const Vector3f& GetLightDir();

	const WorldViewProjectionMatrixes GetWorldViewProjectionMatrixes( CommonUtilities::StaticArray<Vector3f, 8> someCameraCorners, Vector3f aCameraPosition);
	const Matrix44f GetProjectionMatrix();
	const Matrix44f GetViewMatrix(const Vector3f aLightPosition, const Vector3f aTarget, const Vector3f aUpVector);
	

	LightType GetType();
	
	void SetLightColor(Vector4f& aVector);
	void SetPosition(Vector3f& aPosition);
	void SetMaxDistance(const float& aMaxDistance);
	void SetInnerFallofAngle(const float& aAngle);
	void SetOuterFallofAngle(const float& aAngle);
	void SetLightDir(Vector3f& aVector);
	bool GetCastsShadow() const;
	const EffectTechniques::TechniqueType GetEffectTechnique();

	void Rotate(Matrix33f& aOrientation);
	void TurnOffShadow();
	void TurnOnShadow();
	bool LoadProjectiveTexture( const std::string aFileName );

	void Update();
	void SetProjectionTexture();
	Matrix44f GetViewMatrix();

	bool myIsInsideFrustrum;
	RenderTarget* myShadowMap;
	Matrix44f myLightViewMatrix;
private:
	LightType myType;
	EffectTechniques::TechniqueType myEffectTechnique;

	Vector4f myColor;
	Vector4f myDirection;
	Vector3f myOriginalDirection;
	Vector3f myPosition;
	float myMaxDistance;
	float myFallofInnerAngle;
	float myFallofOuterAngle;

	bool myCastShadowFlag;

	Matrix33f myOrientation;
	Texture* myProjectiveTexture;
	
};

#endif