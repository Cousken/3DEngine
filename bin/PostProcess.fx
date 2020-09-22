#include "Labb.fxh"

static const float NumberOfBlurPasses = 12.0f;
static const float BlurStrength = 1.0f;
float lastTime;
float BloomScale = 1.5f;

struct VS_INPUT_PP
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT_PP
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


VS_OUTPUT_PP VS( VS_INPUT_PP input )
{
	VS_OUTPUT_PP output = (VS_OUTPUT_PP)0;
	
	output.Pos = float4(input.Pos,1);
	output.Tex = input.Tex;
	
	return output;
}

float4 PS( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 finalColor = 0;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp , input.Tex );
		 
	return finalColor;
}

float4 PS2( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 finalColor = 0;
	finalColor += PrimaryShaderResourceView.Sample( sampleLinearClamp , input.Tex );
		 
	return finalColor;
}

float TonemapA = 0.15;
float TonemapB = 0.50;
float TonemapC = 0.10;
float TonemapD = 0.20;
float TonemapE = 0.02;
float TonemapF = 0.30;
float TonemapW = 11.2;

float3 TonemapTexel(float3 x)
{
	return ((x*(TonemapA*x+TonemapC*TonemapB)+TonemapD*TonemapE)/(x*(TonemapA*x+TonemapB)+TonemapD*TonemapF))-TonemapE/TonemapF;
}

float CalculateColorLuminanceValue( float4 aColor )
{
	return aColor.r * 0.212671 + aColor.g * 0.715160 + aColor.b * 0.072169;
}

float4 PS_ToneMapping( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 finalColor = 0;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp , input.Tex );
	
	float luminance = CalculateColorLuminanceValue( SecondaryShaderResourceView.Sample( samPointClamp , float2( 0, 0 ) ) );
	luminance = max( luminance , 0.0001f );
 
	float exposure = 1.0f;
	float ExposureBias = 2;

	finalColor.rgb = finalColor * ( CalculateColorLuminanceValue( finalColor ) * exposure ) / luminance;
	finalColor.rgb = ExposureBias * TonemapTexel( finalColor );
	finalColor.rgb = finalColor * ( 1.0 / TonemapTexel( TonemapW ) );
	return finalColor;
}

float4 PS_Downsample( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 finalColor = 0;

	float2 textCoords = input.Tex;
	textCoords.x -= texelSize.x;
	textCoords.y -= texelSize.y;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );
	
	textCoords.x += texelSize.x;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );

	textCoords.x += texelSize.x;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );
	
	textCoords.y += texelSize.y;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );

	textCoords.x -= 2 * texelSize.x;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );

	textCoords.y += texelSize.y;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );
	
	textCoords.x += texelSize.x;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );
	
	textCoords.x += texelSize.x;
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, textCoords );
	
	finalColor /= 8;
	 
	return finalColor;
}

float4 PS_BlurHorizontal( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 Color = 0;

    for (int i = 0; i < 31*2+1; i++)
    {    
        //Color +=  PrimaryShaderResourceView.Sample( samLinearClamp, input.Tex + float2(0, HorTextureOffset[i])) * ColorWeights[i];
    }

    return Color * BloomScale;
}

float4 PS_BlurVertical( VS_OUTPUT_PP input ) : SV_TARGET
{
	float4 Color = 0;

    for (int i = 0; i < 31*2+1; i++)
    {    
        //Color +=  PrimaryShaderResourceView.Sample( samLinearClamp, input.Tex + float2(VertTextureOffset[i], 0)) * ColorWeights[i];
    }

    return Color * BloomScale;
}

float4 PS_Motion_Blur( VS_OUTPUT_PP input ) : SV_TARGET
{		
	float2 velocity = CurrentVelocityTexture.Sample( samPointClamp, input.Tex);
	
	velocity *= 2;
	
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	input.Tex += velocity;
	
	for(int i = 1; i < NumberOfBlurPasses; ++i, input.Tex  += velocity)  
	{
		float4 currentColor = PrimaryShaderResourceView.Sample( sampleLinearClamp, input.Tex );
		finalColor += currentColor;
	}
	finalColor=finalColor/NumberOfBlurPasses;

	finalColor.a=1;
	return finalColor;
}

technique10 Render_Quad_Downsample
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Downsample() ) );
    }
}

