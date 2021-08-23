#include "Labb.fxh"

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : UV;
};

struct VS_OUTPUT
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4  finalColor=0.0;
	float1 ambient = 0.1;
	
	finalColor += ambient;

	for(int i=0; i<NumLights; i++)
	{
		if ( LightTypes[i] == 0 )
		{
			finalColor += LightColors[i] * saturate( dot( (float3)LightDirections[i],input.Norm));
		}
		else if ( LightTypes[i] == 1 )
		{
			float3 vectorToLight = LightPositions[i] - input.WorldPos;
			float distance = length ( vectorToLight );
			float attenuation = max ( 0 , ( 1 - ( distance / LightMaxDistances[i] ) ) );
			finalColor += LightColors[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation;
		
			float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (float3)LightDirections[i]);
			float specular = LightColors[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
			finalColor += specular * attenuation;
		}
		else if ( LightTypes[i] == 2 )
		{
			float3 vectorToLight = LightPositions[i] - input.WorldPos;
			float distance = length ( vectorToLight );
			float attenuation = max ( 0 , ( 1 - ( distance / LightMaxDistances[i] ) ) );
			//float angleFalloff = smoothstep ( LightInnerFalloff[i], LightOuterFalloff[i], dot ( normalize (-vectorToLight), LightDirections[i] ) );
			float angleFalloff = smoothstep ( cos( 1.0f ) , cos(1.5f), dot ( normalize (-vectorToLight), LightDirections[i] ) );
			finalColor += LightColors[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation * angleFalloff;			
		
			float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (float3)LightDirections[i]);
			float specular = LightColors[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
			finalColor += specular * attenuation;
		}	
	}
	
	float4 reflection = EnviromentMap.Sample( samLinearClamp, input.Reflection );	
    float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
	
	return (diffuse + reflection);
}


//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

// Own techniques

