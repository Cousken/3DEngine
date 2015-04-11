#include "TextureContainer.h"

TextureContainer::TextureContainer(void)
{
	myTextures.Init(10, 1);
}

TextureContainer::~TextureContainer(void)
{
}

Texture* TextureContainer::LoadTexture( std::string aFilename, bool aUseSRGBFlag )
{
	Texture* newTexture = new Texture();
	if (newTexture->LoadTexture(aFilename, aUseSRGBFlag) == false )
	{
		return 0;
	}

	myTextures.Add(newTexture);

	return newTexture;
}


Texture* TextureContainer::GetTexture( std::string aFilename, bool aUseSRGBFlag )
{
	Texture* result = 0;
	const int nbrOfTextures = myTextures.Count();
	for ( int textureChecked = 0; textureChecked < nbrOfTextures; textureChecked++)
	{
		if (myTextures[textureChecked]->GetFileName() == aFilename)
		{
			result = myTextures[textureChecked];
			break;
		}

	}
	if ( result == 0 )
	{
		result = LoadTexture(aFilename, aUseSRGBFlag);
	}
	return result;
}

Texture* TextureContainer::GetDiffuse( std::string aFilename )
{
	return GetTexture(aFilename, true);
}

Texture* TextureContainer::GetNormal( std::string aFilename )
{
	return GetTexture(aFilename, false);
}

Texture* TextureContainer::GetSpecular( std::string aFilename )
{
	return GetTexture(aFilename, true);
}

Texture* TextureContainer::GetAmbientOcclusion( std::string aFilename )
{
	return GetTexture(aFilename, true);
}

void TextureContainer::GiveTexture( ID3D10ShaderResourceView* aCubeMap )
{
	myTextures.Add( new Texture(aCubeMap) );
}
