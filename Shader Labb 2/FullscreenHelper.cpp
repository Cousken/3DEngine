#include "FullscreenHelper.h"
#include "Engine.h"

FullscreenHelper::FullscreenHelper(void)
{
	myEffect = NULL;
	myVertexLayout = NULL;
	myRenderAllRasterizerState = NULL;
	myRenderBackRasterizerState = NULL;
}

bool FullscreenHelper::InitVertexBuffer()
{
	HRESULT hr = S_OK;

	// Create vertex buffer
	D3D10_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D10_USAGE_DEFAULT;
	bufferDescription.ByteWidth = myVertexDataWrapper.mySize;
	bufferDescription.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = myVertexDataWrapper.myVertexData;

	myVertexBufferWrapper.myStride = myVertexDataWrapper.myStride;

	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bufferDescription, &initData, &myVertexBufferWrapper.myVertexBuffer );
	if( FAILED( hr ) )
		return false;	

 	return true;
}

FullscreenHelper::~FullscreenHelper(void)
{
}

void FullscreenHelper::Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, std::string aTechniqueName, const Vector2f& aWidthAndHeight, const Vector2f& aLeftCornerXY)
{
	D3D10_VIEWPORT vpOld[D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	Engine::GetInstance()->GetDevice()->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	D3D10_VIEWPORT vp;
	vp.Width = aWidthAndHeight.x;
	vp.Height = aWidthAndHeight.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = aLeftCornerXY.x;
	vp.TopLeftY = aLeftCornerXY.y;
	Engine::GetInstance()->GetDevice()->RSSetViewports( 1, &vp );
	Engine::GetInstance()->GetDevice()->IASetInputLayout(myVertexLayout);
	Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBufferWrapper.myStartSlot,
		myVertexBufferWrapper.myNbrOfBuffers,
		&(myVertexBufferWrapper.myVertexBuffer), 
		&myVertexBufferWrapper.myStride, 
		&myVertexBufferWrapper.myByteOffset );

	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(aResourceToRender);

	Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &aTargetView, NULL);

	D3D10_TECHNIQUE_DESC techDesc;
	myEffect->GetEffect()->GetTechniqueByName(aTechniqueName.c_str())->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{

		myEffect->GetEffect()->GetTechniqueByName(aTechniqueName.c_str())->GetPassByIndex( p )->Apply( 0 );
		Engine::GetInstance()->GetDevice()->Draw( 4,0);
	}

	Engine::GetInstance()->GetDevice()->RSSetViewports( nViewPorts, vpOld );

	//Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture( NULL );
	//myEffect->GetEffect()->GetTechniqueByName(aTechniqueName.c_str())->GetPassByIndex( 0 )->Apply( 0 );
}

void FullscreenHelper::Process(ID3D10ShaderResourceView* aResourceToRender, 
							  ID3D10RenderTargetView* aTargetView,
							  EffectTechniques::TechniqueType aTechnique)
{
	D3D10_VIEWPORT vpOld[D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	Engine::GetInstance()->GetDevice()->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	D3D10_VIEWPORT vp;
	vp.Width = Engine::GetInstance()->GetScreeenWidth();
	vp.Height = Engine::GetInstance()->GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	Engine::GetInstance()->GetDevice()->RSSetViewports( 1, &vp );
	Engine::GetInstance()->GetDevice()->IASetInputLayout(myVertexLayout);
	Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBufferWrapper.myStartSlot,
		myVertexBufferWrapper.myNbrOfBuffers,
		&(myVertexBufferWrapper.myVertexBuffer), 
		&myVertexBufferWrapper.myStride, 
		&myVertexBufferWrapper.myByteOffset );

	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(aResourceToRender);

	Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &aTargetView, NULL);


	D3D10_TECHNIQUE_DESC techDesc;
	myEffect->GetTechnique(aTechnique)->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{

		myEffect->GetTechnique(aTechnique)->GetPassByIndex( p )->Apply( 0 );
		Engine::GetInstance()->GetDevice()->Draw( 4,0);
	}

	Engine::GetInstance()->GetDevice()->RSSetViewports( nViewPorts, vpOld );

	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture( NULL );
	myEffect->GetTechnique(aTechnique)->GetPassByIndex( 0 )->Apply( 0 );
}

