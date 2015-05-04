	
#define MAX_BONE_MATRICES 255

shared Texture2D DiffuseMap;
shared TextureCube EnviromentMap;
shared Texture2D NormalMap;

shared Texture2D PrimaryShaderResourceView;
shared Texture2D SecondaryShaderResourceView;
shared Texture2D ThirdShaderResourceView;
shared Texture2D CurrentVelocityTexture;
shared Texture2D PreviousVelocityTexture;
shared Texture2D RandomSSAOTexture;
shared Texture2D ProjectionTexture2D;
shared TextureCube ProjectionTextureCube;

shared cbuffer StaticShared
{
	matrix View;
	matrix LightView;
	matrix LightProjection;
	matrix prevView;
	matrix invertedView;//touse
}

shared cbuffer OnResolutionChangeShared
{
	matrix Projection;
	matrix invertedProjection;//touse
	matrix prevProjection;
}

shared cbuffer DynamicShared
{
	matrix World;	
	matrix prevWorld;
	
	int NumDirectionalLights;
	int NumPointLights;
	int NumSpotLights;

	float4 DirectionalLightDir[9];
	
	float4 DirectionalLightCol[9];	
	float3 PointLightPos[9];
	float4 PointLightCol[9];
	
	float PointLightMaxDist[9];

	float4 SpotLightColor;	
	float4 SpotLightDirection;
	float3 SpotLightPosition;
	float SpotLightMaxDistance;
	float SpotLightInnerFalloff;
	float SpotLightOuterFalloff;

	float4 DirectionalLightColor;
	float4 DirectionalLightDirection;
	float PointLightMaxDistance;
	float3 PointLightPosition;
	float4 PointLightColor;

	matrix LightViewProjection;

	matrix CubeView[6];
	
	float3 CameraPosition;
	float time;
	float DeltaTime;
	float3 BlackHolePosition;
	float2 texelSize;
}

shared SamplerState samPointClamp 
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

shared SamplerState sampleLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

shared SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

shared BlendState BlendingAdd 
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = ONE; 
	DestBlend = ONE; 
	BlendOp = ADD; 
	SrcBlendAlpha = ZERO; 
	DestBlendAlpha = ZERO; 
	BlendOpAlpha = ADD; 
	RenderTargetWriteMask[0] = 0x0F; 
}; 


shared BlendState BlendingAddAlpha
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA; 
	DestBlend = ONE; 
	BlendOp = ADD; 
	SrcBlendAlpha = ONE; 
	DestBlendAlpha = ONE; 
	BlendOpAlpha = ADD; 
	RenderTargetWriteMask[0] = 0x0F; 
}; 

shared BlendState SrcAlphaBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

shared BlendState AdditiveRendering
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

// // BlendState AlphaBlending
// // {
// // 	BlendEnable[0] = TRUE;
// // 	SrcBlend = SRC_ALPHA; 
// // 	DestBlend = ONE_MINUS_SRC_ALPHA; 
// // 	BlendOp = ADD; 
// // 	SrcBlendAlpha = ONE; 
// // 	DestBlendAlpha = ONE; 
// // 	BlendOpAlpha = ADD; 
// // 	RenderTargetWriteMask[0] = 0x0F; 
// // }; 
// 

shared BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

shared DepthStencilState TwoSidedStencil
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = Less;
    
    // Setup stencil states
    StencilEnable = true;
    StencilReadMask = 0xFFFFFFFF;
    StencilWriteMask = 0xFFFFFFFF;
    
    BackFaceStencilFunc = Always;
    BackFaceStencilDepthFail = Incr;
    BackFaceStencilPass = Keep;
    BackFaceStencilFail = Keep;
    
    FrontFaceStencilFunc = Always;
    FrontFaceStencilDepthFail = Decr;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Keep;
};

shared DepthStencilState RenderNonShadowedPixles
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = Less_Equal;
    
    StencilEnable = true;
    StencilReadMask = 0xFFFFFFFF;
    StencilWriteMask = 0x0;
    
    FrontFaceStencilFunc = Equal;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Zero;
    
    BackFaceStencilFunc = Never;
    BackFaceStencilPass = Zero;
    BackFaceStencilFail = Zero;
};

shared DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = Less;
};

shared DepthStencilState VolumeComplexityStencil
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = Less;
    
    // Setup stencil states
    StencilEnable = true;
    StencilReadMask = 0xFFFFFFFF;
    StencilWriteMask = 0xFFFFFFFF;
    
    BackFaceStencilFunc = Always;
    BackFaceStencilDepthFail = Incr;
    BackFaceStencilPass = Incr;
    BackFaceStencilFail = Incr;
    
    FrontFaceStencilFunc = Always;
    FrontFaceStencilDepthFail = Incr;
    FrontFaceStencilPass = Incr;
    FrontFaceStencilFail = Incr;
};

shared RasterizerState DisableCulling
{
    CullMode = NONE;
	ScissorEnable = FALSE;
};

shared BlendState OpaqueRendering
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

shared DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

shared RasterizerState EnableCulling
{
    CullMode = BACK;
	ScissorEnable = FALSE;
};

shared DepthStencilState ZTestLess_DS
{
    DepthEnable = TRUE;
    DepthFunc = LESS;
    DepthWriteMask = ALL;
};


//------------------------------------------
//OPTIMIZED DEFERRED
//------------------------------------------
shared DepthStencilState FrontFaceCulling
{
	DepthEnable = true;
	DepthWriteMask = ZERO;
	DepthFunc = Greater;
};
shared RasterizerState EnableScissorRect
{
	CullMode = NONE;
    ScissorEnable = TRUE;
};
shared RasterizerState DisableScissorRect
{
	CullMode = BACK;
    ScissorEnable = FALSE;
};