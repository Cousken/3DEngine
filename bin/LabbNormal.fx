#include "Labb.fxh"
// #include "nvidia.fx"
// 
// // cbuffer POISSON_DISKS
// // {
// // 	float2 poissonDisk[16] = {
// // 	float2( -0.94201624, -0.39906216 ),
// // 	float2( 0.94558609, -0.76890725 ),
// // 	float2( -0.094184101, -0.92938870 ),
// // 	float2( 0.34495938, 0.29387760 ),
// // 	float2( -0.91588581, 0.45771432 ),
// // 	float2( -0.81544232, -0.87912464 ),
// // 	float2( -0.38277543, 0.27676845 ),
// // 	float2( 0.97484398, 0.75648379 ),
// // 	float2( 0.44323325, -0.97511554 ),
// // 	float2( 0.53742981, -0.47373420 ),
// // 	float2( -0.26496911, -0.41893023 ),
// // 	float2( 0.79197514, 0.19090188 ),
// // 	float2( -0.24188840, 0.99706507 ),
// // 	float2( -0.81409955, 0.91437590 ),
// // 	float2( 0.19984126, 0.78641367 ),
// // 	float2( 0.14383161, -0.14100790 )
// // 	};
// // };
// 
// 
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD;
};
 
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float2 Tex : UVTEXMAP;
	float2 Depth : DEPTHVALUE;
    float4 WorldPos : WORLDPOS; 
    float3 Reflection : REFLECTIONVECT;
    float4 currentPos : POSITION0;
	float4 prevPos : POSITION1;
};

struct PS_OUTPUT
{
	float4 RGBColor      : SV_TARGET;  // Pixel color    
    float4 PixelVelocity : COLOR1;  // Pixel velocity 
};
// 
// struct VS_OutputCube
// {
// 	 float4 Pos : SV_POSITION;
// 	 float2 Tex : TEXCOORD1;
// };
// 
// struct GS_OutputCube
// {
// 	 float4 Pos : SV_POSITION;
// 	 float2 Tex : TEXCOORD1;
// 	 uint RTIndex : SV_RenderTargetArrayIndex;
// };
 
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT aInput )
{
	float Waviness = 100;
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 prevPos;
    float4 currentPos;
    
    currentPos = mul( aInput.Pos, World );
    output.WorldPos = currentPos;
    output.Norm = mul( aInput.Norm, World );
    
	currentPos = mul( currentPos, View );
    currentPos = mul( currentPos, Projection );
    output.Pos = currentPos;    
    
    prevPos = mul( aInput.Pos, prevWorld );
    prevPos = mul( prevPos, prevView );
    prevPos = mul( prevPos, prevProjection );
	output.prevPos = prevPos;
	output.currentPos = currentPos;
    
    output.Tex = aInput.Tex;    
    
    float3 vectorToCamera = normalize ( CameraPosition - output.WorldPos );
    output.Reflection = reflect( output.Norm, vectorToCamera );
    
    return output;
}
// 
// VS_OUTPUT VS_Ambient( VS_INPUT aInput )
// {
// 	float Waviness = 100;
//     VS_OUTPUT output = (VS_OUTPUT)0;
//     output.Pos = mul( aInput.Pos, World );
//     output.Norm = mul( aInput.Norm, World );
//     output.WorldPos = output.Pos;
//     
//     float3 vectorToCamera = normalize ( CameraPosition - output.WorldPos );
//     output.Reflection = reflect( output.Norm, vectorToCamera );
// 
//     output.Pos = mul( output.Pos, View );
//     output.Pos = mul( output.Pos, Projection );
//     
//     output.Tex = aInput.Tex;    
//     
//     return output;
// }
// 
// void VS_ShadowVolume( float4 vPos : POSITION,
//                        float3 vNormal : NORMAL,
//                        out float4 oPos : POSITION )
// {
// 	// Compute view space normal
//     float3 N = mul( vNormal, (float3x3)World );
// 
//     // Obtain view space position
//     float4 PosInView = mul( vPos, World );
// 
//     // Light-to-vertex vector in view space
//     float3 LightVecView = PosInView - mul(PointLightPos[0], View);
// 
//     // Perform reverse vertex extrusion
//     // Extrude the vertex away from light if it's facing away from the light.
//     if( dot( N, -LightVecView ) < 0.0f )
//     {
//         if( PosInView.z > PointLightPos[0].z )
//             PosInView.xyz += LightVecView * ( 1000 - PosInView.z ) / LightVecView.z;
//         else
//             PosInView = float4( LightVecView, 0.0f );
// 
//         // Transform the position from view space to homogeneous projection space
//         oPos = mul( PosInView, Projection );
//     } else
//         oPos = mul( vPos, World );
//         oPos = mul( oPos, View );
//         oPos = mul( oPos, Projection );
// }
// 
// struct CreateShadowMap_VSOut
// {
//     float4 Position : POSITION;
//     float2 Depth     : TEXCOORD0;
// };
// 
// VS_OUTPUT VS_FromLight( VS_INPUT aInput )
// {
//     VS_OUTPUT output = (VS_OUTPUT)0;
//     output.Pos = mul( aInput.Pos, World );
//     output.Norm = mul( aInput.Norm, World );
//     output.WorldPos = output.Pos;
//     output.Pos = mul( output.Pos, LightView );
//     output.Pos = mul( output.Pos, LightProjection ); 
// 	output.Tex.xy = float2(output.Pos.z, output.Pos.w);
//     return output;
// }
// 
// VS_OutputCube VS_CubeMap( VS_INPUT someInput )
// {
// 	VS_OutputCube output = (VS_OutputCube)0;
// 
// 	output.Pos = mul ( someInput.Pos, World );
// 
// 	output.Tex = someInput.Tex;
// 
// 	return output;
// }
// 
// //--------------------------------------------------------------------------------------
// // Geometry Shader
// //--------------------------------------------------------------------------------------
// 
// [maxvertexcount(18)]
// void GS_CubeMap( triangle VS_OutputCube input[3], inout TriangleStream< GS_OutputCube > CubeMapStream )
// {
//     for( int face = 0; face < 6; face++ )
//     {
//         GS_OutputCube output;
//         output.RTIndex = face;
//         for( int vertex = 0; vertex < 3; vertex++ )
//         {
//             output.Pos = mul( input[ vertex ].Pos, CubeView[ face ] );//
//             output.Pos = mul( output.Pos, Projection );
//             output.Tex = input[ vertex ].Tex;
// 			
//             CubeMapStream.Append( output );
//         }
//         CubeMapStream.RestartStrip();
//     }
// }
// 
// 
// //--------------------------------------------------------------------------------------
// // Pixel Shader
// //--------------------------------------------------------------------------------------
// PS_OUTPUT PS_CubeMap( VS_OutputCube someInput ) : SV_TARGET
// {
// 	 float4 finalColor = 0.2f;
// 
// 	 finalColor.a = 1;
// 	 
// 	 PS_OUTPUT output = (PS_OUTPUT)0;
// 
// 	 output.RGBColor = DiffuseMap.Sample( samLinear, someInput.Tex ) * finalColor;
// 	 
// 	 return output;
// }
// 
// 
PS_OUTPUT PS( VS_OUTPUT input ) : SV_Target
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.RGBColor = DiffuseMap.Sample( samLinear, input.Tex ) * 0.5f;

	float4 currentPos = input.currentPos;
	float4 previousPos = input.prevPos;
	currentPos /= currentPos.w;
	previousPos /= previousPos.w;
	float2 velocity = (previousPos.xy - currentPos.xy);
	velocity = (velocity/DeltaTime) / float2(400,300);
	output.PixelVelocity = float4(velocity, 0, 1);

	// Calculate the reflection vector
	float3 reflection = reflect( -normalize( CameraPosition - input.WorldPos ), normalize( input.Norm ) );
	float4 reflectedColor = EnviromentMap.Sample( sampleLinearClamp, reflection );
    
	output.RGBColor = output.RGBColor + reflectedColor;
	
	return output;
}

