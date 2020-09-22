#include "EffectInput.h"
#include "Engine.h"
#include <D3DX10.h>
#include "Light.h"

EffectInput::EffectInput()
{
	myEffectPool						= NULL;

	myWorldMatrixVariable				= NULL;
	myViewMatrixVariable				= NULL;
	myProjectionMatrixVariable			= NULL;
	myLightViewMatrixVariable			= NULL;
	myLightProjectionMatrixVariable		= NULL;

	myPreviousWorldMatrixVariable		= NULL;
	myPreviousViewMatrixVariable		= NULL;
	myPreviousProjectionMatrixVariable	= NULL;	
	myInvertedProjectionVariable		= NULL;
	myInvertedViewMatrixVariable		= NULL;
	myObserverPositionVariable			= NULL;
	
	myDirectionalLightDirVar	= NULL;
	myDirectionalLightColVar	= NULL;
	myDirectionalLightsVariable	= NULL;

	myPointLightPosVar			= NULL;
	myPointLightColVar			= NULL;
	myPointLightMaxDistVar		= NULL;
	myNumPointLightsVariable	= NULL;

	myDirectionalLightColorVar	= NULL;
	myDirectionalLightDirectionVar = NULL;
	myPointLightColorVar		= NULL;
	myPointLightPositionVar		= NULL;
	myPointLightMaxDistanceVar	= NULL;

	mySpotLightPosVar			= NULL;
	mySpotLightColVar			= NULL;
	mySpotLightMaxDistVar		= NULL;
	mySpotLightInnerFalloff		= NULL;
	mySpotLightOuterFalloff		= NULL;
	//myNumSpotLightsVar			= NULL;

	myDiffuseVariable			= NULL;
	myEnviromentMapVariable		= NULL;
	myNormalMapVariable			= NULL;
	
	myPrimaryPostProcessingVariable		= NULL;
	mySecondaryPostProcessingVariable	= NULL;
	myThirdPostProcessingVariable		= NULL;
	myCurrentVelocityTexture			= NULL;
	myPreviousVelocityTexture			= NULL;
	myNoiseTexture						= NULL;
	myProjectionTexture2D				= NULL;
	myProjectionTextureCube				= NULL;

	myProjectionMatrix = Matrix44f::Identity();
	myPrevProjectionMatrix = Matrix44f::Identity();
	myViewMatrix = Matrix44f::Identity();
	myPrevViewMatrix = Matrix44f::Identity();

	myTime = 0;
}

EffectInput::~EffectInput()
{
}