void FullscreenHelper::Process( ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, EffectTechniques::TechniqueType aTechnique, const Vector2f& aWidthAndHeight, const Vector2f& aTopLeftXY )
{
	D3D10_VIEWPORT vpOld[D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	Engine::GetInstance()->GetDevice()->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	D3D10_VIEWPORT vp;
	vp.Width = aWidthAndHeight.x;
	vp.Height = aWidthAndHeight.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = aTopLeftXY.x;
	vp.TopLeftY = aTopLeftXY.y;
	Engine::GetInstance()->GetDevice()->RSSetViewports( 1, &vp );
	Engine::GetInstance()->GetDevice()->IASetInputLayout(myVertexLayout);
	Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBufferWrapper.myStartSlot,
		myVertexBufferWrapper.myNbrOfBuffers,
		&(myVertexBufferWrapper.myVertexBuffer), 
		&myVertexBufferWrapper.myStride, 
		&myVertexBufferWrapper.myByteOffset );

	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(aResourceToRender);

	Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &aTargetView, NULL);
	
	D3D10_TECHNIQUE_DESC techDesc;
	myEffect->GetTechnique(aTechnique)->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		myEffect->GetTechnique(aTechnique)->GetPassByIndex( p )->Apply( 0 );
		Engine::GetInstance()->GetDevice()->Draw( 4,0);
 	}

	Engine::GetInstance()->GetDevice()->RSSetViewports( nViewPorts, vpOld );
}

void FullscreenHelper::ProcessMeshCulling(Camera &aCamera, Instance* anInstance, EffectTechniques::TechniqueType aTechnique, ID3D10ShaderResourceView* aResourceToRender, ID3D10RenderTargetView* aTargetView, const bool anIsCameraInsideFlag)
{
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture(aResourceToRender);

	Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &aTargetView, NULL);
	if(anIsCameraInsideFlag == true)
	{
		Engine::GetInstance()->GetDevice()->RSSetState(myRenderAllRasterizerState);
	}
	else
	{
		Engine::GetInstance()->GetDevice()->RSSetState(myRenderBackRasterizerState);
	}
	//Engine::GetInstance()->GetDevice()->RSSetState(myRenderBackRasterizerState);
	//Engine::GetInstance()->GetDevice()->OMSetRenderTargets( 1, &aTargetView, Engine::GetInstance()->GetDepthStencil());

	anInstance->Render(aCamera, aTechnique);

	//Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture( NULL );
	//Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture( NULL );
	//Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture( NULL );
	//anInstance->GetModel().GetChildren()[0]->GetEffect()->GetTechnique(aTechnique)->GetPassByIndex(0)->Apply(0);
	//anInstance->GetModel().GetEffect()->GetTechnique(aTechnique)->GetPassByIndex(0)->Apply(0);
}

void FullscreenHelper::ResetPostProcessingTextures()
{
	Engine::GetInstance()->GetEffectInput().SetPrimaryPostProcesingTexture( NULL );
	Engine::GetInstance()->GetEffectInput().SetSecondaryPostProcesingTexture( NULL );
	Engine::GetInstance()->GetEffectInput().SetThirdPostProcesingTexture( NULL );
	myEffect->GetTechnique(EffectTechniques::DEFERRED_POINT)->GetPassByIndex( 0 )->Apply( 0 );
}

void FullscreenHelper::DisableScissorRect()
{
	myEffect->GetEffect()->GetTechniqueByName("Render_DisableScissorRect")->GetPassByIndex(0)->Apply(0);
}