// 
// float4 PS_Ambient( VS_OUTPUT input ) : SV_Target
// {
// 	float4  finalColor=0.0;
// 	float1 ambient = 0.1;
// 	
// 	finalColor += ambient;
//     float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
// 	
// 	return (diffuse);
// }
// 
// float4 PS_FromLight( VS_OUTPUT input ) : SV_Target
// {
// 	return float4(input.Tex.x/input.Tex.y, input.Tex.x/input.Tex.y, input.Tex.x/input.Tex.y, 1.0f);
// }
// 
// float4 PS_Additative_Point( VS_OUTPUT input ) : SV_Target
// {
// 	float4  finalColor=0.0;
// 
// 	for(int i=0; i<NumPointLights; i++)
// 	{
// 		float3 vectorToLight = PointLightPos[i] - input.WorldPos;
// 		float distance = length ( vectorToLight );
// 		float attenuation = max ( 0 , ( 1 - ( distance / PointLightMaxDist[i] ) ) );
// 		finalColor += PointLightCol[i] * saturate ( dot ( normalize ( vectorToLight ), input.Norm ) ) * attenuation;
// 
// 		float3 pointLightDir = normalize ( PointLightPos[i] - input.WorldPos );
// 		float3 halfVector = normalize( normalize(CameraPosition - (-input.WorldPos)) + (pointLightDir) );
// 		float specular = PointLightCol[i] * pow(saturate(dot(halfVector, input.Norm)), 16);
// 		finalColor += specular * attenuation;
// 	}
// 
//     float4 diffuse = DiffuseMap.Sample( samLinear, input.Tex ) * finalColor;
// 	
// 	return (diffuse);
// }
// 
// float4 PS_ShadowVolume() : SV_Target
// {
// 	return float4( 255.0f, 255.0f, 255.0f, 255.0f );
// }
// 
// //--------------------------------------------------------------------------------------
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
// 
// technique10 Render_Ambient
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS_Ambient() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS_Ambient() ) );
//         
//         SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
//     }
// }
// 
// technique10 FromLight
// {
// 	pass PO
// 	{
// 		SetVertexShader( CompileShader( vs_4_0, VS_FromLight() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS_FromLight() ) );
// 		SetDepthStencilState(ZTestLess_DS, 0x00000000);
// 	}
// }
// 
// 
// technique10 Render_Additative_Directional
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS() ) );
//         
//         SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 
//     }
// }
// 
// technique10 Render_Additative_Point
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS() ) );
//         
//         //SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 
//     }
// }
// 
// technique10 Render_Additative_Area
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS() ) );
//         
//         SetBlendState( BlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 
//     }
// }
// 
// technique10 Render_ShadowVolume
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS() ) );
//         
//         SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 
//     }
// }
// 
// technique10 Render_ShadowVolume_SHOW
// {
//     pass P0
//     {
//         SetVertexShader( CompileShader( vs_4_0, VS_ShadowVolume() ) );
//         SetGeometryShader( NULL );
//         SetPixelShader( CompileShader( ps_4_0, PS_ShadowVolume() ) );
//         
//         SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
//         SetDepthStencilState( RenderNonShadowedPixles, 1 ); //state, stencilref
//         SetRasterizerState( DisableCulling );
// 	}
// }
// 
// technique10 Render_CubeShadowBuffer
// {
// 	 pass P0
// 	 {
// 		  SetVertexShader( CompileShader( vs_4_0, VS_CubeMap() ) );
// 		  SetGeometryShader( CompileShader( gs_4_0, GS_CubeMap() ) );
// 		  SetPixelShader( CompileShader( ps_4_0, PS_CubeMap() ) );
// 		  
// 		  SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 		  SetDepthStencilState( EnableDepth, 0 );
// 		  SetRasterizerState( EnableCulling );
// 	 }
// }
// 
// // DEFERRED RENDERING
// 
// //// RENDER DEPTH
// 
// VS_OUTPUT VS_Depth( VS_INPUT input )
// {
// 	 VS_OUTPUT output = (VS_OUTPUT)0;
// 	 output.Pos = mul( input.Pos, World );
// 	 output.Pos = mul( output.Pos, View );
// 	 output.Pos = mul( output.Pos, Projection );
// 	 return output;
// }
// 
// float4 PS_RenderDepth( VS_OUTPUT someInput ) : SV_TARGET
// {
// 	return float4( someInput.Pos.z, 0, 0, 1 );
// }
// 
// technique10 Render_Depth
// {
// 	 pass P0
// 	 {
// 	  SetVertexShader( CompileShader( vs_4_0, VS_Depth() ) );
// 	  SetGeometryShader( NULL );
// 	  SetPixelShader( CompileShader( ps_4_0, PS_RenderDepth() ) );
// 	  
// 	  SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
// 	  SetDepthStencilState( EnableDepth, 0 );
// 	  SetRasterizerState( EnableCulling );
// 	 }
// }
// 

