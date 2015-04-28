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
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	finalColor += DirectionalLightColor * saturate(dot((float3)DirectionalLightDirection, Norm));

	//spec stuff below
	float3 halfVector = normalize(normalize(CameraPosition - worldPosition) + (float3)DirectionalLightDirection);
    finalColor += saturate((1.11111f) * pow(dot(halfVector, Norm), 8.0f) * dot(DirectionalLightDirection, Norm));

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
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 vectorToLight = PointLightPosition - worldPosition;
	float distance = length(vectorToLight);
	float attenuation = max (0, (1 - (distance / PointLightMaxDistance)));
	float normalizer = saturate(dot(normalize(vectorToLight), Norm));
	finalColor += PointLightColor * normalizer * attenuation;
	
	//spec stuff
	float3 pointLightDir = normalize(worldPosition - PointLightPosition);
	float3 halfVector = normalize(normalize(CameraPosition - worldPosition) + (pointLightDir));
	float specular = saturate(PointLightColor + float4(0.5f, 0.5f, 0.5f, 1.0f)) * pow(saturate(dot(halfVector, Norm)), 16);
	finalColor += specular * attenuation;

	// projected texture
	float3 toWorld  = worldPosition - PointLightPosition;
	float4 projectionStrength  = ProjectionTextureCube.Sample( samLinear, toWorld );
	finalColor *= projectionStrength;
	
	finalColor *= ThirdShaderResourceView.Sample( samLinear, input.Tex );

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
	finalColor += SpotLightColor * saturate(dot(normalize(vectorToLight), Norm)) * attenuation * angleFalloff;

	float4 worldPositionLight = worldPosition;
	worldPositionLight = mul( worldPositionLight, LightView );
	worldPositionLight = mul( worldPositionLight, LightProjection );
	float2 projectionTex  = worldPositionLight.xy / worldPositionLight.w;
	projectionTex    = projectionTex * 0.5f + float2( 0.5f, 0.5f );
	projectionTex.y    = 1.0f - projectionTex.y;
	float4 projectionStrength  = ProjectionTexture2D.Sample( samPointClamp, projectionTex );

// 	float4 projectionTexCoors = mul(worldPosition, LightView);
//  	projectionTexCoors = mul(projectionTexCoors, LightProjection);
//  	float4 projectionStrength = ProjectionTexture.Sample( sampleLinearClamp, float2( projectionTexCoors.x, projectionTexCoors.y ) );
//  	 	
	finalColor *= projectionStrength;

	finalColor *= ThirdShaderResourceView.Sample( samLinear, input.Tex );

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
		//SetDepthStencilState( FrontFaceCulling, 0 );
		//SetRasterizerState( EnableScissorRect );
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

VS_OUTPUT1 VS_DeferredPointMesh( VS_INPUT1 aInput )
{
	VS_OUTPUT1 output = (VS_OUTPUT1)0;
    output.Pos = mul( aInput.Pos, World );
    output.Norm = mul( aInput.Norm, World );
    output.WorldPos = output.Pos;
    
    float3 vectorToCamera = normalize ( CameraPosition - output.WorldPos );
    output.Reflection = reflect( output.Norm, vectorToCamera );

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
    output.Tex = aInput.Tex; 
	return output;
}

float4 PS_DeferredPointMesh( VS_OUTPUT1 input ) : SV_TARGET
{
	//return float4(0.0f, 1.0f, 0.0f, 1.0f);
	//return float4(0.0f, 1.0f, 0.0f, 1.0f);
	float depth = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float x = input.Tex.x * 2 - 1; 
    float y = (1 - input.Tex.y) * 2 - 1;
    float4 projectedPosition = float4(x, y, depth, 1.0f);
	float4 viewPosition = mul(projectedPosition, invertedView);
	float4 worldPosition = viewPosition / viewPosition.w; 

	const float3 Norm = SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 vectorToLight = PointLightPosition - worldPosition;
	float distance = length(vectorToLight);
	float attenuation = max (0, (1 - (distance / PointLightMaxDistance)));
	float normalizer = saturate(dot(normalize(vectorToLight), Norm));
	finalColor += PointLightColor * normalizer * attenuation;
	
	//spec stuff
	float3 pointLightDir = normalize(worldPosition - PointLightPosition);
	float3 halfVector = normalize(normalize(CameraPosition - worldPosition) + (pointLightDir));
	float specular = saturate(PointLightColor + float4(0.5f, 0.5f, 0.5f, 1.0f)) * pow(saturate(dot(halfVector, Norm)), 16);
	finalColor += specular * attenuation;

	return finalColor;
}

technique10 Render_DeferredPointMesh
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_DeferredPointMesh() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DeferredPointMesh() ) );

		SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetRasterizerState(EnableCulling);
    }
}

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