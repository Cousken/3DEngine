#ifndef _BUFFER_WRAPPER_H_
#define _BUFFER_WRAPPER_H_

#include <string>
#include <d3d10.h>
#include "Vector.h"
#include "debugHeader.h"

enum VertexType
{
	VertexTypePosNorm,
	VertexTypePosUV,
	VertexTypePosNormUV,
	VertexTypePosColUV,
	VertexTypePosNormUVTan,
	VertexTypePosNormUVTanBiNorm,
	VertexTypePosWeightBonesNorm,
	VertexTypePosWeightBonesNormUV,
	VertexTypePosWeightBonesNormUVTan,	
};

const std::string ShaderFiles[] =
{
	"LabbNormal.fx",
	"LabbNormal.fx",
	"LabbNormal.fx",
	"LabbNormal.fx",
	"LabbNormal.fx",
	"LabbNormal.fx",
	"PosWeightBonesNorm.fx",
	"PosWeightBonesNormUV.fx",
	"PosWeightBonesNormUVTan.fx"
};

struct IndexBufferWrapper
{
	int myByteOffset;
	ID3D10Buffer* myIndexBuffer;
	DXGI_FORMAT myIndexBufferFormat;
};

struct VertexBufferWrapper 
{
	UINT myByteOffset;
	int myNbrOfBuffers;
	int myStartSlot;
	unsigned myStride;
	ID3D10Buffer* myVertexBuffer;
};

struct VertexIndexWrapper
{
	DXGI_FORMAT myFormat;
	int myNrOfIndexes;
	int mySize;
	char* myIndexData;
};

struct VertexDataWrapper
{
	int myNrOfVertexes;
	int mySize;
	int myStride;
	char* myVertexData;
	VertexType myType;
};

struct VertexPosNormUV
{
	VertexPosNormUV()
	{
		myPos = Vector3f(0,0,0);
		myNormal = Vector3f(0,0,0);
		myUV = Vector2f(0,0);
	};

	VertexPosNormUV(const Vector3f aPosition, const Vector3f aNormal, const Vector2f aUV) 
	{
		myPos = aPosition;
		myNormal = aNormal;
		myUV = aUV;
	};

	Vector3f myPos;
	Vector3f myNormal;
	Vector2f myUV;
};

struct VertexPosUV
{
	VertexPosUV()
	{
		myPos = Vector3f(0,0,0);
		myUV = Vector2f(0,0);
	};

	VertexPosUV(const Vector3f aPosition, const Vector2f aUV) 
	{
		myPos = aPosition;
		myUV = aUV;
	};

	Vector3f myPos;
	Vector2f myUV;
};

struct VertexPosNorm
{
	VertexPosNorm()
	{
		myPos = Vector3f(0,0,0);
		myNorm = Vector2f(0,0);
	};

	VertexPosNorm(const Vector3f aPosition, const Vector2f aNormal) 
	{
		myPos = aPosition;
		myNorm = aNormal;
	};

	Vector3f myPos;
	Vector2f myNorm;
};

#endif