technique10 Render_Quad
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 Render_Quad_Linear
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS2() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 Render_Quad_ToneMapping
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ToneMapping() ) );
    }
}


technique10 PP_Motion_Blur
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Motion_Blur() ) );
	}
}

technique10 BlurHorizontal
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BlurHorizontal() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( EnableCulling );
    }   
}

technique10 BlurVertical
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BlurVertical() ) );
        
        SetBlendState(OpaqueRendering,float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( EnableCulling );
    }   
}

//// RECONSTRUCT WORLD

VS_OUTPUT_PP VS_RW( VS_INPUT_PP input )
{
	VS_OUTPUT_PP output = (VS_OUTPUT_PP)0;
	
	output.Pos = float4(input.Pos,1);
	output.Tex = input.Tex;
	
	return output;
}

float4 PS_RW( VS_OUTPUT_PP input ) : SV_TARGET
{
	float zOverW = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex ).x;
	float4 H = float4(input.Tex.x * 2 - 1, (1 - input.Tex.y) * 2 - 1, zOverW, 1);
	//float4 D = mul(H, invertedProjection); 
	float4 D = mul(H, invertedView); 
	//D = mul(H, invertedView); 
	float4 worldPos = float4(D.xyz / D.w, 1.0f);
	//worldPos.w = 1.0f;

	//float4 stuff = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	//return stuff;
    return worldPos;
}


technique10 Reconstruct_World
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_RW() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RW() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

////DeferredDirectional

VS_OUTPUT_PP VS_DeferredDirectional( VS_INPUT_PP input )
{
	VS_OUTPUT_PP output = (VS_OUTPUT_PP)0;
	
	output.Pos = float4(input.Pos,1);
	output.Tex = input.Tex;
	
	return output;
}

float4 PS_DeferredDirectional( VS_OUTPUT_PP input ) : SV_TARGET
{
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

	float depth = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float x = input.Tex.x * 2 - 1; 
    float y = (1 - input.Tex.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w; 

	const float3 Norm = SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float4 finalColor = float4(0.035f, 0.035f, 0.035f, 1.0f);
	finalColor += DirectionalLightColor * saturate(dot((float3)DirectionalLightDirection, Norm));

	finalColor *= ThirdShaderResourceView.Sample(samLinear, input.Tex) * 0.25f;
	
	//Calculate the reflection vector
	//float3 reflection = reflect( -normalize( CameraPosition - worldPosition ), normalize( Norm ) );
	//float4 reflectedColor = EnviromentMap.Sample( sampleLinearClamp, reflection );
	//finalColor += reflectedColor;
	
	//spec stuff below
	//float3 halfVector = normalize(normalize(CameraPosition - worldPosition) + (float3)DirectionalLightDirection);
    //finalColor += saturate((1.11111f) * pow(dot(halfVector, Norm), 256.0f) * dot(DirectionalLightDirection, Norm));

	//spec stuff
	//const float substance = 1.0f;
	//const float roughness = 1.0f;
	//const float3 toCameraDirection		= normalize( CameraPosition - worldPosition );
	//const float3 toLightDirection		= normalize( DirectionalLightDirection );
	//const int MaxGloss = 256;
	//const float3 halfVector	= normalize( toCameraDirection + toLightDirection );
	//const float fresnel		= substance + ( 1 - substance ) * pow( 1 - dot( halfVector, toLightDirection ), 5 );
	//const float ap			= pow( MaxGloss, roughness );
	//const float norm		= ( 2 + ap ) / 8;
	//float SpecModifier = 1.0f;
	//finalColor += ( ( norm * pow( abs( dot( halfVector, Norm ) ), abs( ap ) ) * fresnel * saturate( dot( Norm, toLightDirection ) ) * DirectionalLightColor) ) * SpecModifier;

	return finalColor;
}


technique10 Render_DeferredDirectional
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredDirectional() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredDirectional() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetBlendState( AdditiveRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetDepthStencilState( DisableDepth, 0 );
		//SetRasterizerState( DisableCulling );
    }
}

////DeferredPoint

VS_OUTPUT_PP VS_DeferredPoint( VS_INPUT_PP input )
{
	VS_OUTPUT_PP output = (VS_OUTPUT_PP)0;
	
	output.Pos = float4(input.Pos,1);
	output.Tex = input.Tex;
	
	return output;
}

