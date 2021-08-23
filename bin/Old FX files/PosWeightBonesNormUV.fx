#include "Labb.fxh"

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD;
    //float3 Tan : TANGENT;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float2 Tex : UVTEXMAP;
    float4 WorldPos : WORLDPOS; 
    float3 Reflection : REFLECTIONVECT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT aInput )
{
	float Waviness = 100;
    VS_OUTPUT output = (VS_OUTPUT)0;
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

VS_OUTPUT VS_Ambient( VS_INPUT aInput )
{
	float Waviness = 100;
    VS_OUTPUT output = (VS_OUTPUT)0;
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

void VS_ShadowVolume( float4 vPos : POSITION,
                       float3 vNormal : NORMAL,
                       out float4 oPos : POSITION )
{
	// Compute view space normal
    float3 N = mul( vNormal, (float3x3)World );

    // Obtain view space position
    float4 PosInView = mul( vPos, World );

    // Light-to-vertex vector in view space
    float3 LightVecView = PosInView - mul(PointLightPos[0], View);

    // Perform reverse vertex extrusion
    // Extrude the vertex away from light if it's facing away from the light.
    if( dot( N, -LightVecView ) < 0.0f )
    {
        if( PosInView.z > PointLightPos[0].z )
            PosInView.xyz += LightVecView * ( 1000 - PosInView.z ) / LightVecView.z;
        else
            PosInView = float4( LightVecView, 0.0f );

        // Transform the position from view space to homogeneous projection space
        oPos = mul( PosInView, Projection );
    } else
        oPos = mul( vPos, World );
        oPos = mul( oPos, View );
        oPos = mul( oPos, Projection );
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4  finalColor=0.0;
	float1 ambient = 0.1;
	
	finalColor += ambient;

	for(int i=0; i<NumDirectionalLights; i++)
	{
		float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (DirectionalLightDir[i]) );
		
		finalColor += DirectionalLightCol[i] * saturate( dot( (float3)DirectionalLightDir[i],input.Norm));
		float specular = DirectionalLightCol[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
		finalColor += specular;
	}

	for(int i=0; i<NumPointLights; i++)
	{
		float3 vectorToLight = PointLightPos[i] - input.WorldPos;
		float distance = length ( vectorToLight );
		float attenuation = max ( 0 , ( 1 - ( distance / PointLightMaxDist[i] ) ) );
		finalColor += PointLightCol[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation;

		float3 pointLightDir = normalize ( PointLightPos[i] - input.WorldPos );
		float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (pointLightDir) );
		float specular = PointLightCol[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
		finalColor += specular * attenuation;
	}

	for(int spotLightI=0; spotLightI<NumSpotLights; spotLightI++)
	{
		float3 vectorToLight = SpotLightPos[spotLightI] - input.WorldPos;
		float distance = length ( vectorToLight );
		float attenuation = max ( 0 , ( 1 - ( distance / SpotLightMaxDist[spotLightI] ) ) );
		//float angleFalloff = smoothstep ( SpotLightInnerFalloff[spotLightI], SpotLightOuterFalloff[spotLightI], dot ( normalize (-vectorToLight), SpotLightDir[spotLightI] ) );
		float angleFalloff = smoothstep ( cos( 1.0f ) , cos(1.5f), dot ( normalize (-vectorToLight), SpotLightDir[i] ) );
		finalColor += SpotLightCol[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation * angleFalloff;			

		float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (float3)SpotLightDir[i]);
		float specular = SpotLightCol[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
		finalColor += specular * attenuation;
	}

    float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
	
	return (diffuse);
}

float4 PS_Ambient( VS_OUTPUT input ) : SV_Target
{
	float4  finalColor=0.0;
	float1 ambient = 0.1;
	
	finalColor += ambient;
    float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
	
	return (diffuse);
}

float4 PS_Additative_Point( VS_OUTPUT input ) : SV_Target
{
	float4  finalColor=0.0;

	for(int i=0; i<NumPointLights; i++)
	{
		float3 vectorToLight = PointLightPos[i] - input.WorldPos;
		float distance = length ( vectorToLight );
		float attenuation = max ( 0 , ( 1 - ( distance / PointLightMaxDist[i] ) ) );
		finalColor += PointLightCol[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation;

		float3 pointLightDir = normalize ( PointLightPos[i] - input.WorldPos );
		float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (pointLightDir) );
		float specular = PointLightCol[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
		finalColor += specular * attenuation;
	}

    float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
	
	return (diffuse);
}

float4 PS_ShadowVolume() : SV_Target
{
	return float4( 255.0f, 255.0f, 255.0f, 255.0f );
}

//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

    }
}

technique10 Render_Ambient
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Ambient() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Ambient() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 Render_Additative_Directional
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
        SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

    }
}

technique10 Render_Additative_Point
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Additative_Point() ) );
        
        SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

    }
}

technique10 Render_Additative_Area
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
        SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

    }
}

technique10 Render_ShadowVolume
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

    }
}

technique10 Render_ShadowVolume_SHOW
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_ShadowVolume() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ShadowVolume() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( RenderNonShadowedPixles, 1 ); //state, stencilref
        SetRasterizerState( DisableCulling );
	}
}
