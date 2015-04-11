#include "ComputedInstance.h"
#include "Engine.h"

ComputedInstance::~ComputedInstance(void)
{
}	

bool ComputedInstance::Init( )
{
	HRESULT hr = S_OK;



	// Define the input layout

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	myModel.GetEffect()->GetTechnique()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = Engine::GetInstance()->GetDevice()->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &myVertexLayout );

	if( FAILED( hr ) )
		return false;

	if (InitVertexBuffer() == false)
	{
		return false;
	}

	if (InitiIndexBuffer() == false)
	{
		return false;
	}

	return true;
}

bool ComputedInstance::InitVertexBuffer()
{
	HRESULT hr = S_OK;

	// Create vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexPosCol ) * myModel.GetVertexData().Count();
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &myModel.GetVertexData()[0];
	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bd, &InitData, &myVertexBuffer );
	if( FAILED( hr ) )
		return false;

	return true;
}

bool ComputedInstance::InitiIndexBuffer()
{
	HRESULT hr = S_OK;

	// Create index buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( unsigned int ) * myModel.GetVertextIndexes().Count();        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0; 
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &myModel.GetVertextIndexes()[0];
	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bd, &InitData, &myIndexBuffer );
	if( FAILED( hr ) )
		return hr;	

	return true;
}

ComputedInstance::ComputedInstance( ComputedModel& aModel )
: myModel(aModel)
{
	myEffectFile = "";

	myVertexBuffer = 0;
	myVertexLayout = 0;
}

void ComputedInstance::Render( Camera& aCamera )
{
// 	Set the input layout
// 		Engine::GetInstance()->GetDevice()->IASetInputLayout( myVertexLayout );
// 	
// 		// Set vertex buffer
// 		UINT stride = sizeof( VertexPosCol );
// 		UINT offset = 0;
// 		Engine::GetInstance()->GetDevice()->IASetVertexBuffers( 0, 1, &myVertexBuffer, &stride, &offset );
// 	
// 		// Set index buffer
// 		Engine::GetInstance()->GetDevice()->IASetIndexBuffer( myIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
// 	
// 		// Set primitive topology
// 		Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
// 		
// 		myOrientation.SetPosition(myPosition);
// 		myModel.GetEffect()->SetMatrices(myOrientation, aCamera.GetOrientation().Inverse(), aCamera.GetProjection());
// 	
// 		D3D10_TECHNIQUE_DESC techDesc;
// 		myModel.GetEffect()->GetTechnique()->GetDesc( &techDesc );
// 		for( UINT p = 0; p < techDesc.Passes; ++p )
// 		{
// 			myModel.GetEffect()->GetTechnique()->GetPassByIndex( p )->Apply( 0 );
// 			Engine::GetInstance()->GetDevice()->DrawIndexed( myModel.GetVertextIndexes().Count(), 0, 0 );
// 		}
}

void ComputedInstance::SetOrientation( Matrix44f& aOrientation )
{
	myOrientation = aOrientation;
	myPosition = myOrientation.GetPosition();
}

Matrix44f& ComputedInstance::GetOrientation()
{
	myOrientation.SetPosition(myPosition);
	return myOrientation;
}

void ComputedInstance::SetPosition( Vector3f& aPosition )
{
	myPosition = aPosition;
}

void ComputedInstance::GetPosition( Vector3f& aPosition )
{
	aPosition = myPosition;
}

void ComputedInstance::PerformRotation( Matrix33f& aOrientation )
{
	myOrientation = myOrientation * aOrientation;
}

void ComputedInstance::PerformTransformation( Matrix44f& aOrientation )
{
	myOrientation.SetPosition(myPosition);
	myOrientation *= aOrientation;
	myPosition = myOrientation.GetPosition();
}
