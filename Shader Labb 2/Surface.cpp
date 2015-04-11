#include "Surface.h"
#include <assert.h>
#include "Engine.h"

Surface::Surface(void)
{
	myTexture				= NULL;
	myEnviromentMap			= NULL;
	myNormalMap				= NULL;
	mySpecularMap			= NULL;
	myAmbientOcclusionMap	= NULL;
}

Surface::Surface( std::string aTextureFileName, std::string aEnviromentMapFileName, D3D10_PRIMITIVE_TOPOLOGY aPrimology, int aVertexStart, int aIndexcount )
{
	bool success = false;
	
	success = this->SetDiffuseTexture(aTextureFileName);
	assert( success == true && "Failed to load the texture.");
	this->SetEnviromentalCubeMapTexture(aEnviromentMapFileName);
	assert( success == true && "Failed to load the enviroment map.");
	this->SetPrimologyType(aPrimology);
	this->SetVertexStart(aVertexStart);
	this->SetIndexCount(myIndexCount);
}

Surface::~Surface(void)
{
}

void Surface::SetVertexStart( int aVertexStart )
{
	myVertexStart = aVertexStart;
}

void Surface::SetPrimologyType( D3D10_PRIMITIVE_TOPOLOGY aPrimology )
{
	myPrimologyType = aPrimology;
}

void Surface::SetIndexCount( int aIndexcount )
{
	myIndexCount = aIndexcount;
}

bool Surface::SetDiffuseTexture( std::string aFileName )
{
	myTexture = Engine::GetInstance()->GetTextureContainer().GetDiffuse(aFileName);
	if ( myTexture == NULL) 
	{
		assert( false );
		return false;
	}
	return true;
}

bool Surface::SetEnviromentalCubeMapTexture( std::string aFileName )
{
	myEnviromentMap = Engine::GetInstance()->GetTextureContainer().GetTexture(aFileName, true);
	if ( myEnviromentMap == NULL) 
	{
		assert( false );
		return false;
	}
	return true;
}

bool Surface::SetEnviromentalCubeMapTexture( ID3D10ShaderResourceView* aCubeMap )
{
	assert( aCubeMap != NULL );
	Engine::GetInstance()->GetTextureContainer().GiveTexture( aCubeMap );
	return SetEnviromentalCubeMapTexture( "Generated" );
}

bool Surface::SetNormalMapTexture( std::string aNormalMapFileName )
{
	myNormalMap = Engine::GetInstance()->GetTextureContainer().GetNormal(aNormalMapFileName);
	if ( myNormalMap == NULL) 
	{
		assert( false );
		return false;
	}
	return true;
}

bool Surface::SetSpecularMapTexture( std::string fileName )
{
	mySpecularMap = Engine::GetInstance()->GetTextureContainer().GetSpecular(fileName);
	if ( mySpecularMap == NULL) 
	{
		assert( false );
		return false;
	}
	return true;
}

bool Surface::SetAmbientOcclusionMapTexture( std::string fileName )
{
	myAmbientOcclusionMap = Engine::GetInstance()->GetTextureContainer().GetAmbientOcclusion(fileName);
	if ( myAmbientOcclusionMap == NULL) 
	{
		assert( false );
		return false;
	}
	return true;
}

int Surface::GetVertexStart()
{
	return myVertexStart;
}

int Surface::GetIndexCount()
{
	return myIndexCount;
}

Texture* Surface::GetDiffuseTexture()
{
	return myTexture;
}

Texture* Surface::GetEnviromentMap()
{
	return myEnviromentMap;
}

Texture* Surface::GetNormalMap()
{
	return myNormalMap;
}

void Surface::SetIndexStart( int aIndexStart )
{
	myIndexStart = aIndexStart;
}

void Surface::SetVertexCount( int aVertexCount )
{
	myVertexCount = aVertexCount;
}

int Surface::GetIndexStart()
{
	return myIndexStart;
}

void Surface::SetAdjacentPrimologyType( D3D10_PRIMITIVE_TOPOLOGY aPrimology )
{
	myAdjecentPrimologyType = aPrimology;
}

D3D10_PRIMITIVE_TOPOLOGY Surface::GetPrimologyType()
{
	return myPrimologyType;
}