//RENDER DEPTH NORMAL ALBEDO
VS_OUTPUT VS_DepthNormalAlbedo( VS_INPUT input )
{
	 VS_OUTPUT output = (VS_OUTPUT)0;
	 output.WorldPos = input.Pos;
	 output.Pos = mul( input.Pos, World );
	 output.Pos = mul( output.Pos, View );
	 output.Pos = mul( output.Pos, Projection );

	 output.Depth = float2(output.Pos.z, output.Pos.w);

	 output.Norm = mul( input.Norm, World);

	 output.Tex = input.Tex;

	 return output;
}

struct PS_OUTPUT2
{
    // The pixel shader can output 2+ values simulatanously if 
    // d3dcaps.NumSimultaneousRTs > 1
    
    float4 DepthOutput      : COLOR0;   
    float4 NormalOutput		: COLOR1;
	float4 AlbedoOutput		: COLOR2;
};

PS_OUTPUT2 PS_RenderDepthNormal( VS_OUTPUT someInput ) : SV_TARGET
{
	PS_OUTPUT2 output = (PS_OUTPUT2)0;

	const float Depth = someInput.Depth.x / someInput.Depth.y;
	output.DepthOutput = float4(Depth, Depth, Depth, 1.0f);
	output.NormalOutput = float4(someInput.Norm.x, someInput.Norm.y, someInput.Norm.z, 1.0f);
	output.NormalOutput = output.NormalOutput;
	output.AlbedoOutput = DiffuseMap.Sample(samPointClamp, someInput.Tex);

	return output;
}

technique10 Render_DepthNormal
{
	 pass P0
	 {
	  SetVertexShader( CompileShader( vs_4_0, VS_DepthNormalAlbedo() ) );
	  SetGeometryShader( NULL );
	  SetPixelShader( CompileShader( ps_4_0, PS_RenderDepthNormal() ) );
	  
	  SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	  SetDepthStencilState( EnableDepth, 0 );
	  SetRasterizerState( EnableCulling );
	 }
}
//RENDER DEPTH NORMAL ALBEDO