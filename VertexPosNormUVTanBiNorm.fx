//--------------------------------------------------------------------------------------
// File: Tutorial06.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "Variables.fxh"

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 UV : TEXCOORD;
	float3 Tan : TANGENT;
	float3 BiNorm : BINORMAL;
};

struct VS_INPUT_INSTANCED
{
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 UV : TEXCOORD;
	float3 Tan : TANGENT;
	float3 BiNorm : BITANGENT;
	float4 World0 : WORLDMATRIX0;
	float4 World1 : WORLDMATRIX1;
	float4 World2 : WORLDMATRIX2;
	float4 World3 : WORLDMATRIX3;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos: WORLDPOSITION;
	float3 Norm : NORMAL0;
	float2 UV : TEXCOORD0;
	float3 Tan : TANGENT0;
	float3 BiNorm : BINORMAL0;
	float4 CurrentPos : POSITION0;
	float4 PreviousPos: PREVPOSITION0;
	float2 Depth : DEPTH0;
};

struct VS_OUTPUT_CUBEMAP
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD1;
};

struct GS_OUTPUT_CUBEMAP
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD1;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

float4x4 decodeMatrix(float3x4 encodedMatrix)
{
	return float4x4(	float4(encodedMatrix[0].xyz,0),
						float4(encodedMatrix[1].xyz,0),
						float4(encodedMatrix[2].xyz,0),
						float4(encodedMatrix[0].w,encodedMatrix[1].w,encodedMatrix[2].w,1));
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	
	output.Tan = mul( input.Tan, World );
	
	output.Norm = mul( input.Norm, World );

	output.BiNorm = mul( input.BiNorm, World );
	
	output.WorldPos = mul(input.Pos, World);
	
    output.CurrentPos = mul( output.WorldPos, View );
	
    output.CurrentPos = mul( output.CurrentPos, Projection );
    
    output.Pos = output.CurrentPos;
    
    output.PreviousPos = mul(input.Pos, World); //mul(input.Pos, LastWorld);
    output.PreviousPos = mul(output.PreviousPos, View); //mul(output.PreviousPos, LastView);
    output.PreviousPos = mul(output.PreviousPos, Projection); //mul(output.PreviousPos, LastProjection);
	
	output.UV = input.UV;

	output.Depth.x = output.Pos.z;
	output.Depth.y = output.Pos.w;
    
    return output;
}

PS_INPUT VS_Instanced( VS_INPUT_INSTANCED input )
{
	PS_INPUT output = (PS_INPUT)0;
	
	Matrix worldMatrix = float4x4( input.World0, input.World1, input.World2, input.World3 );
	
	output.Tan = mul( input.Tan, worldMatrix );
	
	output.Norm = mul( input.Norm, worldMatrix );

	output.BiNorm = mul( input.BiNorm, worldMatrix );
	
	output.WorldPos = mul(input.Pos, worldMatrix);
	
    output.CurrentPos = mul( output.WorldPos, View );
	
    output.CurrentPos = mul( output.CurrentPos, Projection );
    
    output.Pos = output.CurrentPos;
    
    output.PreviousPos = mul(input.Pos, worldMatrix); //mul(input.Pos, LastWorld);
    output.PreviousPos = mul(output.PreviousPos, View); //mul(output.PreviousPos, LastView);
    output.PreviousPos = mul(output.PreviousPos, Projection); //mul(output.PreviousPos, LastProjection);
	
	output.UV = input.UV;
    
    return output;
}

VS_OUTPUT_CUBEMAP VS_Cube( VS_INPUT input )
{
	VS_OUTPUT_CUBEMAP output = (VS_OUTPUT_CUBEMAP)0;

	output.Pos = mul( input.Pos, World );

	output.UV = input.UV;
	
	return output;
}


//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------

