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
    
    float3 tempOutputPos = float3(output.Pos.x, output.Pos.y, output.Pos.z);
   	float3 gravityWell = float3(10, 70, 0);
   	float dist = distance(tempOutputPos, BlackHolePosition);
   	const float maxDistance = 200;
   
	float3 normal = normalize(tempOutputPos - BlackHolePosition);
	output.Pos.x -= normal.x * min(dist, (maxDistance - dist));// * time;
	output.Pos.y -= normal.y * min(dist, (maxDistance - dist));// * time;
	output.Pos.z -= normal.z * min(dist, (maxDistance - dist));// * time;
     
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

		for(int i=0; i<3; i++)
		{
			finalColor += dot((float3)LightDirections[i],input.Norm) * LightColors[i];
			
			float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (float3)LightDirections[i]);
			
			finalColor += dot(input.Norm, halfVector) * LightColors[i];
		}
		
    return DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
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


