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
    GSPS_INPUT output = (GSPS_INPUT)0;
    //

    output.Pos = mul( aInput.Pos, World );
    output.Norm = mul( aInput.Norm, World );
    output.WorldPos = output.Pos;
    
    float3 vectorToCamera = normalize ( CameraPosition - output.WorldPos );
    output.Reflection = reflect( output.Norm, vectorToCamera );

//     output.Pos = mul( output.Pos, View );
//     output.Pos = mul( output.Pos, Projection );
    
    output.Tex = aInput.Tex;     
    
    return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(12)]
void GS( triangle GSPS_INPUT input[3], inout TriangleStream<GSPS_INPUT> TriStream )
{
    GSPS_INPUT output = (GSPS_INPUT)0;
    
    float Explode = time * 5;
    float3 fallSpeed;
    fallSpeed.x = 0;
    fallSpeed.y = -10;
    fallSpeed.z = 0;
    
    //
    // Calculate the face normal
    //
    float3 faceEdgeA = input[1].Pos - input[0].Pos;
    float3 faceEdgeB = input[2].Pos - input[0].Pos;
    float3 faceNormal = normalize( cross(faceEdgeA, faceEdgeB) );
    
    float3 ExplodeAmt = faceNormal*Explode;
    ExplodeAmt += fallSpeed * Explode;
    
    
    //
    // Calculate the face center
    //
    float3 centerPos = (input[0].Pos.xyz + input[1].Pos.xyz + input[2].Pos.xyz)/3.0;
    float2 centerTex = (input[0].Tex + input[1].Tex + input[2].Tex)/3.0;
    centerPos += faceNormal;
    
    //
    // Output the pyramid
    //
    for( int i=0; i<3; i++ )
    {
        output.Pos = input[i].Pos + float4(ExplodeAmt,0);
        output.Pos = mul( output.Pos, View );
        output.Pos = mul( output.Pos, Projection );
        output.Norm = input[i].Norm;
        output.Tex = input[i].Tex;
        
         output.Pos.y = max (output.Pos.y, 0);
        TriStream.Append( output );
        
        int iNext = (i+1)%3;
        output.Pos = input[iNext].Pos + float4(ExplodeAmt,0);
        output.Pos = mul( output.Pos, View );
        output.Pos = mul( output.Pos, Projection );
        output.Norm = input[iNext].Norm;
        output.Tex = input[iNext].Tex;
        
         output.Pos.y = max (output.Pos.y, 0);
        TriStream.Append( output );
        
        output.Pos = float4(centerPos,1) + float4(ExplodeAmt,0);
        output.Pos = mul( output.Pos, View );
        output.Pos = mul( output.Pos, Projection );
        output.Norm = faceNormal;
        output.Tex = centerTex;
        
        output.Pos.y = max (output.Pos.y, 0);        
        TriStream.Append( output );
        
        TriStream.RestartStrip();
    }
    
    for( int i=2; i>=0; i-- )
    {
        output.Pos = input[i].Pos + float4(ExplodeAmt,0);
        output.Pos = mul( output.Pos, View );
        output.Pos = mul( output.Pos, Projection );
        output.Norm = -input[i].Norm;
        output.Tex = input[i].Tex;
        
        output.Pos.y = max (output.Pos.y, 0);
        TriStream.Append( output );
    }
	TriStream.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( GSPS_INPUT input ) : SV_Target
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
	
	//float4 reflection = EnviromentMap.Sample( samLinearClamp, input.Reflection );	
    float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
	
	return (diffuse);
}


//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

// Own techniques