float4 PS_DeferredPoint( VS_OUTPUT_PP input ) : SV_TARGET
{
	float depth = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float x = input.Tex.x * 2 - 1; 
    float y = (1 - input.Tex.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w; 

	const float3 Norm = SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );

	float3 vectorToLight = PointLightPosition - worldPosition;
	float distance = length(vectorToLight);
	float attenuation = max(0.0f, (1.0f - (distance / PointLightMaxDistance)));
	float normalizer = saturate(dot(normalize(vectorToLight), Norm));
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	finalColor += PointLightColor;
	finalColor *= normalizer;
	finalColor *= attenuation;
	finalColor *= ThirdShaderResourceView.Sample( samLinear, input.Tex );
	
	//projected texture
	//float3 toWorld  = worldPosition - PointLightPosition;
	//float4 projectionStrength  = ProjectionTextureCube.Sample( samLinear, toWorld );
	//finalColor *= projectionStrength;

	//spec stuff
	const float substance = 1.0f;
	const float roughness = 1.0f;
	const float3 toCameraDirection		= normalize( CameraPosition - worldPosition );
	const float3 toLightDirection		= normalize( PointLightPosition.xyz - worldPosition.xyz );
	const int MaxGloss = 256;
	const float3 halfVector	= normalize( toCameraDirection + toLightDirection );
	const float fresnel		= substance + ( 1 - substance ) * pow( 1 - dot( halfVector, vectorToLight ), 5 );
	const float ap			= pow( MaxGloss, roughness );
	const float norm		= ( 2 + ap ) / 8;
	float SpecModifier = 0.015f;
	float4 specValue = ( ( norm * pow( abs( dot( halfVector, Norm ) ), abs( ap ) ) * fresnel * saturate( dot( Norm, vectorToLight ) ) * PointLightColor) ) * SpecModifier;
	finalColor += specValue * attenuation;
	finalColor.a = 1.0f;
	return finalColor;
}

technique10 Render_DeferredPoint
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPoint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredPoint() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetBlendState( AdditiveRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetDepthStencilState( DisableDepth, 0 );
		//SetRasterizerState( DisableCulling );
    }
}

