#include "CPUParticleEmittorInstance.h"
#include <assert.h>
#include "Engine.h"
#include "CPUParticle.h"

CPUParticleEmittorInstance::CPUParticleEmittorInstance( CPUParticleEmittorData& aEmittor )
: myEmittor(aEmittor)
{
	myParticleIndexes.Init(myEmittor.myMaxNrOfParticles, myEmittor.myMaxNrOfParticles);
	myParticles.Init(myEmittor.myMaxNrOfParticles, myEmittor.myMaxNrOfParticles);

	bool success = true;
	success = CreateVertexBuffer();
	assert( success && "Failed to create a VertexBuffer");

	success = CreateIndexBuffer();
	assert( success && "Failed to create a IndexBuffer");

	myVertexBufferWrapper.myByteOffset = 0;
	myVertexBufferWrapper.myNbrOfBuffers = 1;
	myVertexBufferWrapper.myStartSlot = 0;
	myVertexBufferWrapper.myStride = sizeof( CPUParticle );

	myIndexBufferWrapper.myByteOffset = 0;
	myIndexBufferWrapper.myIndexBufferFormat = DXGI_FORMAT_R32_UINT;

	myProcessTime = 0;

	int particlesToEmitt = 100;
	while ( particlesToEmitt > 0 )
	{
		myParticles.Add( CPUParticle() );
		myParticleIndexes.Add( myParticles.Count() );
		particlesToEmitt--;
	}	
}

CPUParticleEmittorInstance::~CPUParticleEmittorInstance(void)
{
}

void CPUParticleEmittorInstance::Render()
{
// 	EffectInput& effectInput=Engine::GetInstance()->GetEffectInput();
// 	effectInput.UpdateWorld(myOrientation);
// 
// 	myEmittor.myEffect->SetAlphaDelta(myEmittor.myParticleData.myAlphaDelta);
// 	myEmittor.myEffect->SetSizeDelta(myEmittor.myParticleData.mySizeDelta);
// 	myEmittor.myEffect->SetStartAlpha(myEmittor.myParticleData.myStartAlpha);

	// Render the particles
	Engine::GetInstance()->GetDevice()->IASetInputLayout(  myEmittor.myParticleInputLayout );
	ID3D10Buffer *pBuffers[1] = { myVertexBufferWrapper.myVertexBuffer };
	UINT stride[1] = { sizeof( CPUParticle ) };
	UINT offset[1] = { 0 };
	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBufferWrapper.myStartSlot,
															myVertexBufferWrapper.myNbrOfBuffers,
															&myVertexBufferWrapper.myVertexBuffer,
															&myVertexBufferWrapper.myStride,
															&myVertexBufferWrapper.myByteOffset);
	Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	Engine::GetInstance()->GetDevice()->IASetIndexBuffer( myIndexBufferWrapper.myIndexBuffer,
															myIndexBufferWrapper.myIndexBufferFormat,
															myIndexBufferWrapper.myByteOffset );

// 	Engine::GetInstance()->GetDevice()->IASetInputLayout( myEmittor.myParticleInputLayout);
// 	Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
// 	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBufferWrapper.myStartSlot,myVertexBufferWrapper.myNbrOfBuffers , &(myVertexBufferWrapper.myVertexBuffer), &myVertexBufferWrapper.myStride, &myVertexBufferWrapper.myByteOffset );
// 	Engine::GetInstance()->GetDevice()->IASetIndexBuffer( myIndexBufferWrapper.myIndexBuffer, myIndexBufferWrapper.myIndexBufferFormat ,myIndexBufferWrapper.myByteOffset);

	D3D10_TECHNIQUE_DESC techDesc;
	myEmittor.myEffect->GetTechnique()->GetDesc( &techDesc );

	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		Engine::GetInstance()->GetEffectInput().SetDiffuseMap(myEmittor.myTexture);
		myEmittor.myEffect->GetTechnique()->GetPassByIndex( p )->Apply(0);
		Engine::GetInstance()->GetDevice()->DrawIndexed( myParticles.Count(), 0, 0 );
	}
}

bool CPUParticleEmittorInstance::CreateIndexBuffer()
{
	HRESULT hr = S_OK;

	D3D10_BUFFER_DESC vbdesc;
	vbdesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	vbdesc.ByteWidth = myEmittor.myMaxNrOfParticles*sizeof(CPUParticle);
	vbdesc.CPUAccessFlags = 0;
	vbdesc.MiscFlags = 0;
	vbdesc.Usage = D3D10_USAGE_DEFAULT;
	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &vbdesc, NULL, &myIndexBufferWrapper.myIndexBuffer );
	

	return true;
}

bool CPUParticleEmittorInstance::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	D3D10_BUFFER_DESC vbdesc;
	vbdesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbdesc.ByteWidth = myEmittor.myMaxNrOfParticles*sizeof(CPUParticle);
	vbdesc.CPUAccessFlags = 0;
	vbdesc.MiscFlags = 0;
	vbdesc.Usage = D3D10_USAGE_DEFAULT;
	hr = ( Engine::GetInstance()->GetDevice()->CreateBuffer( &vbdesc, NULL, &myVertexBufferWrapper.myVertexBuffer ) );
	myVertexBufferWrapper.myNbrOfBuffers = 1;

	if ( hr == S_FALSE)
	{
		return false;
	}

	return true;
}

void CPUParticleEmittorInstance::Update( const float aElapsedTime )
{
	EmittorUpdate(aElapsedTime);	
	
	EmittParticle(aElapsedTime);
	ParticlesUpdate(aElapsedTime);
	UpdateVertexBuffer();

}

void CPUParticleEmittorInstance::EmittorUpdate( const float aElapsedTime )
{
	myProcessTime += aElapsedTime;
}

void CPUParticleEmittorInstance::EmittParticle( const float aElapsedTime )
{
// 	const int nbrOfParticles = myParticles.Count();
// 	int targetParticleNbr = (myProcessTime*myEmittor.myEmissionRate);
// 	if ( nbrOfParticles < targetParticleNbr)
// 	{
// 
// 	}
	int particlesToEmitt = aElapsedTime / myEmittor.myEmissionRate;
	while ( particlesToEmitt > 0 )
	{
		myParticles.Add( CPUParticle() );
		myParticleIndexes.Add( myParticles.Count() );
		particlesToEmitt--;
	}	
}

void CPUParticleEmittorInstance::ParticlesUpdate( const float aElapsedTime )
{
	const int nbrOfParticles = myParticles.Count();
	for (int particleUpdated = 0; particleUpdated < nbrOfParticles; particleUpdated++)
	{
		//myParticles[particleUpdated].myPosition += Vector3f(0,0, (- 0.1 * aElapsedTime));
	}
}

void CPUParticleEmittorInstance::UpdateVertexBuffer()
{
	if(myParticles.Count()>0)
	{
		Engine::GetInstance()->GetDevice()->UpdateSubresource( myVertexBufferWrapper.myVertexBuffer, NULL, NULL, &myParticles[0], 0, 0 );
		//Engine::GetInstance()->GetDevice()->UpdateSubresource( myIndexBufferWrapper.myIndexBuffer, NULL, NULL, &myParticleIndexes[0], 0, 0 );
	}
}