bool EffectInput::Init( const std::string &aEffectFile )
{
	myEffectFile = aEffectFile;

	unsigned int dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	ID3D10Blob *errors = NULL;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	HRESULT result = D3DX10CreateEffectPoolFromFile( myEffectFile.c_str(), NULL, NULL, "fx_4_0", dwShaderFlags, 0,
												 Engine::GetInstance()->GetDevice(), NULL,
												 &myEffectPool, &errors, NULL );

	if( FAILED( result ) )
	{
		if( errors != NULL )
		{
			char *error = NULL;
			error = (char *)errors->GetBufferPointer();
			MessageBox( Engine::GetInstance()->GetWindowHandler(), error, "Shader Error", MB_OK );
		}
		else
		{
			MessageBox( Engine::GetInstance()->GetWindowHandler(), "The FX file cannot be located or a silent error occured...", "Shader Error", MB_OK );
		}
		return false;
	}

	myWorldMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "World" )->AsMatrix();
	myViewMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "View" )->AsMatrix();
	myProjectionMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "Projection" )->AsMatrix();
	myLightViewMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "LightView" )->AsMatrix();
	myLightProjectionMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "LightProjection" )->AsMatrix();

	myPreviousWorldMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "prevWorld" )->AsMatrix();
	myPreviousViewMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "prevView" )->AsMatrix();
	myPreviousProjectionMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "prevProjection" )->AsMatrix();

	myInvertedProjectionVariable = myEffectPool->AsEffect()->GetVariableByName( "invertedProjection" )->AsMatrix();
	myInvertedViewMatrixVariable = myEffectPool->AsEffect()->GetVariableByName( "invertedView" )->AsMatrix();

	myObserverPositionVariable = myEffectPool->AsEffect()->GetVariableByName( "CameraPosition" )->AsVector();
	
	myDirectionalLightDirVar = myEffectPool->AsEffect()->GetVariableByName( "DirectionalLightDir" )->AsVector();
	myDirectionalLightColVar = myEffectPool->AsEffect()->GetVariableByName( "DirectionalLightCol" )->AsVector();
	myDirectionalLightsVariable = myEffectPool->AsEffect()->GetVariableByName( "LightDirections" )->AsScalar();
	myNumDirectionalLightsVariable = myEffectPool->AsEffect()->GetVariableByName( "NumDirectionalLights" )->AsScalar();

	myPointLightPosVar = myEffectPool->AsEffect()->GetVariableByName( "PointLightPos" )->AsVector();
	myPointLightColVar = myEffectPool->AsEffect()->GetVariableByName( "PointLightCol" )->AsVector();
	myPointLightMaxDistVar = myEffectPool->AsEffect()->GetVariableByName( "PointLightMaxDist" )->AsScalar();
	myNumPointLightsVariable = myEffectPool->AsEffect()->GetVariableByName( "NumPointLights" )->AsScalar();

	myPointLightColorVar			= myEffectPool->AsEffect()->GetVariableByName( "PointLightColor" )->AsVector();
	myPointLightPositionVar			= myEffectPool->AsEffect()->GetVariableByName( "PointLightPosition" )->AsVector();
	myPointLightMaxDistanceVar		= myEffectPool->AsEffect()->GetVariableByName( "PointLightMaxDistance" )->AsScalar();
	myDirectionalLightColorVar		= myEffectPool->AsEffect()->GetVariableByName( "DirectionalLightColor" )->AsVector();
	myDirectionalLightDirectionVar	=  myEffectPool->AsEffect()->GetVariableByName( "DirectionalLightDirection" )->AsVector();

	mySpotLightDirVar = myEffectPool->AsEffect()->GetVariableByName( "SpotLightDirection" )->AsVector();
	mySpotLightPosVar = myEffectPool->AsEffect()->GetVariableByName( "SpotLightPosition" )->AsVector();
	mySpotLightColVar = myEffectPool->AsEffect()->GetVariableByName( "SpotLightColor" )->AsVector();
	mySpotLightMaxDistVar = myEffectPool->AsEffect()->GetVariableByName( "SpotLightMaxDistance" )->AsScalar();
	mySpotLightInnerFalloff = myEffectPool->AsEffect()->GetVariableByName( "SpotLightInnerFalloff" )->AsScalar();
	mySpotLightOuterFalloff = myEffectPool->AsEffect()->GetVariableByName( "SpotLightOuterFalloff" )->AsScalar();
	//myNumSpotLightsVar = myEffectPool->AsEffect()->GetVariableByName( "NumSpotLights" )->AsScalar();;

	myDiffuseVariable = myEffectPool->AsEffect()->GetVariableByName( "DiffuseMap" )->AsShaderResource();
	myEnviromentMapVariable = myEffectPool->AsEffect()->GetVariableByName( "EnviromentMap" )->AsShaderResource();
	myAmbientProbePositionVariable = myEffectPool->AsEffect()->GetVariableByName("AmbientProbePosition")->AsVector();
	myNormalMapVariable = myEffectPool->AsEffect()->GetVariableByName( "NormalMap" )->AsShaderResource();
	
	myPrimaryPostProcessingVariable = myEffectPool->AsEffect()->GetVariableByName( "PrimaryShaderResourceView" )->AsShaderResource();
	mySecondaryPostProcessingVariable = myEffectPool->AsEffect()->GetVariableByName( "SecondaryShaderResourceView" )->AsShaderResource();
	myThirdPostProcessingVariable = myEffectPool->AsEffect()->GetVariableByName( "ThirdShaderResourceView" )->AsShaderResource();
	myShadowTextureVariable = myEffectPool->AsEffect()->GetVariableByName("ShadowTexture")->AsShaderResource();
	myCurrentVelocityTexture = myEffectPool->AsEffect()->GetVariableByName( "CurrentVelocityTexture" )->AsShaderResource();
	myPreviousVelocityTexture = myEffectPool->AsEffect()->GetVariableByName( "PreviousVelocityTexture" )->AsShaderResource();
	myNoiseTexture = myEffectPool->AsEffect()->GetVariableByName("RandomSSAOTexture")->AsShaderResource();
	myProjectionTexture2D = myEffectPool->AsEffect()->GetVariableByName("ProjectionTexture2D")->AsShaderResource();
	myProjectionTextureCube = myEffectPool->AsEffect()->GetVariableByName("ProjectionTextureCube")->AsShaderResource();

	return true;
}