float4 PS_DeferredSpot( VS_OUTPUT_PP input ) : SV_TARGET
{
	float depth = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float x = input.Tex.x * 2 - 1; 
    float y = (1 - input.Tex.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w; 
	
	const float3 Norm = SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 vectorToLight = SpotLightPosition - worldPosition;
	float distance = length(vectorToLight);
	float attenuation = max(0 , (1 - (distance / SpotLightMaxDistance)));
	float angleFalloff = smoothstep(cos(SpotLightOuterFalloff), cos(SpotLightInnerFalloff), dot(normalize(-vectorToLight), SpotLightDirection));
	finalColor += SpotLightColor * saturate(dot(normalize(vectorToLight), Norm));

	//float4 worldPositionLight = worldPosition;
	//worldPositionLight = mul( worldPositionLight, LightView );
	//worldPositionLight = mul( worldPositionLight, LightProjection );
	//float2 projectionTex  = worldPositionLight.xy / worldPositionLight.w;
	//projectionTex    = projectionTex * 0.5f + float2( 0.5f, 0.5f );
	//projectionTex.y    = 1.0f - projectionTex.y;
	//float4 projectionStrength  = ProjectionTexture2D.Sample( samPointClamp, projectionTex );
	 	
	//finalColor *= projectionStrength;

	finalColor *= ThirdShaderResourceView.Sample( samLinear, input.Tex );

	//spec stuff
	const float substance = 1.0f;
	const float roughness = 1.0f;
	const float3 toCameraDirection	= normalize( CameraPosition - worldPosition );
	const float3 toLightDirection		= normalize( SpotLightPosition.xyz - worldPosition.xyz );
	const int MaxGloss = 256;
	const float3 halfVector	= normalize( toCameraDirection + toLightDirection );
	const float fresnel		= substance + ( 1 - substance ) * pow( 1 - dot( halfVector, vectorToLight ), 5 );
	const float ap			= pow( MaxGloss, roughness );
	const float norm		= ( 2 + ap ) / 8;
	float SpecModifier = 0.015f;
	finalColor += ( ( norm * pow( abs( dot( halfVector, Norm ) ), abs( ap ) ) * fresnel * saturate( dot( Norm, vectorToLight ) ) * SpotLightColor) ) * SpecModifier;
	finalColor *= attenuation * angleFalloff;

	//shadow stuff
	float4 lightingPosition = mul(worldPosition, LightView);
	float ourdepth = (lightingPosition.z / SpotLightMaxDistance);
	lightingPosition = mul(lightingPosition, LightProjection);

	float2 ShadowTexCoord = lightingPosition.xy / lightingPosition.w;
	ShadowTexCoord = ShadowTexCoord * 0.5 + float2( 0.5, 0.5 );
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
	
	float lightDepthFromScreen = ShadowTexture.Sample(sampleLinearPointClamp, ShadowTexCoord) + 0.01f;//sampleLinearPointClamp
		
	if(ourdepth > lightDepthFromScreen)
	{
		//return float4(lightDepthFromScreen, 0.0f, 0.0f, 1.0f);
		////emulate bilinear filtering (not needed if your GPU can filter FP32 textures)SHADOW_MAP_SIZE
		const float2 texelSize = float2(800.0f, 800.0f);
		const float bias = 0.01f;
		const float darkfactor = 0.12f;
		const float receiver = distance - bias;
	
		float2 unnormalized = ShadowTexCoord * texelSize;
		float2 fractional = frac(unnormalized); 
		unnormalized = floor(unnormalized);

		float4 exponent;         
		exponent.x = ShadowTexture.Sample(sampleLinearPointClamp,(unnormalized + float2( -0.5f, 0.5f )) / texelSize );
		exponent.y = ShadowTexture.Sample(sampleLinearPointClamp,(unnormalized + float2( 0.5f, 0.5f )) / texelSize );
		exponent.z = ShadowTexture.Sample(sampleLinearPointClamp,(unnormalized + float2( 0.5f, -0.5f )) / texelSize );
		exponent.w = ShadowTexture.Sample(sampleLinearPointClamp,(unnormalized + float2( -0.5f, -0.5f )) / texelSize );

		float4 occluder4 = exponent; 

		float occluder = (occluder4.w + (occluder4.x - occluder4.w) * fractional.y);
		occluder = occluder + ((occluder4.z + (occluder4.y - occluder4.z) * fractional.y) - occluder)*fractional.x;    
		
		finalColor = finalColor * saturate(exp(darkfactor * ( occluder - receiver ))) ;  
	}
	
	//finalColor.a = 1.0f;
	
	return finalColor;
}

float4 PS_DeferredSpotNoSpec( VS_OUTPUT_PP input ) : SV_TARGET
{
	float depth = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float x = input.Tex.x * 2 - 1; 
    float y = (1 - input.Tex.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w; 
	
	const float3 Norm = SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 vectorToLight = SpotLightPosition - worldPosition;
	float distance = length(vectorToLight);
	float attenuation = max(0 , (1 - (distance / SpotLightMaxDistance)));
	float angleFalloff = smoothstep(cos(SpotLightOuterFalloff), cos(SpotLightInnerFalloff), dot(normalize(-vectorToLight), SpotLightDirection));
	finalColor += SpotLightColor * saturate(dot(normalize(vectorToLight), Norm));
	finalColor *= attenuation * angleFalloff;
	//finalColor *= ThirdShaderResourceView.Sample( samLinear, input.Tex );

	return finalColor;
}

technique10 Render_DeferredSpot
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPoint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredSpot() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 Render_DeferredSpotNoSpec
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPoint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredSpotNoSpec() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 Render_DeferredPointScissored
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPoint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredPoint() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( EnableScissorRect );
    }
}
technique10 Render_DeferredSpotScissored
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPoint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredSpot() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( EnableScissorRect );
    }
}
technique10 Render_DisableScissorRect
{
	pass P0
	{
		SetVertexShader( NULL );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );
		SetRasterizerState( DisableScissorRect );
	}
}

struct VS_INPUT1
{
	float4 Pos : POSITION;
	float4 Weights : WEIGHTS;
	uint4 Bones : BONES;
    float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
	float3 Tan : TANGENT;
};

struct VS_OUTPUT1
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float2 Tex : UVTEXMAP;
    float4 WorldPos : WORLDPOS; 
    float3 Reflection : REFLECTIONVECT;
};

