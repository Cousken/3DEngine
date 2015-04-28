#define BLOCKER_SEARCH_NUM_SAMPLES 48
#define PCF_NUM_SAMPLES 48
#define NEAR_PLANE 0.1
#define LIGHT_WORLD_SIZE .5
#define LIGHT_FRUSTUM_WIDTH 3.75

// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

float2 g_vFullTexelOffset = float2( 1.0f/800.0f, 1.0f/600.0f );

SamplerState PointSampler
{
	Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float2 ShadowMapTexelKernels[ 48 ] = 
{
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 },
	{ 2, 0 },
	{ 0, 2 },
	{ 2, 1 },
	{ 1, 2 },
	{ 2, 2 },
	{ 3, 0 },
	{ 0, 3 },
	{ 3, 1 },
	{ 1, 3 },
	{ 3, 2 },
	{ 2, 3 },
	{ 3, 3 },
	{ 4, 0 },
	{ 0, 4 },
	{ 4, 1 },
	{ 1, 4 },
	{ 4, 2 },
	{ 2, 4 },
	{ 4, 3 },
	{ 3, 4 },
	{ 4, 4 },
	{ 5, 0 },
	{ 0, 5 },
	{ 5, 1 },
	{ 1, 5 },
	{ 5, 2 },
	{ 2, 5 },
	{ 5, 3 },
	{ 3, 5 },
	{ 5, 4 },
	{ 4, 5 },
	{ 5, 5 },
	{ 6, 0 },
	{ 0, 6 },
	{ 6, 1 },
	{ 1, 6 },
	{ 6, 2 },
	{ 2, 6 },
	{ 6, 3 },
	{ 3, 6 },
	{ 6, 4 },
	{ 4, 6 },
	{ 6, 5 },
	{ 5, 6 },
};

float2 Poisson64[] = {
	{-0.934812, 0.366741},
	{-0.918943, -0.0941496},
	{-0.873226, 0.62389},
	{-0.8352, 0.937803},
	{-0.822138, -0.281655},
	{-0.812983, 0.10416},
	{-0.786126, -0.767632},
	{-0.739494, -0.535813},
	{-0.681692, 0.284707},
	{-0.61742, -0.234535},
	{-0.601184, 0.562426},
	{-0.607105, 0.847591},
	{-0.581835, -0.00485244},
	{-0.554247, -0.771111},
	{-0.483383, -0.976928},
	{-0.476669, -0.395672},
	{-0.439802, 0.362407},
	{-0.409772, -0.175695},
	{-0.367534, 0.102451},
	{-0.35313, 0.58153},
	{-0.341594, -0.737541},
	{-0.275979, 0.981567},
	{-0.230811, 0.305094},
	{-0.221656, 0.751152},
	{-0.214393, -0.0592364},
	{-0.204932, -0.483566},
	{-0.183569, -0.266274},
	{-0.123936, -0.754448},
	{-0.0859096, 0.118625},
	{-0.0610675, 0.460555},
	{-0.0234687, -0.962523},
	{-0.00485244, -0.373394},
	{0.0213324, 0.760247},
	{0.0359813, -0.0834071},
	{0.0877407, -0.730766},
	{0.14597, 0.281045},
	{0.18186, -0.529649},
	{0.188208, -0.289529},
	{0.212928, 0.063509},
	{0.23661, 0.566027},
	{0.266579, 0.867061},
	{0.320597, -0.883358},
	{0.353557, 0.322733},
	{0.404157, -0.651479},
	{0.410443, -0.413068},
	{0.413556, 0.123325},
	{0.46556, -0.176183},
	{0.49266, 0.55388},
	{0.506333, 0.876888},
	{0.535875, -0.885556},
	{0.615894, 0.0703452},
	{0.637135, -0.637623},
	{0.677236, -0.174291},
	{0.67626, 0.7116},
	{0.686331, -0.389935},
	{0.691031, 0.330729},
	{0.715629, 0.999939},
	{0.8493, -0.0485549},
	{0.863582, -0.85229},
	{0.890622, 0.850581},
	{0.898068, 0.633778},
	{0.92053, -0.355693},
	{0.933348, -0.62981},
	{0.95294, 0.156896},
};

cbuffer POISSON_DISKS
{
	float2 poissonDisk[16] = {
	float2( -0.94201624, -0.39906216 ),
	float2( 0.94558609, -0.76890725 ),
	float2( -0.094184101, -0.92938870 ),
	float2( 0.34495938, 0.29387760 ),
	float2( -0.91588581, 0.45771432 ),
	float2( -0.81544232, -0.87912464 ),
	float2( -0.38277543, 0.27676845 ),
	float2( 0.97484398, 0.75648379 ),
	float2( 0.44323325, -0.97511554 ),
	float2( 0.53742981, -0.47373420 ),
	float2( -0.26496911, -0.41893023 ),
	float2( 0.79197514, 0.19090188 ),
	float2( -0.24188840, 0.99706507 ),
	float2( -0.81409955, 0.91437590 ),
	float2( 0.19984126, 0.78641367 ),
	float2( 0.14383161, -0.14100790 )
	};
};

SamplerComparisonState PCF_Sampler 
{ 
	AddressU = Clamp;
	AddressV = Clamp;

	ComparisonFunc = LESS;
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
};

float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
	return (zReceiver - zBlocker) / zBlocker;
}

void FindBlocker(out float avgBlockerDepth,out float numBlockers,float2 uv, float zReceiver )
{
	//This uses similar triangles to compute what //area of the shadow map we should search
	float searchWidth = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver;
	float blockerSum = 0;
	numBlockers = 0;
	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i )
	{
		float shadowMapDepth = PrimaryShaderResourceView.SampleLevel(PointSampler,uv + Poisson64[i] * searchWidth * g_vFullTexelOffset + ShadowMapTexelKernels[ i ] * g_vFullTexelOffset,0);
		if ( shadowMapDepth < zReceiver ) 
		{
			blockerSum += shadowMapDepth;
			numBlockers++;
		}
	}
	avgBlockerDepth = blockerSum / numBlockers;
}

float PCF_Filter( float2 uv, float zReceiver, float filterRadiusUV )
{
	float sum = 0.0f;
	for ( int i = 0; i < PCF_NUM_SAMPLES; ++i )
	{
		float2 offset = Poisson64[i] * filterRadiusUV * g_vFullTexelOffset + ShadowMapTexelKernels[ i ] * g_vFullTexelOffset;
		sum += PrimaryShaderResourceView.SampleCmpLevelZero(PCF_Sampler, uv + offset, zReceiver).r;
	}
	return sum / PCF_NUM_SAMPLES;
}

float PCSS ( Texture2D shadowMapTex, float3 coords )
{
	float2 uv = coords.xy;
	float zReceiver = coords.z;

	float avgBlockerDepth = 0;
	float numBlockers = 0;
	FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver );
	
	if( numBlockers < 1 )
	{	
		//There are no occluders so early out (this saves filtering)
		return 1.0f;
	}

	// STEP 2: penumbra size	
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
	float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / coords.z;
	
	// STEP 3: filtering
	return PCF_Filter( uv, zReceiver, filterRadiusUV );
}