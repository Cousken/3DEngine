#ifndef NEMESIS3D_TEXTURE_HEADER
#define NEMESIS3D_TEXTURE_HEADER

#include <string>

#include <d3dx10.h>
#include <d3d10.h>


class Texture
{
public:
	Texture();
	Texture(ID3D10ShaderResourceView* aTexture);
	~Texture();

	std::string GetFileName();
	bool LoadTexture(std::string aTextureName,bool aUseSRGBFlag);
	ID3D10ShaderResourceView* GetTexture(); 
private:
	bool SetupLoadingData(D3DX10_IMAGE_LOAD_INFO& someInfo,D3DX10_IMAGE_INFO& sourceInfo,std::string aFileName);
	bool CompensateForSRGB(D3DX10_IMAGE_LOAD_INFO& someInfo,ID3D10Texture2D*& aTexture);
	bool BindTextureToView(D3DX10_IMAGE_LOAD_INFO& someInfo,ID3D10Texture2D*& aTexture,bool aUseSRGBFlag);
	bool LoadTextureAsShaderResourceView(std::string aTextureName,bool aUseSRGBFlag);

	std::string myFileName;
	ID3D10ShaderResourceView* myTexture;

};



#endif