void EffectInput::UpdateCamera( Camera &aCamera )
{
	Vector3f observer;
	aCamera.GetPosition(observer);

	myPrevProjectionMatrix = myProjectionMatrix;
	myPrevViewMatrix = myViewMatrix;
	myProjectionMatrix = aCamera.GetProjection();
	myViewMatrix = aCamera.GetInverseOrientation();

	myViewMatrixVariable->SetMatrix( &myViewMatrix.myData[0] );
	myObserverPositionVariable->SetFloatVector( reinterpret_cast< float * >( &observer ) );
	myProjectionMatrixVariable->SetMatrix( &myProjectionMatrix.myData[0] );

	myPreviousProjectionMatrixVariable->SetMatrix( &myPrevProjectionMatrix.myData[0] );
	myPreviousViewMatrixVariable->SetMatrix( &myPrevViewMatrix.myData[0] );

	myInvertedView = aCamera.GetOrientation();

	D3DXMATRIX proj;
	D3DXMATRIX cameraSpace;
	for(int i = 0; i < 16; i++)
	{
		proj[i] = aCamera.GetProjection().myData[i];
		cameraSpace[i] = myInvertedView.myData[i];
	}
	D3DXMATRIX invProjView;
	D3DXMatrixInverse(&invProjView, NULL, &proj);

	invProjView *= cameraSpace;
	
	myInvertedViewMatrixVariable->SetMatrix( &invProjView[0] );

	//invviewproj = CAM PROJECTON INVERSE * CAM ORIENTATION
	
// 	Matrix44f invViewProj = aCamera.GetProjection().Inverse() * aCamera.GetOrientation();
// 	myInvertedViewMatrixVariable->SetMatrix(&invViewProj.myData[0]);
}

void EffectInput::UpdateCamera( WorldViewProjectionMatrixes& someData )
{	
	Vector3f observer = someData.myLightPosition;

	myPrevProjectionMatrix = myProjectionMatrix;
	myPrevViewMatrix = myViewMatrix;
	myProjectionMatrix = someData.myProjectionMatrix;
	myViewMatrix = someData.myViewMatrix;

	//myLightViewMatrixVariable->SetMatrix( &myViewMatrix.myData[0] );
	//myLightProjectionMatrixVariable->SetMatrix( &myProjectionMatrix.myData[0] );
	myInvertedProjection = myProjectionMatrix.Inverse();
	myInvertedView = myViewMatrix.Inverse();

	D3DXMATRIX invProj;
	D3DXMATRIX invView;
	for(int i = 0; i < 16; i++)
	{
		invProj[i] = myInvertedProjection.myData[i];
		invView[i] = myInvertedView.myData[i];
	}
	D3DXMATRIX freshProj;
	D3DXMATRIX freshView;
	D3DXMatrixInverse(&freshProj, NULL, &invProj);
	D3DXMatrixInverse(&freshView, NULL, &invView);

	myInvertedViewMatrixVariable->SetMatrix( &freshView[0] );
	myInvertedProjectionVariable->SetMatrix( &freshProj[0] );
}

void EffectInput::UpdateWorld( Matrix44f &aToWorldMatrix )
{
	myWorldMatrixVariable->SetMatrix( &aToWorldMatrix.myData[0] );
}

void EffectInput::UpdatePreviousWorld( Matrix44f &aToWorldMatrix )
{
	myPreviousWorldMatrixVariable->SetMatrix( &aToWorldMatrix.myData[0] );
}

