#include "Streak.h"
#include "Engine.h"

Streak::Streak(void)
{
	myEmissionRate = 0.1f;
	myAliveParticles = 0;
	myTimeSinceEmission = 0;
	myPosition = Vector3f(0,0,0);
}

Streak::~Streak(void)
{
}

void Streak::Update( const float aDeltaTime )
{
	myTimeSinceEmission += aDeltaTime;

	while ( myTimeSinceEmission >= myEmissionRate )
	{
		myTimeSinceEmission -= myEmissionRate;
		if ( myAliveParticles < ourMaxParticles)
		{
			myVertex[myAliveParticles].myPos = myPosition;
			myAliveParticles++;
		}		
	}
	
	UpdateBuffers();

}

void Streak::Render()
{
		static const UINT stride = sizeof( VertexPosNorm );
		static const UINT offset = 0;
	
		Engine::GetInstance()->GetDevice()->IASetInputLayout( myVertexLayout );
		Engine::GetInstance()->GetDevice()->IASetVertexBuffers( 0, 1, &myVertexBuffer, 
			&stride, &offset );
		Engine::GetInstance()->GetDevice()->IASetIndexBuffer( myIndexBuffer, 
			DXGI_FORMAT_R32_UINT, 0 );
		Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		Engine::GetInstance()->GetEffectInput().UpdateWorld(Matrix44f::Identity());
		Engine::GetInstance()->GetEffectInput().SetDiffuseMap(myTexture);

		D3D10_TECHNIQUE_DESC techDesc;
		myEffect->GetTechnique( EffectTechniques::STREAK )->GetDesc( &techDesc );

		for( UINT p = 0; p < techDesc.Passes; ++p )
		{		
			myEffect->GetTechnique()->GetPassByIndex( p )->Apply( 0 );
			Engine::GetInstance()->GetDevice()->DrawIndexed( myAliveParticles , 0, 0);
		}
}

bool Streak::Init()
{
	myParticles.Init(ourMaxParticles, 10);
	for (int particleAdded = 0; particleAdded < ourMaxParticles; particleAdded++)
	{
		myParticles.Add(StreakParticle());
	}

	myEffect = Engine::GetInstance()->GetEffectContainer().GetEffect("Streak.fx");
	if (myEffect == NULL)
	{
		return false;
	}
	
	if ( InitInputLayout() == false )
	{
		return false;
	}

	if (InitVertexBuffer() == false)
	{
		return false;
	}

	if (InitiIndexBuffer() == false)
	{
		return false;
	}

	myTexture = Engine::GetInstance()->GetTextureContainer().GetDiffuse("streak.png");
	if ( myTexture == NULL )
	{
		return false;
	}

	return true;
}

bool Streak::InitInputLayout()
{
	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	myEffect->GetTechnique( EffectTechniques::STREAK )->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	HRESULT hr;
	hr = Engine::GetInstance()->GetDevice()->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &myVertexLayout );
	if( FAILED( hr ) )
		return false;

	return true;
}

bool Streak::InitVertexBuffer()
{
	myVertex.Init(ourNbrOfVertex, 1);
	for ( int vertexAdded = 0; vertexAdded < ourNbrOfVertex; vertexAdded++)
	{
		myVertex.Add( VertexPosNorm() );
	}	
	
	HRESULT hr = S_OK;

	// Create vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexPosNormUV ) * myVertex.Count();
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &myVertex[0];
	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bd, &InitData, &myVertexBuffer );
	if( FAILED( hr ) )
		return false;

	return true;
}

bool Streak::InitiIndexBuffer()
{
	myVertexIndexes.Init(ourNbrOfVertexIndex, 1);
	for ( int indexAdded = 0; indexAdded < ourNbrOfVertexIndex; indexAdded++)
	{
		myVertexIndexes.Add(indexAdded);
	}	

	
	HRESULT hr = S_OK;

	// Create index buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( unsigned int ) * myVertexIndexes.Count(); // one particle is a quad so 6 index for 2 polys.
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0; 
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &myVertexIndexes[0];
	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bd, &InitData, &myIndexBuffer );
	if( FAILED( hr ) )
		return hr;	

	return true;
}

void Streak::SetPosition( Vector3f aPosition )
{
	myPosition = aPosition;
}

void Streak::UpdateBuffers()
{
	Engine::GetInstance()->GetDevice()->UpdateSubresource(myVertexBuffer, NULL, NULL, &myVertex[0], 0, 0);
	Engine::GetInstance()->GetDevice()->UpdateSubresource(myIndexBuffer, NULL, NULL, &myVertexIndexes[0], 0, 0);
}

void Streak::Move( Vector3f aOffset )
{
	myPosition += aOffset;
}
