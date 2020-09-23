#ifndef NEMESIS3D_DXSDKMESHFACTORY_HEADER
#define NEMESIS3D_DXSDKMESHFACTORY_HEADER

#include <vector>
#include <string>
#include <d3d10.h>
#include <d3d9.h>
#include <d3dx10.h>
#include "debugHeader.h"

const int MAX_NAME_LENGTH= 100;
const int MAX_VERTEX_STREAMS= 16;
const int MAX_VERTEX_ELEMENTS= 32;


class Model;
class Bone;
class TransformationNode_KeyFrames;
class DXSDKMeshFactory
{
protected:
	struct HierachyDataStruct
	{
		int myParent;
		std::vector<int> myChildren;
		std::vector<int> myMeshes;
		std::vector<int> myRenderMeshes;
		bool myIsNullFlag;
		bool myIsVoidFlag;
		bool myIsSharedFlag;
	};




	struct SDKMeshVertexHeader
	{
		unsigned __int64 NumVertices;
		unsigned __int64 SizeBytes;
		unsigned __int64 StrideBytes;
		D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
		union
		{
			unsigned __int64 myDataOffset;				//(This also forces the union to 64bits)
		};
	};

	struct SDKMeshFileHeader
	{
		//Basic Info and sizes
		unsigned int myVersion;
		unsigned char myIsBigEndian;
		unsigned __int64 myHeaderSize;
		unsigned __int64 myNonBufferDataSize;
		unsigned __int64 myBufferDataSize;

		//Stats
		unsigned int myNumVertexBuffers;
		unsigned int myNumIndexBuffers;
		unsigned int myNumMeshes;
		unsigned int myNumTotalSubsets;
		unsigned int myNumFrames;
		unsigned int myNumMaterials;

		//Offsets to Data
		unsigned __int64 myVertexStreamHeadersOffset;
		unsigned __int64 myIndexStreamHeadersOffset;
		unsigned __int64 myMeshDataOffset;
		unsigned __int64 mySubsetDataOffset;
		unsigned __int64 myFrameDataOffset;
		unsigned __int64 myMaterialDataOffset;
	};

	enum SDKMeshPrimitiveType
	{
		PT_TRIANGLE_LIST = 0,
		PT_TRIANGLE_STRIP,
		PT_LINE_LIST,
		PT_LINE_STRIP,
		PT_POINT_LIST,
		PT_TRIANGLE_LIST_ADJ,
		PT_TRIANGLE_STRIP_ADJ,
		PT_LINE_LIST_ADJ,
		PT_LINE_STRIP_ADJ,
	};

	struct SDKMeshMesh
	{
		char    Name[MAX_NAME_LENGTH];
		BYTE NumVertexBuffers;
		UINT    VertexBuffers[MAX_VERTEX_STREAMS];
		UINT IndexBuffer;
		UINT NumSubsets;
		UINT NumFrameInfluences; //aka bones

		D3DXVECTOR3 BoundingBoxCenter;
		D3DXVECTOR3 BoundingBoxExtents;

		union
		{
			UINT64 SubsetOffset;	//Offset to list of subsets (This also forces the union to 64bits)
			UINT* pSubsets;	    //Pointer to list of subsets
		};
		union
		{
			UINT64 FrameInfluenceOffset;  //Offset to list of frame influences (This also forces the union to 64bits)
			UINT* pFrameInfluences;      //Pointer to list of frame influences
		};
	};



public:
	DXSDKMeshFactory();
	~DXSDKMeshFactory();

	Model* CreateModelAndAnimation(std::string aFileName,std::string aAnimationFileName);
	Model* CreateModel(std::string aFileName);
	bool CreateAnimation(Model& aTargetModel,std::string aFileName);
protected:
	void CleanUp();

	bool DecodeBonesData(Model* aModel,SDKMeshMesh* aMesh,SDKMeshFileHeader& aFileHeader,int aMeshIndex);
	bool LoadModel(Model* aModel,std::string aFileName);
	bool LoadData(std::string aFileName);
	bool DecipherData(Model* aModel,std::string aFileName);
	bool DecodeGlobalBonesData(Model* aModel,SDKMeshFileHeader& aFileHeader);


	bool DecodeData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex,bool aIsShadowFlag);
	bool RecalculateNormals(Model* aModel);
	bool DecodeVertexData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex);
	bool DecodeIndexData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex);
	bool DecodeSubsetAndMaterialData(Model* aModel,SDKMeshMesh* aMesh,SDKMeshFileHeader& aFileHeader,int aMeshIndex);
	bool SelectEffect(Model* aModel);

	bool DecodeAdjacencyIndices( Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex);

	bool DetermineVertexLayout(Model* aModel,SDKMeshVertexHeader* aMeshVertexHeader);
	bool DetermineVertexType(Model* aModel);
	D3D10_PRIMITIVE_TOPOLOGY GetPrimitiveType( SDKMeshPrimitiveType PrimType );
	D3D10_PRIMITIVE_TOPOLOGY GetAdjacentPrimitiveType( SDKMeshPrimitiveType PrimType );

	bool LoadAnimationData(std::string aFileName);
	bool DecodeAnimationData(Model& aModel);
	Bone* FindBoneByName(Model& aModel,std::string aFileName);
	bool CreateShadowMesh(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex);
	bool CreateSkinnedShadowMesh(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex);

	int FindFrameID( char* pszName );


//***************************************************************
//							DATA 
//***************************************************************


	unsigned char* myRawFileData;
	unsigned char* myRawAnimationFileData;
	HierachyDataStruct* myAnimationHierachy;
	int* myFullToRealTranslationArray;
	int* myRealToFullTranslationArray;
	int myBonesCount;
	TransformationNode_KeyFrames* myBoneAnimations;

};

#endif