//void EffectInput::UpdateDirectionalLights( CommonUtilities::StaticArray< Vector4f, 9 > &someLightVectors, 
//										  CommonUtilities::StaticArray< Vector4f, 9 > &someColors, 
//										  unsigned int aNumberOfLights )
//{
//	myDirectionalLightColVar->SetFloatVectorArray(reinterpret_cast< float * >(&someColors[0] ), 0, aNumberOfLights);
//	myDirectionalLightDirVar->SetFloatVectorArray( reinterpret_cast< float * >( &someLightVectors[0] ), 0, aNumberOfLights );
//	myNumDirectionalLightsVariable->SetInt( aNumberOfLights );
//}
//
//
//void EffectInput::UpdatePointLights( CommonUtilities::StaticArray< Vector3f, 9 >& somePositions, CommonUtilities::StaticArray< Vector4f, 9 > &someColors, CommonUtilities::StaticArray< float, 9 >& someMaxDistnaces, unsigned int aNumberOfLights )
//{
//	myPointLightColVar->SetFloatVectorArray(reinterpret_cast< float * >(&someColors[0] ), 0, aNumberOfLights);
//	myPointLightPosVar->SetFloatVectorArray( reinterpret_cast< float* >( &somePositions[0]), 0, aNumberOfLights );
//	myPointLightMaxDistVar->SetFloatArray( reinterpret_cast< float* >( &someMaxDistnaces[0]), 0, aNumberOfLights);
//	myNumPointLightsVariable->SetInt( aNumberOfLights );
//}
//
//
//void EffectInput::UpdateSpotLights( CommonUtilities::StaticArray< Vector3f, 9 >& somePositions, CommonUtilities::StaticArray< Vector4f, 9 > &someColors, CommonUtilities::StaticArray< float, 9 >& someMaxDistnaces, CommonUtilities::StaticArray< Vector4f, 9 > &someLightVectors, CommonUtilities::StaticArray< float, 9 >& someInnerFalloffs, CommonUtilities::StaticArray< float, 9 >& someOuterFalloffs, unsigned int aNumberOfLights )
//{
//	mySpotLightPosVar->SetFloatVectorArray( reinterpret_cast< float * >( &someLightVectors[0] ), 0, aNumberOfLights );
//	mySpotLightColVar->SetFloatVectorArray( reinterpret_cast< float * >( &someColors[0] ), 0, aNumberOfLights );
//	mySpotLightPosVar->SetFloatVectorArray( reinterpret_cast< float* >( &somePositions[0]), 0, aNumberOfLights );
//	mySpotLightMaxDistVar->SetFloatArray( reinterpret_cast< float* >( &someMaxDistnaces[0]), 0, aNumberOfLights);
//	mySpotLightInnerFalloff->SetFloatArray( &someInnerFalloffs[0], 0, aNumberOfLights );
//	mySpotLightOuterFalloff->SetFloatArray( &someOuterFalloffs[0], 0, aNumberOfLights );
//	myNumSpotLightsVar->SetInt( aNumberOfLights );
//}

void EffectInput::UpdateDirectionalLight(Vector4f &aColor, Vector4f &aDirection)
{
	myDirectionalLightColorVar->SetFloatVector(reinterpret_cast<float*>(&aColor));
	myDirectionalLightDirectionVar->SetFloatVector(reinterpret_cast<float*>(&aDirection));
}	
void EffectInput::UpdatePointLight(Vector4f &aColor, Vector3f &aPosition, float &aMaxDistance)
{
	myPointLightColorVar->SetFloatVector(reinterpret_cast<float*>(&aColor));
	myPointLightPositionVar->SetFloatVector(reinterpret_cast<float*>(&aPosition));
	myPointLightMaxDistanceVar->SetFloat(aMaxDistance);
}

void EffectInput::UpdateSpotLight(Vector4f& aColor, Vector4f& aDirection, Vector3f& aPosition, float& aMaxDistance, float& anInnerFalloff, float& anOuterFalloff)
{
	mySpotLightColVar->SetFloatVector(reinterpret_cast<float*>(&aColor));
	mySpotLightDirVar->SetFloatVector(reinterpret_cast<float*>(&aDirection));
	mySpotLightPosVar->SetFloatVector(reinterpret_cast<float*>(&aPosition));
	mySpotLightMaxDistVar->SetFloat(aMaxDistance);
	mySpotLightInnerFalloff->SetFloat(anInnerFalloff);
	mySpotLightOuterFalloff->SetFloat(anOuterFalloff);
}

void EffectInput::SetDiffuseMap( Texture *aTexture )
{
	myDiffuseVariable->SetResource( aTexture->GetTexture() );
}

void EffectInput::SetEnviromentMap( Texture* aEnviromentMap )
{
	myEnviromentMapVariable->SetResource( aEnviromentMap->GetTexture() );
}

