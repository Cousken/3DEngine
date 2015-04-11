//------------------------------------------
//INCLUDES
//------------------------------------------
#include "Variables.fxh"

//------------------------------------------
//HARDCODED VARIABLES
//------------------------------------------
float A = 0.22;
float B = 0.30;
float C = 0.10;
float D = 0.20;
float E = 0.01;
float F = 0.30;
float W = 11.2;

//------------------------------------------
//VARIABLES
//------------------------------------------

Texture2D PrimaryShaderResourceView;
Texture2D SecondaryShaderResourceView;
Texture2D LuminanceMap;

float ScreenSizeX;
float ScreenSizeY;

const int NumberOfBlurPasses = 50;

cbuffer TexelSizes
{
	float2 TexelSize;
	
	float2 DownSampleOffsets[16];
	
	float4 ColorWeights[31*2+1];
	float HorTextureOffset[31*2+1];
	float VertTextureOffset[31*2+1];
}

//------------------------------------------
//STRUCTS
//------------------------------------------
struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

//------------------------------------------
//FUNCTIONS
//------------------------------------------
float3 CalculateFilmicToneMap(float3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float LinearColorToLum(float3 color)
{
	return( color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722 );
}

//------------------------------------------
//VERTEX SHADERS
//------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	output.Pos = float4( input.Pos, 1 );
	output.Tex = input.Tex;
	
	return output;
}

//------------------------------------------
//PIXEL SHADERS
//------------------------------------------
float4 PS_DownSample( VS_OUTPUT input ) : SV_TARGET
{
	float4 finalColor = 0;

	for( int i = 0; i < 16; i++ )
	{
		finalColor += PrimaryShaderResourceView.Sample( samPointClamp, input.Tex + DownSampleOffsets[i] );
	}
	
	finalColor /= 16;
	
	return finalColor;
}

float4 PS_UpSample( VS_OUTPUT input ) : SV_TARGET
{
	return PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
}

float bloomFactor = 1.0f;
float glowFactor = 1.0f;
float bloomThreshHold = 0.1f;

float4 PS_BrightPassFilter( VS_OUTPUT input ) : SV_TARGET
{
	//float4 color = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );	
	
	float3 color = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );	
	
	float lum = LinearColorToLum( color );
	color *= ( lum - bloomThreshHold ) * bloomFactor;
	
	//if(color.a==0)
	//{
	//	color.rgb *= (saturate( lum-bloomThreshHold )) * bloomFactor;
	//}
	//else
	//{
	//	color.rgb *= color.a * glowFactor * 1.41f;
	//}
	
	return float4( saturate( color ), 1 );
}

float BloomScale = 1.5f;

float4 PS_BlurHorizontal( VS_OUTPUT input ) : SV_TARGET
{
	float4 Color = 0;

    for (int i = 0; i < 31*2+1; i++)
    {    
        Color +=  PrimaryShaderResourceView.Sample( samPointClamp, input.Tex + float2(0, HorTextureOffset[i])) * ColorWeights[i];
    }

    return Color * BloomScale;
}

float4 PS_BlurVertical( VS_OUTPUT input ) : SV_TARGET
{
	float4 Color = 0;

    for (int i = 0; i < 31*2+1; i++)
    {    
        Color +=  PrimaryShaderResourceView.Sample( samPointClamp, input.Tex + float2(VertTextureOffset[i], 0)) * ColorWeights[i];
    }

    return Color * BloomScale;
}

float4 PS_Bloom( VS_OUTPUT input ) : SV_TARGET
{
	return float4(0,0,0,0);
}

float4 PS_Combine( VS_OUTPUT input ) : SV_TARGET
{
	float4 finalColor = 0;
	
	finalColor += PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	finalColor += SecondaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	return finalColor;
}

float4 PS_RadialBlur( VS_OUTPUT input ) : SV_TARGET
{
	const float BlurStrength = 25; //30
	
	float2 velocity = float2( ( input.Pos.x / ScreenSizeX ) - 0.5, ( input.Pos.y / ScreenSizeY ) - 0.5 );
	//float2 velocity = float2( 0.5 - ( input.Pos.x / ScreenSizeX ), 0.5 - ( input.Pos.y / ScreenSizeY ) );
	velocity /= BlurStrength;
	
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	velocity /= NumberOfBlurPasses;
	
	for( int i = 1; i < NumberOfBlurPasses; ++i )
	{
		// Sample the color buffer along the velocity vector.  
		float4 currentColor = PrimaryShaderResourceView.Sample( samPointClamp, ( input.Tex + ( velocity * i )));
		// Add the current color to our color sum.  
		finalColor += currentColor;
	}
	
	finalColor /= NumberOfBlurPasses;
	
	finalColor.a = 1;
	
	return finalColor;
}

float4 PS_CameraBasedMotionBlur( VS_OUTPUT input ) : SV_TARGET
{
	const float BlurStrength = 35;

	float zOverW = DepthTexture.Sample( samPointClamp, input.Tex );
	float4 H = float4(input.Tex.x * 2 - 1, (1 - input.Tex.y) * 2 - 1, zOverW, 1);
	float4 D = mul(H, ViewProjectionInverseMatrix); 
	float4 worldPos = D / D.w;
	float4 currentPos = H;
	float4 previousPos = mul(worldPos, PreviousViewProjectionMatrix);
	previousPos /= previousPos.w;
	float2 velocity = (currentPos - previousPos);
	
	velocity.x *=( ScreenSizeY / ScreenSizeX );
	
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	velocity.x = -velocity.x;
	
	velocity /= ( DeltaTime );
	velocity /= BlurStrength;
	
	velocity /= NumberOfBlurPasses;
	
	for( int i = 1; i < NumberOfBlurPasses; ++i )
	{
		// Sample the color buffer along the velocity vector.  
		float4 currentColor = PrimaryShaderResourceView.Sample( samPointClamp, ( input.Tex + ( velocity * i )));
		// Add the current color to our color sum.  
		finalColor += currentColor;  
	}
	
	finalColor /= NumberOfBlurPasses;

	finalColor.a = 1;
	
	return finalColor; 
}