//SIMPLE SSAO
float random_size = 0.1f;
float g_sample_rad = 0.9f;
float g_intensity = 5.0f;
float g_scale = 0.5f;
float g_bias = 0.2f;
float2 g_screen_size = float2(800.0f, 800.0f);

VS_OUTPUT_PP VS_SSAO( VS_INPUT_PP input )
{
	VS_OUTPUT_PP output = (VS_OUTPUT_PP)0;
	
	output.Pos = float4(input.Pos,1);
	output.Tex = input.Tex;
	
	return output;
}

float3 getPosition(in float2 uv)
{
	//return tex2D(g_buffer_pos,uv).xyz;
	float depth = PrimaryShaderResourceView.Sample( samPointClamp, uv );
	//depth /= 200.0f;
	float x = uv.x * 2 - 1; 
    float y = (1 - uv.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w;
	worldPosition = mul ( worldPosition, View ); 
	return worldPosition.xyz;
}
float3 getNormal(in float2 uv)
{
	//return normalize(tex2D(g_buffer_norm, uv).xyz * 2.0f - 1.0f);
	float3 result = ThirdShaderResourceView.Sample( samPointClamp, uv ).xyz;
	result = mul( result, View );
	return result;
}
float2 getRandom(in float2 uv)
{
	//return normalize(tex2D(g_random, g_screen_size * uv / random_size).xy * 2.0f - 1.0f);
	return normalize(RandomSSAOTexture.Sample( samPointClamp, uv ));
}
float doAmbientOcclusion(in float2 tcoord,in float2 uv, in float3 p, in float3 cnorm)
{
	float3 diff = getPosition(tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff)*g_scale;
	const float stuff = dot(cnorm,v)-g_bias;
	return max(0.0, stuff) * (1.0 / (1.0 + d)) * g_intensity;
}
float4 PS_SSAO(VS_OUTPUT_PP i) : SV_TARGET
{
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	//PS_OUTPUT_SSAO o = (PS_OUTPUT_SSAO)0;

	//o.color.rgb = 1.0f;
	const float2 vec[4] = {float2(1,0), float2(-1,0),
						   float2(0,1), float2(0,-1)};

	float3 worldPos = getPosition(i.Tex);
	//return float4(worldPos.xyz, 1.0f);
	float3 n = getNormal(i.Tex);
	//return float4(n.xyz, 1.0f);
	float2 rand = getRandom(i.Tex);
	//return float4(rand.x, rand.y, 0.0f, 1.0f);

	float ao = 0.0f;
	float rad = g_sample_rad / worldPos.z;

	//return float4(rad, rad, rad, 1.0f);
	//**SSAO Calculation**//
	int iterations = 4;
	for (int j = 0; j < iterations; ++j)
	{
	  float2 coord1 = reflect(vec[j],rand)*rad;
	  float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707,
							  coord1.x*0.707 + coord1.y*0.707);
	  
	  ao += doAmbientOcclusion(i.Tex,coord1*0.25, worldPos, n);
	  ao += doAmbientOcclusion(i.Tex,coord2*0.5, worldPos, n);
	  ao += doAmbientOcclusion(i.Tex,coord1*0.75, worldPos, n);
	  ao += doAmbientOcclusion(i.Tex,coord2, worldPos, n);
	}
	ao/=(float)iterations*4.0;
	ao = 1.0f - ao;
	//ao = saturate(ao);
	//**END**//

	//Do stuff here with your occlusion value “ao”: modulate ambient lighting, write it to a buffer for later //use, etc.
	float4 valueToReturn = float4(ao, ao, ao, 1.0f);
	return normalize(valueToReturn);
}

technique10 Render_SSAO
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_SSAO() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetRasterizerState(EnableCulling);
    }
}

const int JaffeBlurStrength = 15;//500
const int JaffeBlurDetail = 5;

float BlurModifiers[5] = 
{
	0.1959522f,	
	0.1749486f,	
	0.1245063f,	
	0.0706305f,	
	0.0319385f
};

