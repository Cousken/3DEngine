#ifndef PEDOBEAR_EFFECT_INPUT_HEADER_
#define PEDOBEAR_EFFECT_INPUT_HEADER_

#include "Effect.h"
#include "StaticArray.h"
#include "Matrix.h"
#include "Camera.h"

struct WorldViewProjectionMatrixes;

class EffectInput
{
public:
	EffectInput();
	~EffectInput();

	bool Init( const std::string &anEffectFile );

	void UpdateCamera( Camera &aCamera );
	void UpdateCamera( WorldViewProjectionMatrixes& someData );
	void UpdateWorld( Matrix44f &aToWorldMatrix );
	void UpdatePreviousWorld( Matrix44f &aToWorldMatrix );

	//void UpdateDirectionalLights( CommonUtilities::StaticArray< Vector4f, 9 > &someLightVectors, 
	//	CommonUtilities::StaticArray< Vector4f, 9 > &someColors,
	//	unsigned int aNumberOfLights);
	//void UpdatePointLights(CommonUtilities::StaticArray< Vector3f, 9 >& somePositions,
	//	CommonUtilities::StaticArray< Vector4f, 9 > &someColors,
	//	CommonUtilities::StaticArray< float, 9 >& someMaxDistnaces,
	//	unsigned int aNumberOfLights);
	//void UpdateSpotLights(CommonUtilities::StaticArray< Vector3f, 9 >& somePositions,
	//	CommonUtilities::StaticArray< Vector4f, 9 > &someColors,
	//	CommonUtilities::StaticArray< float, 9 >& someMaxDistnaces,
	//	CommonUtilities::StaticArray< Vector4f, 9 > &someLightVectors, 
	//	CommonUtilities::StaticArray< float, 9 >& someInnerFalloffs,
	//	CommonUtilities::StaticArray< float, 9 >& someOuterFalloffs,
	//	unsigned int aNumberOfLights);

	void UpdateDirectionalLight(Vector4f &aColor, Vector4f &aDirection);
	void UpdatePointLight(Vector4f &aColor, Vector3f &aPosition, float &aMaxDistance);
	void UpdateSpotLight(Vector4f& aColor, Vector4f& aDirection, Vector3f& aPosition, float& aMaxDistance, float& anInnerFalloff, float& anOuterFalloff);

	void SetDiffuseMap( Texture* aTexture );
	void SetEnviromentMap( Texture* aEnviromentMap );
	void SetEnviromentMap( ID3D10ShaderResourceView* aEnviromentMap );
	void SetReflectionMap( ID3D10ShaderResourceView* aReflectionMap );
	void SetAmbientProbePosition(Vector3f& aPosition);
	void SetEmptyEnviromentMap();
	void SetNormalMap( Texture* aNormalMap );
	
	void SetPrimaryPostProcesingTexture( ID3D10ShaderResourceView* aResourceToRender );
	void SetSecondaryPostProcesingTexture( ID3D10ShaderResourceView* aResourceToRender );
	void SetThirdPostProcesingTexture( ID3D10ShaderResourceView* aResourceToRender );
	void SetShadowTexture(ID3D10ShaderResourceView* aResource);
	void SetVelocityTexture( ID3D10ShaderResourceView* aResourceToRender );
	void SetPreviousVelocityTexture( ID3D10ShaderResourceView* aResourceToRender );
	void SetNoiseTexture( ID3D10ShaderResourceView* aResource );
	void SetProjectionTexture2D( ID3D10ShaderResourceView* aResource );
	void SetProjectionTextureCube( ID3D10ShaderResourceView* aResource );

	void SetDownsampleArray( Vector2f someDownsampleValues[16] );

	void SetVector( const std::string &aName, Vector3f aVector );
	void SetVector( const std::string &aName, Vector2f aVector );
	void SetScalar( const std::string &aName, const float aScalar );
	void SetMatrixArray( const std::string &aName, const Matrix44f* aMatrix, const unsigned int anAmount );

//private:
	friend class Effect;