float4 PS_TrueMotionBlur( VS_OUTPUT input ) : SV_TARGET
{
	const float BlurStrength = 35;

	float4 clampedVelocity = VelocityTexture.Sample( samPointClamp, input.Tex );
	float2 velocity = float2( clampedVelocity.x, clampedVelocity.y );
	//velocity.x = 0;
	//velocity.y = 0;
	
	velocity.x *=( ScreenSizeY / ScreenSizeX );
	
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	velocity /= ( DeltaTime );
	velocity /= BlurStrength;
	
	velocity /= NumberOfBlurPasses;
	
	for( int i = 1; i < NumberOfBlurPasses; ++i )
	{
		// Sample the color buffer along the velocity vector.  
		float4 currentColor = PrimaryShaderResourceView.Sample( samPointClamp, ( input.Tex + ( velocity * i )));
		// Add the current color to our color sum.  
		finalColor += currentColor;  
	}
	
	finalColor /= NumberOfBlurPasses;

	finalColor.a = 1;
	
	return finalColor;
}

float4 PS_Blur( VS_OUTPUT input ) : SV_TARGET
{
	const float Thresh = 0.5;
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp , input.Tex );
	
	//finalColor = 1.0 - finalColor;
	
	float lum = LinearColorToLum( finalColor );
	
	finalColor.r = lum;
	finalColor.b = lum;
	finalColor.g = lum;
	
	//finalColor.r *= 3;
	//finalColor.g *= 0.25;
	
	//finalColor.r = saturate( finalColor.r );

	finalColor.a = 1;
	
	return finalColor;
}

float4 PS_Transfer( VS_OUTPUT input ) : SV_TARGET
{
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );
	
	return finalColor;
}

float4 PS_ToneMapFilmic(VS_OUTPUT input) : SV_TARGET
{
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp, input.Tex );

	float luminance = LinearColorToLum( LuminanceMap.Sample( samPointClamp,input.Tex ));
	float exposure = 1;
	float exposureBias = 2;

    finalColor.rgb = finalColor * ( LinearColorToLum( finalColor ) * exposure ) / luminance;
	finalColor.rgb = exposureBias * CalculateFilmicToneMap( finalColor );
	finalColor.rgb = finalColor * ( 1.0 / CalculateFilmicToneMap( W ) );
	return finalColor;	
}

float4 PS_Luminance( VS_OUTPUT input ) : SV_TARGET
{
	float4 finalColor = PrimaryShaderResourceView.Sample( samPointClamp , input.Tex );
	
	float lum = LinearColorToLum( finalColor );
	
	finalColor.r = lum;
	finalColor.b = lum;
	finalColor.g = lum;
	finalColor.a = 1;
	
	return finalColor;
}

float4 PS_Restore( VS_OUTPUT input ) : SV_TARGET
{
	float zOverW = DepthTexture.Sample( samPointClamp, input.Tex );
	float4 H = float4(input.Tex.x * 2 - 1, (1 - input.Tex.y) * 2 - 1, zOverW, 1);
	float4 D = mul(H, ViewProjectionInverseMatrix); 
	float4 worldPos = D / D.w;
	
    	return worldPos;
}

float4 PS_Additive_Directional_Deferred( VS_OUTPUT input ) : SV_TARGET
{
	float3 norm = NormalMap.Sample( samLinear, input.Tex ) * 2 - 1;
	
	float4 finalColor = 0.0f;

	float Roughness = 10;
    
	float normalizer = saturate( dot( (float3)DirectionalLightDirections[0], norm ));
    	finalColor += normalizer * DirectionalLightColors[0];

	float zOverW = DepthTexture.Sample( samPointClamp, input.Tex );
	float4 H = float4(input.Tex.x * 2 - 1, (1 - input.Tex.y) * 2 - 1, zOverW, 1);
	float4 D = mul(H, ViewProjectionInverseMatrix); 
	float4 worldPos = D / D.w;
	
	float4 halfVector = normalize( normalize( CameraDir - worldPos ) + normalize( DirectionalLightDirections[0] ));
	finalColor += saturate((( Roughness + 2 )/8) * pow(dot(halfVector, norm) , Roughness)) * normalizer;
    
    	finalColor.a = 1;
    
   	return ( PrimaryShaderResourceView.Sample( samPointClamp, input.Tex ) );//* finalColor );
}

//------------------------------------------
//TECHNIQUES
//------------------------------------------
technique10 DownSample
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_DownSample() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 UpSample
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_UpSample() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 BrightPass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BrightPassFilter() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
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
        SetRasterizerState( DisableCulling );
    }   
}

technique10 BlurVertical
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_BlurVertical() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Render_Bloom
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Bloom() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Combine
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Combine() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 RadialBlur
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RadialBlur() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 CameraBasedMotionBlur
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_CameraBasedMotionBlur() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 TrueMotionBlur
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_TrueMotionBlur() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Blur
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Blur() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Transfer
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Transfer() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 ToneMapFilmic
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_ToneMapFilmic() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Luminance
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Luminance() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }   
}

technique10 Restore
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Restore() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }
}

technique10 Additive_Directional_Deferred
{
    pass p0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Additive_Directional_Deferred() ) );
        
        SetBlendState( OpaqueRendering,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }
}