bool FullscreenHelper::Init()
{
	myVertexDataWrapper.myType=VertexTypePosUV;

	myVertexDataWrapper.myNrOfVertexes=4;
	myVertexDataWrapper.myStride=sizeof(VertexPosUV);
	myVertexDataWrapper.mySize=myVertexDataWrapper.myStride*myVertexDataWrapper.myNrOfVertexes;
	myVertexDataWrapper.myVertexData=new  char[myVertexDataWrapper.mySize] ();

	VertexPosUV quad[4];

	quad[0].myPos.Set(-1.0f, 1.0f, 0.5f);
	quad[0].myUV.Set( 0.0f, 0.0f );
	quad[1].myPos.Set( 1.0f, 1.0f, 0.5f);
	quad[1].myUV.Set( 1.0f, 0.0f );
	quad[2].myPos.Set( -1.0f, -1.0f, 0.5f);
	quad[2].myUV.Set( 0.0f, 1.0f );
	quad[3].myPos.Set( 1.0f, -1.0f, 0.5f);
	quad[3].myUV.Set( 1.0f, 1.0f );

	memcpy(myVertexDataWrapper.myVertexData,quad,myVertexDataWrapper.mySize);

	D3D10_PASS_DESC passDescription;
	HRESULT hr;

	myEffect = Engine::GetInstance()->GetEffectContainer().GetEffect("PostProcess.fx");
	myEffect->GetTechnique(EffectTechniques::QUAD)->GetPassByIndex( 0 )->GetDesc( &passDescription );

	D3D10_INPUT_ELEMENT_DESC postProcessLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE( postProcessLayout );

	hr = Engine::GetInstance()->GetDevice()->CreateInputLayout( postProcessLayout, numElements, passDescription.pIAInputSignature,
		passDescription.IAInputSignatureSize, &myVertexLayout );
	if( FAILED( hr ) )
		return false;

	myVertexBufferWrapper.myVertexBuffer	= NULL;
	myVertexBufferWrapper.myByteOffset = 0;
	myVertexBufferWrapper.myNbrOfBuffers = 1;
	myVertexBufferWrapper.myStartSlot = 0;
	myVertexBufferWrapper.myStride = sizeof( VertexPosUV );

	if ( InitVertexBuffer() == false )
	{
		return false;
	}
//1	D3D10_RASTERIZER_DESC rasterizerState;
//02	rasterizerState.CullMode = D3D10_CULL_NONE;
//03	rasterizerState.FillMode = D3D10_FILL_SOLID;
//04	rasterizerState.FrontCounterClockwise = true;
//05	rasterizerState.DepthBias = false;
//06	rasterizerState.DepthBiasClamp = 0;
//07	rasterizerState.SlopeScaledDepthBias = 0;
//08	rasterizerState.DepthClipEnable = true;
//09	rasterizerState.ScissorEnable = false;
//10	rasterizerState.MultisampleEnable = false;
//11	rasterizerState.AntialiasedLineEnable = true;
//12	 
//13	ID3D10RasterizerState* pRS;
//14	pD3DDevice->CreateRasterizerState( &rasterizerState, &pRS);

	D3D10_RASTERIZER_DESC renderAllRasterizerDesc;
	renderAllRasterizerDesc.CullMode = D3D10_CULL_NONE;
	renderAllRasterizerDesc.FillMode = D3D10_FILL_SOLID;
	renderAllRasterizerDesc.FrontCounterClockwise = true;
	renderAllRasterizerDesc.DepthBias = false;
	renderAllRasterizerDesc.DepthBiasClamp = 0;
	renderAllRasterizerDesc.SlopeScaledDepthBias = 0;
	renderAllRasterizerDesc.DepthClipEnable = true;
	renderAllRasterizerDesc.ScissorEnable = false;
	renderAllRasterizerDesc.MultisampleEnable = false;
	renderAllRasterizerDesc.AntialiasedLineEnable = false;
	renderAllRasterizerDesc.ScissorEnable = true;
	Engine::GetInstance()->GetDevice()->CreateRasterizerState(&renderAllRasterizerDesc, &myRenderAllRasterizerState);
	
	D3D10_RASTERIZER_DESC renderBackRasterizerDesc;
	renderBackRasterizerDesc.CullMode = D3D10_CULL_BACK;
	renderBackRasterizerDesc.FillMode = D3D10_FILL_SOLID;
	renderBackRasterizerDesc.FrontCounterClockwise = false;
	renderBackRasterizerDesc.DepthBias = false;
	renderBackRasterizerDesc.DepthBiasClamp = 0;
	renderBackRasterizerDesc.SlopeScaledDepthBias = 0;
	renderBackRasterizerDesc.DepthClipEnable = true;
	renderBackRasterizerDesc.ScissorEnable = false;
	renderBackRasterizerDesc.MultisampleEnable = false;
	renderBackRasterizerDesc.AntialiasedLineEnable = false;
	renderBackRasterizerDesc.ScissorEnable = true;
	Engine::GetInstance()->GetDevice()->CreateRasterizerState(&renderBackRasterizerDesc, &myRenderBackRasterizerState);

	return true;
}

Effect* FullscreenHelper::GetEffect()
{
	return myEffect;
}