	ID3D10EffectPool* GetEffectPool();

	std::string myEffectFile;
	float myTime;

	ID3D10EffectPool *myEffectPool;

	ID3D10EffectMatrixVariable *myWorldMatrixVariable;
	ID3D10EffectMatrixVariable *myViewMatrixVariable;
	ID3D10EffectMatrixVariable *myProjectionMatrixVariable;
	ID3D10EffectMatrixVariable *myLightViewMatrixVariable;
	ID3D10EffectMatrixVariable *myLightProjectionMatrixVariable;

	ID3D10EffectMatrixVariable *myPreviousWorldMatrixVariable;
	ID3D10EffectMatrixVariable *myPreviousViewMatrixVariable;
	ID3D10EffectMatrixVariable *myPreviousProjectionMatrixVariable;

	ID3D10EffectMatrixVariable *myInvertedViewMatrixVariable;
	ID3D10EffectMatrixVariable *myInvertedProjectionVariable;
	ID3D10EffectVectorVariable *myObserverPositionVariable;

	ID3D10EffectVectorVariable *myDirectionalLightDirVar;
	ID3D10EffectVectorVariable *myDirectionalLightColVar;
	ID3D10EffectScalarVariable *myDirectionalLightsVariable;
	ID3D10EffectScalarVariable *myNumDirectionalLightsVariable;

	ID3D10EffectVectorVariable *myPointLightPosVar;
	ID3D10EffectVectorVariable *myPointLightColVar;
	ID3D10EffectScalarVariable *myPointLightMaxDistVar;
	ID3D10EffectScalarVariable *myNumPointLightsVariable;


	ID3D10EffectVectorVariable *myDirectionalLightColorVar;
	ID3D10EffectVectorVariable *myDirectionalLightDirectionVar;
	ID3D10EffectVectorVariable *myPointLightColorVar;
	ID3D10EffectVectorVariable *myPointLightPositionVar;
	ID3D10EffectScalarVariable *myPointLightMaxDistanceVar;

	ID3D10EffectVectorVariable *mySpotLightDirVar;
	ID3D10EffectVectorVariable *mySpotLightPosVar;
	ID3D10EffectVectorVariable *mySpotLightColVar;
	ID3D10EffectScalarVariable *mySpotLightMaxDistVar;
	ID3D10EffectScalarVariable *mySpotLightInnerFalloff;
	ID3D10EffectScalarVariable *mySpotLightOuterFalloff;
	//ID3D10EffectScalarVariable *myNumSpotLightsVar;
	
	ID3D10EffectShaderResourceVariable *myDiffuseVariable;
	ID3D10EffectShaderResourceVariable *myEnviromentMapVariable;
	ID3D10EffectVectorVariable *myAmbientProbePositionVariable;
	ID3D10EffectShaderResourceVariable *myNormalMapVariable;
	
	ID3D10EffectShaderResourceVariable *myPrimaryPostProcessingVariable;
	ID3D10EffectShaderResourceVariable *mySecondaryPostProcessingVariable;
	ID3D10EffectShaderResourceVariable *myThirdPostProcessingVariable;
	ID3D10EffectShaderResourceVariable *myShadowTextureVariable;
	ID3D10EffectShaderResourceVariable *myCurrentVelocityTexture;
	ID3D10EffectShaderResourceVariable *myPreviousVelocityTexture;
	ID3D10EffectShaderResourceVariable *myNoiseTexture;
	ID3D10EffectShaderResourceVariable *myProjectionTexture2D;
	ID3D10EffectShaderResourceVariable *myProjectionTextureCube;

	Matrix44f myPrevViewMatrix;
	Matrix44f myPrevProjectionMatrix;
	Matrix44f myProjectionMatrix;
	Matrix44f myViewMatrix;
	Matrix44f myInvertedProjection;
	Matrix44f myInvertedView;
};


#endif // PEDOBEAR_EFFECT_INPUT_HEADER_