float4 PS_JaffeBlurVertical(VS_OUTPUT_PP input) : SV_TARGET
{	
	const float resolution = 60.0f;
	
	float4 color = PrimaryShaderResourceView.Sample(samPointClamp, input.Tex) * BlurModifiers[0];
    for (int i = 1; i < JaffeBlurStrength; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2(0, (float)i / resolution)) * BlurModifiers[i / (JaffeBlurStrength / JaffeBlurDetail)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2(0, (float)i / resolution)) * BlurModifiers[i / (JaffeBlurStrength / JaffeBlurDetail)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrength / JaffeBlurDetail) * 1.0f;//1.0f
	
	float4 depth = SecondaryShaderResourceView.Sample(samPointClamp, input.Tex);
	if(depth.x < 0.0f)
	{
		color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

    return color;
}

float4 PS_JaffeBlurHorizontal(VS_OUTPUT_PP input) : SV_TARGET
{
	const float resolution = 60.0f;
	
	float4 color = PrimaryShaderResourceView.Sample(samPointClamp, input.Tex) * BlurModifiers[0];
    for (int i = 1; i < JaffeBlurStrength; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2((float)i / resolution, 0)) * BlurModifiers[i / (JaffeBlurStrength / JaffeBlurDetail)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2((float)i / resolution, 0)) * BlurModifiers[i / (JaffeBlurStrength / JaffeBlurDetail)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrength / JaffeBlurDetail) * 2.0f;
	
	float4 depth = SecondaryShaderResourceView.Sample(samPointClamp, input.Tex);
	if(depth.x < 0.0f)
	{
		color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
    return color;
}

technique10 JaffeBlurVertical
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeBlurVertical() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 JaffeBlurHorizontal
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeBlurHorizontal() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

float4 PS_JaffeCopyFromCrossCenter(VS_OUTPUT_PP input) : SV_TARGET
{
	float2 texCoord = input.Tex / 3.0f;
	texCoord += float2(0.332f, 0.332f);
	float4 color = PrimaryShaderResourceView.Sample(samPointClamp, texCoord);
	return color;
}	

technique10 JaffeCopyFromCrossCenter
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeCopyFromCrossCenter() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

float4 PS_RenderAmbientPass(VS_OUTPUT_PP input) : SV_TARGET
{
	float4 albedo = PrimaryShaderResourceView.Sample(samLinear, input.Tex);
	float4 ambient = SecondaryShaderResourceView.Sample(samLinear, input.Tex);
	float4 finalColor = albedo * ambient;
	return finalColor;
}

technique10 RenderAmbientPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RenderAmbientPass() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( EnableCulling );
    }
}

float4 PS_JaffeDownSample(VS_OUTPUT_PP input) : SV_TARGET
{
	return PrimaryShaderResourceView.Sample(samLinear, input.Tex);
}

technique10 JaffeDownSample
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeDownSample() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

const int JaffeBlurStrengthSpecial = 10;
const int JaffeBlurDetailSpecial = 10;

float BlurModifiersSpecial[10] = 
{
	0.2f,
	0.16f,
	0.11f,
	0.07f,
	0.035f,
	0.001f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
};

