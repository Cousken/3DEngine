#ifndef _SURFACE_H_
#define _SURFACE_H_

#include <D3D10.h>
#include <string>

class Texture;

class Surface
{
public:
	Surface(void);
	Surface( std::string aTextureFileName, std::string aEnviromentMapFileName, D3D10_PRIMITIVE_TOPOLOGY aPrimology, int aVertexStart, int aIndexcount );
	~Surface(void);

	void SetPrimologyType(D3D10_PRIMITIVE_TOPOLOGY aPrimology);
	void SetAdjacentPrimologyType(D3D10_PRIMITIVE_TOPOLOGY aPrimology);
	void SetVertexStart(int aVertexStart);
	void SetVertexCount(int aVertexCount);
	void SetIndexCount(int aIndexcount);
	void SetIndexStart(int aIndexStart);
	bool SetDiffuseTexture(std::string aFileName);
	bool SetEnviromentalCubeMapTexture( std::string aEnviromentMapFileName );
	bool SetEnviromentalCubeMapTexture( ID3D10ShaderResourceView* aCubeMap );
	bool SetNormalMapTexture( std::string aNormalMapFileName );
	bool SetSpecularMapTexture( std::string fileName );
	bool SetAmbientOcclusionMapTexture( std::string fileName );

	D3D10_PRIMITIVE_TOPOLOGY GetPrimologyType();
	D3D10_PRIMITIVE_TOPOLOGY GetAdjecentPrimologyType();
	int GetVertexStart();
	int GetVertexCount();
	int GetIndexCount();
	int GetIndexStart();
	Texture* GetDiffuseTexture();
	Texture* GetEnviromentMap();
	Texture* GetNormalMap();
	
private:
	Texture* myTexture;
	Texture* myEnviromentMap;
	Texture* myNormalMap;
	Texture* mySpecularMap;
	Texture* myAmbientOcclusionMap;

	D3D10_PRIMITIVE_TOPOLOGY myPrimologyType;
	D3D10_PRIMITIVE_TOPOLOGY myAdjecentPrimologyType;
	int myVertexStart;
	int myVertexCount;
	int myIndexCount;
	int myIndexStart;
};

#endif