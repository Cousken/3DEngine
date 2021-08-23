#include "Labb.fxh"

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : UV;
};

struct GSPS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float4 WorldPos : WORLDPOS;
    float2 Tex : UVTEXMAP;
    float3 Reflection : REFLECTIONVECT;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
GSPS_INPUT VS( VS_INPUT aInput )
{
	float Waviness = 100;
    GSPS_INPUT output = (GSPS_INPUT)0;
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

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( GSPS_INPUT input ) : SV_Target
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

// Own techniques