float4 PS_JaffeSpecialBlurHorizontal(VS_OUTPUT_PP input) : SV_TARGET
{	
	const float resolution = 60.0f;
	//input.Tex = (input.Tex / 1.0f) + 0.25f;
	
	float4 color = PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex) * BlurModifiersSpecial[0];
    for (int i = 1; i < JaffeBlurStrengthSpecial; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2(0, (float)i / resolution)) * BlurModifiersSpecial[i / (JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2(0, (float)i / resolution)) * BlurModifiersSpecial[i / (JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial) * 2.0f;
	
	float4 depth = SecondaryShaderResourceView.Sample(samPointClamp, input.Tex);
	if(depth.x < 0.0f)
	{
		color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

    return color;
}

float4 PS_JaffeSpecialBlurVertical(VS_OUTPUT_PP input) : SV_TARGET
{	
	const float resolution = 60.0f;
	//input.Tex = (input.Tex / 2.0f) + 0.25f;
	
	float4 color = PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex) * BlurModifiersSpecial[0];
    for (int i = 1; i < JaffeBlurStrengthSpecial; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2((float)i / resolution, 0)) * BlurModifiersSpecial[i / (JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2((float)i / resolution, 0)) * BlurModifiersSpecial[i / (JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrengthSpecial / JaffeBlurDetailSpecial) * 1.75f;//0.75f
	
	float4 depth = SecondaryShaderResourceView.Sample(samPointClamp, input.Tex);
	if(depth.x < 0.0f)
	{
		color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

    return color;
}

technique10 JaffeSpecialBlurHorizontal
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeSpecialBlurHorizontal() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

technique10 JaffeSpecialBlurVertical
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeSpecialBlurVertical() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

const int JaffeBlurStrengthSpecial2 = 10;
const int JaffeBlurDetailSpecial2 = 5;

float BlurModifiersSpecial2[5] = 
{
	0.1959522f,	
	0.1749486f,	
	0.1245063f,	
	0.0706305f,	
	0.0319385f
};

float4 PS_JaffeSpecialBlurHorizontal2(VS_OUTPUT_PP input) : SV_TARGET
{	
	const float resolution = 800.0f;
	float4 color = PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex) * BlurModifiersSpecial2[0];
    for (int i = 1; i < JaffeBlurStrengthSpecial2; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2(0, (float)i / resolution)) * BlurModifiersSpecial2[i / (JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2(0, (float)i / resolution)) * BlurModifiersSpecial2[i / (JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2) * 1.0f;
    return color;
}

float4 PS_JaffeSpecialBlurVertical2(VS_OUTPUT_PP input) : SV_TARGET
{	
	const float resolution = 800.0f;
	float4 color = PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex) * BlurModifiersSpecial2[0];
	float4 origColor = color;
    for (int i = 1; i < JaffeBlurStrengthSpecial2; i++)
    {   
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex + float2((float)i / resolution, 0)) * BlurModifiersSpecial2[i / (JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2)];
		color += PrimaryShaderResourceView.Sample(sampleLinearClamp, input.Tex - float2((float)i / resolution, 0)) * BlurModifiersSpecial2[i / (JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2)];
    }
	color.a = 1.0f;
	color /= (float)(JaffeBlurStrengthSpecial2 / JaffeBlurDetailSpecial2) * 1.1f;//0.75f
    return color;
}

technique10 JaffeSpecialBlurHorizontal2
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeSpecialBlurHorizontal2() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

technique10 JaffeSpecialBlurVertical2
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_JaffeSpecialBlurVertical2() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

float4 PS_ComputeBlurDepth(VS_OUTPUT_PP input) : SV_TARGET
{	
	float4 cOut = PrimaryShaderResourceView.Sample(samPointClamp, input.Tex);//albedo
	float depth = SecondaryShaderResourceView.Sample(samPointClamp, input.Tex).x;//depth = viewpos.z / farplane
		
	const float FarPlane = 500.0f;
	float f = 0;
	//float4 vDofParams = float4(1/FarPlane, 4/FarPlane , 35/FarPlane, 1);
	float4 vDofParams = float4(1.0f/FarPlane, 4.0f/FarPlane , 35.0f/FarPlane, 1);
	if( depth < vDofParams.y)
	{
		f = (depth - vDofParams.y) / (vDofParams.y - vDofParams.x);
	}
	else
	{
		f = (depth - vDofParams.y) / (vDofParams.z - vDofParams.y);

		f = clamp(f, 0, vDofParams.w);
	}
	f = (f * 0.5f + 0.5f);
	
	float centerDepth = f;
	cOut.a = centerDepth;
	return (cOut * centerDepth);
}

technique10 ComputeBlurDepth
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ComputeBlurDepth() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
};

float2 poissonDofDisk[8] = 
{
	float2( -0.94201624, -0.39906216 ),
	float2( 0.94558609, -0.76890725 ),
	float2( -0.094184101, -0.92938870 ),
	float2( 0.34495938, 0.29387760 ),
	float2( -0.91588581, 0.45771432 ),
	float2( -0.81544232, -0.87912464 ),
	float2( -0.38277543, 0.27676845 ),
	float2( 0.97484398, 0.75648379 ),
};

float4 PS_DepthOfField(VS_OUTPUT_PP input) : SV_TARGET
{	
	float4 cOut;
	float discRadius, discRadiusLow, centerDepth;
	float2 vMaxCoC = float2(5.0f, 10.0f);
	float radiusScale = 0.4f;//0.4
	
	const float SecondTexelSize = 1.0f / 800.0f;
	const float TexelSize = 1.0f / 800.0f;
	float2 pixelSizeLow = SecondTexelSize;		//SecondTexelSize
	float2 pixelSizeHigh = TexelSize;	//TexelSize
	
	cOut = PrimaryShaderResourceView.Sample(samPointClamp, input.Tex);//albedo
	
	centerDepth = cOut.a;

	discRadius = abs(cOut.a * vMaxCoC.y - vMaxCoC.x);
	discRadiusLow = discRadius * radiusScale;
	cOut = 0;
	for(int i = 0; i < 8; i++)
	{
		//compute tap texture coordinates
		float2 coordLow = input.Tex + (pixelSizeLow * poissonDofDisk[i] * discRadiusLow);
		float2 coordHigh = input.Tex + (pixelSizeHigh * poissonDofDisk[i] * discRadius);

		//fetch high-res tap
		float4 tapLow = SecondaryShaderResourceView.Sample(sampleLinearClamp, coordLow);//second
		float4 tapHigh = PrimaryShaderResourceView.Sample(sampleLinearClamp, coordHigh);//colorTexture
		
		//mix Low- and Hi-res taps based on tap bluriness
		float tapBlur = abs(tapHigh.a * 2.0f - 1.0f);
		float4 tap = lerp(tapHigh, tapLow, tapBlur);
		
		//"smart" blur ignores
		tap.a = (tap.a >= centerDepth) ? 1.0f : abs(tap.a * 2.0 - 1.0);

		cOut.rgb += tap.rgb * tap.a;
		cOut.a += tap.a;
	}
	return (cOut / cOut.a);
}

technique10 RenderDepthOfField
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, VS_SSAO() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DepthOfField() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
};

float log_conv ( float x0, float X, float y0, float Y )
{
    return (X + log(x0 + (y0 * exp(Y - X))));
}

float4 PS_ShadowBlurFunctionX(VS_OUTPUT_PP input) : SV_Target
{
	float2 direction = float2(1,0);
	
    float2 texelSize = 1.0f / float2(800.0, 800.0);
    float2 step = direction * texelSize;
    float2 base = input.Tex - (7.0 - 1.0) * 0.5 * step;
	
	float2 texArray[8];
      
    for (int i = 0; i < 7; i++)
    {
        texArray[i] = base;
        base += step;
    }

    float  sample[7];
    for (int i = 0; i < 7; i++)
    {
		sample[i] = PrimaryShaderResourceView.Sample(sampleLinearClamp,texArray[i]).x;
    }
 
    // 1D Gaussian 7 taps coefficients
    const float coeff[7] = {0.006f, 0.061f, 0.242f, 0.382f, 0.242f, 0.061f, 0.006f};     

    float accum;
    accum = log_conv( coeff[0], sample[0], coeff[1], sample[1] );
    for (int i = 2; i < 7; i++)
    {
        accum = log_conv( 1.f, accum, coeff[i], sample[i] );
    }        
    
    return accum;
}

float4 PS_ShadowBlurFunctionY(VS_OUTPUT_PP input) : SV_Target
{
	float2 direction = float2(0,1);
	
    float2 texelSize = 1.0f / float2(800.0, 800.0);
    float2 step = direction * texelSize;
    float2 base = input.Tex - (7.0 - 1.0) * 0.5 * step;
	float2 texArray[8];
      
    for (int i = 0; i < 7; i++)
    {
        texArray[i] = base;
        base += step;
    }

    float  sample[7];
    for (int i = 0; i < 7; i++)
    {
		sample[i] = PrimaryShaderResourceView.Sample(sampleLinearClamp,texArray[i]).x;
    }
 
    // 1D Gaussian 7 taps coefficients
    const float coeff[7] = {0.006f, 0.061f, 0.242f, 0.382f, 0.242f, 0.061f, 0.006f};     

    float accum;
    accum = log_conv( coeff[0], sample[0], coeff[1], sample[1] );
    for (unsigned int i = 2; i < 7; i++)
    {
        accum = log_conv( 1.f, accum, coeff[i], sample[i] );
    }        
    
    return accum;	
}
technique10 BlurShadowX
{
   pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ShadowBlurFunctionX() ) );
       
		SetBlendState( NoBlending,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		//SetRasterizerState( EnableCulling );
		 //SetDepthStencilState( DisableDepthWrites, 0 );
    }
}
technique10 BlurShadowY
{
   pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ShadowBlurFunctionY() ) );
		
		SetBlendState( NoBlending,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		//SetRasterizerState( EnableCulling );
		 //SetDepthStencilState( DisableDepthWrites, 0 );
    }
}