[maxvertexcount(18)]
void GS_Cube( triangle VS_OUTPUT_CUBEMAP In[3], inout TriangleStream<GS_OUTPUT_CUBEMAP> CubeMapStream )
{
	//matrix identity = matrix( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
    for( int f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        GS_OUTPUT_CUBEMAP Out;
        Out.RTIndex = f;
        for( int v = 0; v < 3; v++ )
        {
           // Out.Pos = mul( In[v].Pos, identity );
			Out.Pos = mul( In[v].Pos, CubeCamera[f] );
            Out.Pos = mul( Out.Pos, Projection  );
            Out.UV = In[v].UV;
            CubeMapStream.Append( Out );
        }
        CubeMapStream.RestartStrip();
    }
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    float4 finalColor = 0.0f;
	
	float3 norm = NormalMap.Sample( samLinear, input.UV ) * 2 - 1;
	
	input.Norm = normalize( input.Norm );
	input.Tan = normalize( input.Tan );
	
	float3x3 tangentSpaceMatrix = float3x3( input.Tan, input.BiNorm, input.Norm );
	
	norm = normalize( mul( norm, tangentSpaceMatrix ));
	
	input.Norm = norm;
	
	float Roughness = 10;
    
    for(int i=0; i<NumberOfDirectionalLights; i++)
    {
		float normalizer = saturate( dot( (float3)DirectionalLightDirections[i],input.Norm ));
        finalColor += normalizer * DirectionalLightColors[i];
		
		float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( DirectionalLightDirections[i] ));
		finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, input.Norm) , Roughness) * normalizer);
    }
	
	for(int i=0; i<NumberOfPointLights; i++ )
	{
		float4 VectorToLight = PointLightPositions[i] - input.WorldPos;
		float dist = length( VectorToLight );
		float normalizer = saturate( dot( normalize( VectorToLight ), input.Norm ));
		float attenuation = max( 0, 1 - ( dist / PointLightMaxDistance[i] ));
		
		finalColor +=  PointLightColors[i] * normalizer * attenuation;
		
		float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( VectorToLight ));
		finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, input.Norm), Roughness)) * normalizer * attenuation;
	}
	
	for(int i=0; i<NumberOfSpotLights; i++ )
	{
		float4 VectorToLight = SpotLightPositions[i] - input.WorldPos;
		float dist = length( VectorToLight );
		float normalizer = saturate( dot( normalize( VectorToLight ), input.Norm ));
		float attenuation = max( 0, 1 - ( dist / SpotLightMaxDistance[i] ));
		
		float AngleFallOff = smoothstep( SpotLightFallOffOuterAngle[i], SpotLightFallOffInnerAngle[i], dot( -normalize( VectorToLight ), normalize( SpotLightDirections[i] )));
		
		finalColor +=  SpotLightColors[i] * normalizer * attenuation * AngleFallOff;
		
		float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( VectorToLight ));
		finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, input.Norm), Roughness)) * normalizer * attenuation * AngleFallOff;
	}
	
    //float3 reflection = reflect( normalize( input.Norm ), normalize( CameraDir - input.WorldPos ) );
    float3 reflection = reflect( -normalize( CameraDir - input.WorldPos ), normalize( input.Norm ) );
    float4 reflectedColor = EnvMap.Sample( samLinearClamp, reflection );
	
    finalColor.a = 1;
	//return reflectedColor;
    return ( DiffuseTexture.Sample( samLinear, input.UV )) * ( finalColor /*+ reflectedColor*/) * finalColor;
}

PS_OUTPUT PS_Ambient( PS_INPUT input ) : SV_TARGET
{
	float4 finalColor = 0.2f;

	float3 norm = NormalMap.Sample( samLinear, input.UV ) * 2 - 1;
	
	input.Norm = normalize( input.Norm );
	input.Tan = normalize( input.Tan );
	
	float3x3 tangentSpaceMatrix = float3x3( input.Tan, input.BiNorm, input.Norm );
	
	norm = normalize( mul( norm, tangentSpaceMatrix ));
	
	//norm används inte till ngt atm, men finns här om det behövs i framtiden
	
	finalColor.a = 1;
	
	PS_OUTPUT output = (PS_OUTPUT)0;

	float3 reflection = reflect( -normalize( CameraDir - input.WorldPos ), normalize( input.Norm ) );
	float4 reflectedColor = EnvMap.Sample( samLinearClamp, reflection );
	
	//output.RGBColor = reflectedColor;
	//output.RGBColor = ( DiffuseTexture.Sample( samLinear, input.UV )) * ( finalColor + reflectedColor );
	output.RGBColor = reflectedColor + ( DiffuseTexture.Sample( samLinear, input.UV )) * ( finalColor );
	
	float4 velocity = (( input.CurrentPos / input.CurrentPos.w ) - ( input.PreviousPos / input.PreviousPos.w ));
	velocity.x = -velocity.x;
	velocity.z = 0;
	velocity.a = 0;
	
	output.PixelVelocity = velocity;
	
    return output;
}

PS_OUTPUT PS_Cube( VS_OUTPUT_CUBEMAP input ) : SV_TARGET
{
	float4 finalColor = 0.2f;
	
	finalColor.a = 1;
	
	PS_OUTPUT output = (PS_OUTPUT)0;
	
	output.RGBColor = ( DiffuseTexture.Sample( samLinear, input.UV )) * finalColor ;
	
    return output;
}

PS_OUTPUT PS_Depth( PS_INPUT input ) : SV_TARGET
{
	float4 finalColor = mul( input.WorldPos, View ).z * 0.02f;
	
	finalColor.a = 1;
	
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.RGBColor = finalColor;
	
	//output.RGBColor = ( DiffuseTexture.Sample( samLinear, input.UV )) * finalColor ;
	
    	return output;
}

