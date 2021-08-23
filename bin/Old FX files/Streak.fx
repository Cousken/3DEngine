#include "Labb.fxh"

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
};

struct GSPS_PARTICLEINPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float Size : SIZE;
    float Alpha : ALPHA;
    float Time : TIME;
};

VS_OUTPUT VS_Streaks( VS_INPUT aInput )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Pos = mul( aInput.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = mul( aInput.Norm, World );
    
    return output;
}

[maxvertexcount(4)]
void GS_Streaks(point GSPS_PARTICLEINPUT input[1], inout TriangleStream<GSPS_PARTICLEINPUT> TriStream)
{
	GSPS_PARTICLEINPUT output = input[0];
    
	output.Size = max(0, 0.25f + 0.15f * input[0].Time);
	output.Time = 0.15f;
	 
	output.Pos = input[0].Pos + float4(-0.25f, 0.25f, 0, 0);
	output.Pos = mul(output.Pos, Projection); 
	output.Tex = float2(0.0f, 0.0f);
	TriStream.Append(output);
	    
	output.Pos = input[0].Pos + float4(0.25f, 0.25f, 0, 0);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = float2(1.0f, 0.0f);
	TriStream.Append(output);
	    
	output.Pos = input[0].Pos + float4(-0.25f, -0.25f, 0, 0);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = float2(0.0f, 1.0f);
	TriStream.Append(output);
	   
	output.Pos = input[0].Pos + float4(0.25f, -0.25f, 0, 0);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = float2(1.0f, 1.0f);
	TriStream.Append(output);
	   
	TriStream.RestartStrip();
}

float4 PS_Streaks( VS_OUTPUT input ) : SV_Target
{
	return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}

technique10 Streaks
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Streaks() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Streaks() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

