#ifndef AMBIENT_PROBE_H
#define AMBIENT_PROBE_H

class RenderTarget;
class ID3D10Texture2D;
class ID3D10ShaderResourceView;

class AmbientProbe
{
public:
	enum CubeMapSideTypes
	{
		Right,
		Left,
		Up,
		Bottom,
		Front,
		Back,
	};

	AmbientProbe();
	~AmbientProbe();

	void Init();
	
//private:
	void CreateTexture2D();
	void CreateShaderResourceView();
	void CreateRenderTargetView();

	RenderTarget* myCubeMapSides[6];
	RenderTarget* myReflectionCubeMapSides[6];
	RenderTarget* myCubeMapSidesColor[6];
	RenderTarget* myCubeMapSidesDepth[6];
	RenderTarget* myCrossMapColor[6];
	RenderTarget* myCrossMapDepth[6];
	
	RenderTarget* myCrossMapCopy;
	RenderTarget* myCrossMapCopy2;
	RenderTarget* myTemporaryNormalMap;
	RenderTarget* myTemporaryColorMap;
	ID3D10Texture2D* myTexture;
	ID3D10Texture2D* myReflectionTexture;
	ID3D10ShaderResourceView* myShaderResourceView;
	ID3D10ShaderResourceView* myReflectionShaderResourceView;
};

#endif