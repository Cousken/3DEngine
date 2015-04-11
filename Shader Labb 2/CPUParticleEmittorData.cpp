#include "CPUParticleEmittorData.h"
#include "EffectContainer.h"
#include "TextureContainer.h"
#include "Engine.h"

CPUParticleEmittorData::CPUParticleEmittorData(void)
{
}

CPUParticleEmittorData::~CPUParticleEmittorData(void)
{
}

bool CPUParticleEmittorData::LoadDataFile( std::string aDataFile )
{
	myEffect = Engine::GetInstance()->GetEffectContainer().GetEffect("Particle_Shader.fx");
	myTexture = Engine::GetInstance()->GetTextureContainer().GetTexture(aDataFile, true);
	myEmissionRate = 155;
	myMinRotation = Vector3f(0, 800, 0);
	myMaxRotation = Vector3f(0, 800, 0);
	myEmissionLifeTime = 6;
	myEmissionExtents = Vector3f(0,0,0);
	myMixVelocity = Vector3f(0,0,0);
	myMaxVelocity = Vector3f(0,0,0);
	myEmissionRateDelta = 0;
	myEmissionVelocityDelta = Vector3f(0,0,0);
	myRotationDelta = Vector3f(0,0,0);
	myMaxNrOfParticles = (myEmissionLifeTime + 1) * myEmissionRate;

	myParticleData.myAffectedByGravityFlag = true;
	myParticleData.myMinVelocity = Vector3f( 0.2, 0, 0.2);
	myParticleData.myMaxVelocity = Vector3f( 0.8, 1.5, 0.8);
	myParticleData.myLifeTime = 2;
	myParticleData.myMinStartSize = 0.25;
	myParticleData.myMaxStartSize = 0.25;
	myParticleData.mySizeDelta = -0.20;
	myParticleData.myStartAlpha = 0.8;
	myParticleData.myAlphaDelta = 0;

	if ( InitLayoutBuffer() == false)
	{
		return false;
	}
	return true;
}

bool CPUParticleEmittorData::InitLayoutBuffer()
{
	HRESULT hr = S_OK;

	// Define the input layout
	const D3D10_INPUT_ELEMENT_DESC cpuParticleLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE",     0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "ALPHA",    0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TIME",     0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( cpuParticleLayout );

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	myEffect->GetTechnique()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = Engine::GetInstance()->GetDevice()->CreateInputLayout( cpuParticleLayout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &myParticleInputLayout );

	if (hr == S_FALSE)
	{
		return false;
	}

	return true;
}