PS_OUTPUT_DEPTHNORMAL PS_DepthNormal( PS_INPUT input ) : SV_TARGET
{
	float4 finalColor = mul( input.WorldPos, View ).z * 0.02f;
	
	finalColor.a = 1;
	
	PS_OUTPUT_DEPTHNORMAL output = (PS_OUTPUT_DEPTHNORMAL)0;

	output.Depth = finalColor;

	output.Normal.xyz = input.Norm;
	output.Normal.w = 1;
	
    	return output;
}

float4 PS_Additive_Directional( PS_INPUT input ) : SV_TARGET
{
	float3 norm = NormalMap.Sample( samLinear, input.UV ) * 2 - 1;
	
	input.Norm = normalize( input.Norm );
	input.Tan = normalize( input.Tan );
	
	float3x3 tangentSpaceMatrix = float3x3( input.Tan, input.BiNorm, input.Norm );
	
	norm = normalize( mul( norm, tangentSpaceMatrix ));
	
	float4 finalColor = 0.0f;

	float Roughness = 10;
    
	float normalizer = saturate( dot( (float3)DirectionalLightDirections[0], norm ));
    finalColor += normalizer * DirectionalLightColors[0];
	
	float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( DirectionalLightDirections[0] ));
	finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, norm) , Roughness)) * normalizer;
    
    finalColor.a = 1;
    
    return ( DiffuseTexture.Sample( samLinear, input.UV )) * finalColor;
}

float4 PS_Additive_Directional_Deferred( PS_INPUT input ) : SV_TARGET
{
	float3 norm = NormalMap.Sample( samLinear, input.UV ) * 2 - 1;
	
	input.Norm = normalize( input.Norm );
	input.Tan = normalize( input.Tan );
	
	float3x3 tangentSpaceMatrix = float3x3( input.Tan, input.BiNorm, input.Norm );
	
	norm = normalize( mul( norm, tangentSpaceMatrix ));
	
	float4 finalColor = 0.0f;

	float Roughness = 10;
    
	float normalizer = saturate( dot( (float3)DirectionalLightDirections[0], norm ));
    finalColor += normalizer * DirectionalLightColors[0];
	
	float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( DirectionalLightDirections[0] ));
	finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, norm) , Roughness)) * normalizer;
    
    finalColor.a = 1;
    
    return ( DiffuseTexture.Sample( samLinear, input.UV )) * finalColor;
}

float4 PS_Additive_Point( PS_INPUT input ) : SV_TARGET
{
	float3 norm = NormalMap.Sample( samLinear, input.UV ) * 2 - 1;
	
	input.Norm = normalize( input.Norm );
	input.Tan = normalize( input.Tan );
	
	float3x3 tangentSpaceMatrix = float3x3( input.Tan, input.BiNorm, input.Norm );
	
	norm = normalize( mul( norm, tangentSpaceMatrix ));
	
	float4 finalColor = 0.0f;

	float Roughness = 10;
    
	float4 VectorToLight = PointLightPositions[0] - input.WorldPos;
	float dist = length( VectorToLight );
	float normalizer = saturate( dot( normalize( VectorToLight ), input.Norm ));
	float attenuation = max( 0, 1 - ( dist / PointLightMaxDistance[0] ));
	
	finalColor +=  PointLightColors[0] * normalizer * attenuation;
	
	float4 halfVector = normalize( normalize( CameraDir - input.WorldPos ) + normalize( VectorToLight ));
	finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, input.Norm), Roughness)) * normalizer * attenuation;
    
    finalColor.a = 1;
    
    return ( DiffuseTexture.Sample( samLinear, input.UV )) * finalColor;
}

//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState( EnableCulling );
    }
}

technique10 Render_Ambient
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Ambient() ) );
        
        SetBlendState( OpaqueRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( EnableCulling );
    }
}

technique10 Render_Ambient_Instanced
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Instanced() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Ambient() ) );
        
        SetBlendState( OpaqueRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( EnableCulling );
    }
}

technique10 Render_Additive_Directional
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Additive_Directional() ) );
        
        SetBlendState( AdditiveRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( RenderNonShadowedPixles, 0 );
        SetRasterizerState( EnableCulling );
    }
}

technique10 Render_Additive_Point
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Additive_Point() ) );
        
        SetBlendState( AdditiveRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( RenderNonShadowedPixles, 0 );
        SetRasterizerState( EnableCulling );
    }
}

technique10 Render_Cube
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Cube() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS_Cube() ) );
		SetPixelShader( CompileShader( ps_4_0, PS_Cube() ) );
		
		SetBlendState( OpaqueRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState( EnableCulling );
	}
}

technique10 Render_Depth
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Depth() ) );
		
		SetBlendState( OpaqueRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState( EnableCulling );
	}
}

technique10 Render_DepthNormal
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_DepthNormal() ) );
		
		SetBlendState( OpaqueRendering, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState( EnableCulling );
	}
}