void EffectInput::SetEnviromentMap( ID3D10ShaderResourceView* aEnviromentMap )
{
	HRESULT hr = myEnviromentMapVariable->SetResource( aEnviromentMap );
}

void EffectInput::SetReflectionMap( ID3D10ShaderResourceView* aReflectionMap )
{
	HRESULT hr = myEffectPool->AsEffect()->GetVariableByName("ReflectionMap")->AsShaderResource()->SetResource(aReflectionMap);
}

void EffectInput::SetAmbientProbePosition(Vector3f& aPosition)
{
	myAmbientProbePositionVariable->SetFloatVector(&aPosition.myX);
}

void EffectInput::SetEmptyEnviromentMap()
{
	myEnviromentMapVariable->SetResource( NULL );
}

void EffectInput::SetNormalMap( Texture* aNormalMap )
{
	myNormalMapVariable->SetResource( aNormalMap->GetTexture() );
}

void EffectInput::SetPrimaryPostProcesingTexture( ID3D10ShaderResourceView* aPostProcessingResource )
{
	myPrimaryPostProcessingVariable->SetResource( aPostProcessingResource );
}

void EffectInput::SetVelocityTexture( ID3D10ShaderResourceView* aResourceToRender )
{
	myCurrentVelocityTexture->SetResource( aResourceToRender );
}

void EffectInput::SetPreviousVelocityTexture( ID3D10ShaderResourceView* aResourceToRender )
{
	myPreviousVelocityTexture->SetResource( aResourceToRender );
}

void EffectInput::SetNoiseTexture( ID3D10ShaderResourceView* aResource )
{
	myNoiseTexture->SetResource( aResource );
}

void EffectInput::SetScalar( const std::string &aName, const float aScalar )
{
	ID3D10EffectScalarVariable *variable = myEffectPool->AsEffect()->GetVariableByName( aName.c_str() )->AsScalar();
	variable->SetFloat( aScalar );
}

void EffectInput::SetMatrixArray( const std::string &aName, const Matrix44f* aMatrix, const unsigned int anAmount )
{
	ID3D10EffectMatrixVariable *variable = myEffectPool->AsEffect()->GetVariableByName( aName.c_str() )->AsMatrix();
	variable->SetMatrixArray( const_cast< float * >( reinterpret_cast< const float * >( &aMatrix->myData[0] ) ), 0, anAmount );
}

ID3D10EffectPool* EffectInput::GetEffectPool()
{
	return myEffectPool;
}

void EffectInput::SetVector( const std::string &aName, Vector3f aVector )
{
	ID3D10EffectVectorVariable *variable = myEffectPool->AsEffect()->GetVariableByName( aName.c_str() )->AsVector();
	variable->SetFloatVector( reinterpret_cast< float * >( &aVector ) );
}

void EffectInput::SetVector( const std::string &aName, Vector2f aVector )
{
	ID3D10EffectVectorVariable *variable = myEffectPool->AsEffect()->GetVariableByName( aName.c_str() )->AsVector();
	variable->SetFloatVector( reinterpret_cast< float * >( &aVector ) );
}

void EffectInput::SetDownsampleArray( Vector2f someDownsampleValues[16] )
{
	ID3D10EffectVectorVariable *variable = myEffectPool->AsEffect()->GetVariableByName( "downSampleOffsets" )->AsVector();
	variable->SetFloatVectorArray( reinterpret_cast<float*>(&someDownsampleValues[0]), 0, 16 );	
}

void EffectInput::SetSecondaryPostProcesingTexture( ID3D10ShaderResourceView* aResourceToRender )
{
	mySecondaryPostProcessingVariable->SetResource( aResourceToRender );
}

void EffectInput::SetThirdPostProcesingTexture( ID3D10ShaderResourceView* aResourceToRender )
{
	myThirdPostProcessingVariable->SetResource( aResourceToRender );
}

void EffectInput::SetShadowTexture(ID3D10ShaderResourceView* aResource)
{
	myShadowTextureVariable->SetResource(aResource);
}

void EffectInput::SetProjectionTexture2D( ID3D10ShaderResourceView* aResource )
{
	assert( aResource != NULL );
	myProjectionTexture2D->SetResource(aResource);
}

void EffectInput::SetProjectionTextureCube( ID3D10ShaderResourceView* aResource )
{
	assert( aResource != NULL );
	myProjectionTextureCube->SetResource(aResource);
}