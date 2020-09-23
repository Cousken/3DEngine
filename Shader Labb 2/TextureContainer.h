#ifndef _TEXTURE_CONTAINER_H_
#define _TEXTURE_CONTAINER_H_

#include <map>
#include <string>
#include "Texture.h"
#include "CU_GrowingArray.h"
#include "debugHeader.h"

class TextureContainer
{
public:
	TextureContainer(void);
	~TextureContainer(void);

	Texture* GetDiffuse(std::string aFilename);
	Texture* GetNormal(std::string aFilename);
	Texture* GetSpecular(std::string aFilename);
	Texture* GetAmbientOcclusion(std::string aFilename);
	Texture* GetTexture(std::string aFilename, bool aUseSRGBFlag);

	void GiveTexture( ID3D10ShaderResourceView* aCubeMap );

private:
	Texture* LoadTexture(std::string aFilename, bool aUseSRGBFlag);	
	CommonUtilities::GrowingArray<Texture*> myTextures;
};

#endif