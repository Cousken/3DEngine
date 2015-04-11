#include "stdafx.h"
#include "DXSDKMeshFactory.h"
#include "model.h"
#include "surface.h"
#include "dl_Assert.h"
#include "FI_File.h"
#include <d3d9types.h>
#include <string>
#include "Engine.h"
#include "Bone.h"
#include "TransformationNode_KeyFrames.h"
#include "BoneHierarchy.h"
#include "BufferWrapper.h"

struct SDKMeshIndexHeader
{
	unsigned __int64 NumIndices;
	unsigned __int64 SizeBytes;
	unsigned int IndexType;
	union
	{
		unsigned __int64 myDataOffset;				//(This also forces the union to 64bits)
	};
};


struct SDKMeshMaterial
{
	char    Name[MAX_NAME_LENGTH];

	// Use MaterialInstancePath
	char    MaterialInstancePath[MAX_PATH];

	// Or fall back to d3d8-type materials
	char    DiffuseTexture[MAX_PATH];
	char    NormalTexture[MAX_PATH];
	char    SpecularTexture[MAX_PATH];

	D3DXVECTOR4 Diffuse;
	D3DXVECTOR4 Ambient;
	D3DXVECTOR4 Specular;
	D3DXVECTOR4 Emissive;
	float Power;

	union
	{
		unsigned __int64 Force64_1;			//Force the union to 64bits
	};
	union
	{
		unsigned __int64 Force64_2;			//Force the union to 64bits
	};
	union
	{
		unsigned __int64 Force64_3;			//Force the union to 64bits
	};

	union
	{
		unsigned __int64 Force64_4;			//Force the union to 64bits
	};
	union
	{
		unsigned __int64 Force64_5;		    //Force the union to 64bits
	};
	union
	{
		unsigned __int64 Force64_6;			//Force the union to 64bits
	};
};

struct SDKMeshSubset
{
	char Name[MAX_NAME_LENGTH];
	unsigned int MaterialID;
	unsigned int PrimitiveType;
	unsigned __int64 IndexStart;
	unsigned __int64 IndexCount;
	unsigned __int64 VertexStart;
	unsigned __int64 VertexCount;
};
struct SDKMeshFrame
{
	char Name[MAX_NAME_LENGTH];
	UINT Mesh;
	UINT ParentFrame;
	UINT ChildFrame;
	UINT SiblingFrame;
	D3DXMATRIX Matrix;
	UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame
};

D3D10_PRIMITIVE_TOPOLOGY DXSDKMeshFactory::GetPrimitiveType( SDKMeshPrimitiveType PrimType )
{
    D3D10_PRIMITIVE_TOPOLOGY retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch( PrimType )
    {
        case PT_TRIANGLE_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case PT_TRIANGLE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        case PT_LINE_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case PT_LINE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        case PT_POINT_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case PT_TRIANGLE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
			assert(0);
            break;
        case PT_TRIANGLE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
			assert(0);
            break;
        case PT_LINE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
			assert(0);
            break;
        case PT_LINE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
			assert(0);
            break;
    };

    return retType;
}



D3D10_PRIMITIVE_TOPOLOGY DXSDKMeshFactory::GetAdjacentPrimitiveType( SDKMeshPrimitiveType PrimType )
{
    D3D10_PRIMITIVE_TOPOLOGY retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch( PrimType )
    {
        case PT_TRIANGLE_LIST:
            retType = retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;;
            break;
        case PT_TRIANGLE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            break;
        case PT_LINE_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            break;
        case PT_LINE_STRIP:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            break;
        case PT_POINT_LIST:
            retType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
			assert(0);
            break;
        case PT_TRIANGLE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
			assert(0);
            break;
        case PT_TRIANGLE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
			assert(0);
            break;
        case PT_LINE_LIST_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
			assert(0);
            break;
        case PT_LINE_STRIP_ADJ:
            retType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
			assert(0);
            break;
    };

    return retType;
}


DXSDKMeshFactory::DXSDKMeshFactory()
{
	myRawFileData=NULL;
	myRawAnimationFileData=NULL;
	myAnimationHierachy=NULL;
	myFullToRealTranslationArray=NULL;
	myRealToFullTranslationArray=NULL;
	myBonesCount=0;

}


DXSDKMeshFactory::~DXSDKMeshFactory()
{
}


void DXSDKMeshFactory::CleanUp()
{
	delete [] (myRawFileData);
	myRawFileData=NULL;

	delete [] (myRawAnimationFileData);
	myRawAnimationFileData=NULL;

	delete [] (myAnimationHierachy);
	delete [] (myFullToRealTranslationArray);
	delete [] (myRealToFullTranslationArray);
		
	myAnimationHierachy=NULL;
	myFullToRealTranslationArray=NULL;
	myRealToFullTranslationArray=NULL;

	myBonesCount=0;
}

Model* DXSDKMeshFactory::CreateModelAndAnimation(std::string aFileName,std::string aAnimationFileName)
{
	Model* tempModel=NULL;

	tempModel=new Model();

	if((tempModel==NULL) || (LoadModel(tempModel,aFileName)==false) || (CreateAnimation(*tempModel,aAnimationFileName)==false))
	{
		CleanUp();
		delete tempModel;
		tempModel=NULL;
	};
	CleanUp();
	return(tempModel);

}

Model* DXSDKMeshFactory::CreateModel(std::string aFileName)
{
	Model* tempModel=NULL;

	tempModel=new Model();

	if((tempModel==NULL) || (LoadModel(tempModel,aFileName)==false))
	{
		CleanUp();
		delete tempModel;
		tempModel=NULL;
	};
	CleanUp();
	return(tempModel);
}

bool DXSDKMeshFactory::LoadModel(Model* aModel,std::string aFileName)
{
	if(LoadData(aFileName)==false)
	{
		return(false);
	}
	if(DecipherData(aModel,aFileName)==false)
	{
		return(false);
	}
	aModel->SetFileName(aFileName);
	return(true);
}

bool DXSDKMeshFactory::LoadData(std::string aFileName)
{
	FI::File tempFile;

	if(tempFile.Open(aFileName, ((1 << FI::READ) + (1 << FI::BINARY)))==false)
	{
		return(false);
	}
	int fileSize=tempFile.GetFileSize();

	myRawFileData=new unsigned char [fileSize] ();


	tempFile.ReadData(myRawFileData,fileSize);
	tempFile.Close();
	return(true);
}

bool DXSDKMeshFactory::DecipherData(Model* aModel,std::string aFileName)
{
	SDKMeshFileHeader fileHeader;
	memcpy(&fileHeader,myRawFileData,sizeof(SDKMeshFileHeader));
	

	if(DecodeGlobalBonesData(aModel,fileHeader)==false)
	{
		return(false);
	}


	if(fileHeader.myNumMeshes>1)
	{
		Model* childs;
		aModel->myIsNULLObject=true;
		childs= new Model [fileHeader.myNumMeshes] ();
		for(unsigned int i=0;i<fileHeader.myNumMeshes;i++)
		{
/*			childs[i].myShadowModelRef = new Model ();
			if(DecodeData(childs[i].myShadowModelRef,fileHeader,i,false)==false)
			{
				return(false);
			}*/
			aModel->AddChild(&childs[i],new TransformationNode(Matrix44f::Identity()));
			if(DecodeData(&childs[i],fileHeader,i,false)==false)
			{
				return(false);
			}
		}
		
	}
	else
	{
/*		aModel->myShadowModelRef = new Model ();
		if(DecodeData(aModel->myShadowModelRef,fileHeader,0,false)==false)
		{
			return(false);
		}*/
		if(DecodeData(aModel,fileHeader,0,false)==false)
		{
			return(false);
		}
		
	}

	return(true);
}


bool DXSDKMeshFactory::DecodeData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex,bool aIsShadowFlag)
{

	SDKMeshMesh* meshArray=reinterpret_cast<SDKMeshMesh*>(&myRawFileData[aFileHeader.myMeshDataOffset]);
	if(aIsShadowFlag==false)
	{
		meshArray[aMeshIndex].pSubsets=( UINT* )&myRawFileData[meshArray[aMeshIndex].SubsetOffset];
	}

	if(strcmp(meshArray[aMeshIndex].Name,"ground_planeShape")==0)
	{
		aModel->myCastsShadowFlag=false;
	}

	if(DecodeBonesData(aModel,&meshArray[aMeshIndex],aFileHeader,aMeshIndex)==false)
	{
		return(false);
	}


	if(DecodeSubsetAndMaterialData(aModel,&meshArray[aMeshIndex],aFileHeader,aMeshIndex)==false)
	{
		return(false);
	}

	if(DecodeIndexData(aModel,aFileHeader,aMeshIndex)==false)
	{
		return(false);
	}
    if(DecodeAdjacencyIndices(aModel,aFileHeader,aMeshIndex)==false)
	{
		return(false);
	}

	if(DecodeVertexData(aModel,aFileHeader,aMeshIndex)==false)
	{
		return(false);
	}

	if(SelectEffect(aModel)==false)
	{
		return(false);
	}
	if(aIsShadowFlag==false)
	{
		if(aModel->myVertexData.myType!=VertexTypePosWeightBonesNormUVTan)
		{
			if(CreateShadowMesh(aModel,aFileHeader,aMeshIndex)==false)
			{
				return(false);
			}
		}
		else
		{
			if(CreateSkinnedShadowMesh(aModel,aFileHeader,aMeshIndex)==false)
			{
				return(false);
			}
		}
	}
	return(true);
}





bool DXSDKMeshFactory::RecalculateNormals(Model* aModel)
{
/*	int i;
	VertexPosNormUV* data=reinterpret_cast<VertexPosNormUV*>(aModel->myVertexData.myVertexData);

	for(i=0;i<aModel->myVertexData.myNrOfVertexes;i++)
	{
		data[i].myNormal.Set(0,0,0);
	}
	for(i=0;i<aModel->mySurfaces.Count();i++)
	{
		if(aModel->mySurfaces[i].GetPrimologyType()==D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			int startIndex=aModel->mySurfaces[i].GetVertexStart();
			for(int j=0;j<aModel->mySurfaces[i].GetIndexCount();j+=3)
			{
				CU::Vector3f normal=(data[aModel->myVerticeIndexes[j+2]+startIndex].myPos-data[aModel->myVerticeIndexes[j+0]+startIndex].myPos).Cross(data[aModel->myVerticeIndexes[j+1]+startIndex].myPos-data[aModel->myVerticeIndexes[j]+startIndex].myPos);
				normal.Normalize();
				for(int k=0;k<3;k++)
				{
					data[aModel->myVerticeIndexes[j+k]].myNormal+=normal;
				}
			}
		}
	}
	for(i=0;i<aModel->myVertexData.myNrOfVertexes;i++)
	{
		data[i].myNormal.Normalize();
	}*/
	return(true);
}


bool DXSDKMeshFactory::DetermineVertexLayout(Model* aModel,SDKMeshVertexHeader* aMeshVertexHeader)
{
	int vertexIndex=0;
	int lastSize=0;
	int currentSize=0;
	int totalSize=static_cast<int>(aMeshVertexHeader->StrideBytes);
	while(aMeshVertexHeader->Decl[vertexIndex].Stream==0)
	{
		D3D10_INPUT_ELEMENT_DESC element= { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 };
		element.AlignedByteOffset=currentSize;
		if(aMeshVertexHeader->Decl[vertexIndex].Offset<lastSize)
		{
			break;
		}
		lastSize=aMeshVertexHeader->Decl[vertexIndex].Offset;
		if(lastSize>=totalSize)
		{
			break;
		}
		if(currentSize>=totalSize)
		{
			break;
		}

		if(aMeshVertexHeader->Decl[vertexIndex].Usage!=D3DDECLUSAGE_BINORMAL)
		{
			switch(aMeshVertexHeader->Decl[vertexIndex].Type)
			{
				case D3DDECLTYPE_FLOAT4	:
											element.Format=DXGI_FORMAT_R32G32B32A32_FLOAT;
											currentSize+=16;
										break;

				case D3DDECLTYPE_FLOAT3	:
											element.Format=DXGI_FORMAT_R32G32B32_FLOAT;
											currentSize+=12;

										break;

				case D3DDECLTYPE_FLOAT2	:
										element.Format=DXGI_FORMAT_R32G32_FLOAT;
										currentSize+=8;


										break;
				case D3DDECLTYPE_UBYTE4 :
										element.Format=DXGI_FORMAT_R8G8B8A8_UINT;
										currentSize+=4;
										break;
						
				case D3DDECLTYPE_UBYTE4N :
										element.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
										currentSize+=4;
										break;
				default:
						assert(0);
			};

			switch(aMeshVertexHeader->Decl[vertexIndex].Usage)
			{
				case D3DDECLUSAGE_POSITION	:
											element.SemanticName="POSITION";
										break;
				case D3DDECLUSAGE_BLENDWEIGHT :
											element.SemanticName="WEIGHTS";
										break;

				case D3DDECLUSAGE_BLENDINDICES :
											element.SemanticName="BONES";
										break;

				case D3DDECLUSAGE_NORMAL	:
											element.SemanticName="NORMAL";
										break;

				case D3DDECLUSAGE_TEXCOORD	:
											element.SemanticName="TEXCOORD";

										break;
				case D3DDECLUSAGE_TANGENT	:
											element.SemanticName="TANGENT";
											break;

				case D3DDECLUSAGE_BINORMAL	:
											element.SemanticName="BINORMAL";

										break;
				case  D3DDECLUSAGE_COLOR	:
											element.SemanticName="COLOR";

										break;
				default:
						assert(0);
			};


			aModel->myVertexFormat.Add(element);
		}
		vertexIndex++;

	}
	return(true);
}

bool DXSDKMeshFactory::DetermineVertexType(Model* aModel)
{
	if(strcmp(aModel->myVertexFormat[0].SemanticName,"POSITION")!=0)  return(false);
	if(aModel->myVertexFormat[0].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);
	if(aModel->myVertexFormat.Count()==2)
	{
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
		aModel->myVertexType=VertexTypePosUV;
		return(true);
	}
	if(aModel->myVertexFormat.Count()==3)
	{
		if(strcmp(aModel->myVertexFormat[2].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);


		if(strcmp(aModel->myVertexFormat[1].SemanticName,"NORMAL")==0)
		{
			if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);
			aModel->myVertexType=VertexTypePosNormUV;
			return(true);
		}
		else
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"COLOR")==0)
		{
			if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32A32_FLOAT) return(false);
			aModel->myVertexType=VertexTypePosColUV;
			return(true);
		}
	}
	if(aModel->myVertexFormat.Count()==4)
	{
		if(strcmp(aModel->myVertexFormat[1].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[2].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"TANGENT")!=0)  return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		aModel->myVertexType=VertexTypePosNormUVTan;
		return(true);

	}
	if(aModel->myVertexFormat.Count()==6)
	{

		if(strcmp(aModel->myVertexFormat[1].SemanticName,"WEIGHTS")!=0) return(false);
		if(aModel->myVertexFormat[1].Format!=DXGI_FORMAT_R8G8B8A8_UNORM) return(false);
		if(strcmp(aModel->myVertexFormat[2].SemanticName,"BONES")!=0) return(false);
		if(aModel->myVertexFormat[2].Format!=DXGI_FORMAT_R8G8B8A8_UINT) return(false);
		if(strcmp(aModel->myVertexFormat[3].SemanticName,"NORMAL")!=0) return(false);
		if(aModel->myVertexFormat[3].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		if(strcmp(aModel->myVertexFormat[4].SemanticName,"TEXCOORD")!=0)  return(false);
		if(aModel->myVertexFormat[4].Format!=DXGI_FORMAT_R32G32_FLOAT) return(false);
	
		if(strcmp(aModel->myVertexFormat[5].SemanticName,"TANGENT")!=0)  return(false);
		if(aModel->myVertexFormat[5].Format!=DXGI_FORMAT_R32G32B32_FLOAT) return(false);

		aModel->myVertexType=VertexTypePosWeightBonesNormUVTan;
		return(true);

	}

	return(false);
}

bool DXSDKMeshFactory::DecodeGlobalBonesData(Model* aModel,SDKMeshFileHeader& aFileHeader)
{

	SDKMeshMesh* meshArray=reinterpret_cast<SDKMeshMesh*>(&myRawFileData[aFileHeader.myMeshDataOffset]);
	SDKMeshFrame* frameArray = reinterpret_cast<SDKMeshFrame*>(myRawFileData + aFileHeader.myFrameDataOffset);


    for( UINT i = 0; i < aFileHeader.myNumMeshes; i++ )
    {
        meshArray[i].pFrameInfluences = reinterpret_cast<UINT*>(&myRawFileData[meshArray[i].FrameInfluenceOffset]);
    }



	myAnimationHierachy= new HierachyDataStruct [aFileHeader.myNumFrames] ();

// initing defaut values to the hierachy

	for(unsigned int i=0; i <aFileHeader.myNumFrames;i++)
	{
		myAnimationHierachy[i].myParent=frameArray[i].ParentFrame;
		myAnimationHierachy[i].myIsVoidFlag=false;
		myAnimationHierachy[i].myIsNullFlag=false;
		myAnimationHierachy[i].myIsSharedFlag=false;
		if(myAnimationHierachy[i].myParent!=-1)
		{
			myAnimationHierachy[myAnimationHierachy[i].myParent].myChildren.push_back(i);
		}
	}
// init bones in use.

	for(unsigned int i=0;i <aFileHeader.myNumMeshes;i++)
	{
		for(unsigned int j =0;j<meshArray[i].NumFrameInfluences;j++)
		{
			myAnimationHierachy[meshArray[i].pFrameInfluences[j]].myMeshes.push_back(i);
			myAnimationHierachy[meshArray[i].pFrameInfluences[j]].myRenderMeshes.push_back(i);
		}

	}

	// recursively detect all bones in use
	bool meshAdded=false;
	do
	{
		meshAdded=false;
		for(unsigned int frameIndex=0;frameIndex<aFileHeader.myNumFrames;frameIndex++)
		{
			for(unsigned int childIndex=0;childIndex<myAnimationHierachy[frameIndex].myChildren.size();childIndex++)
			{
				for(unsigned int meshIndex=0;meshIndex<myAnimationHierachy[myAnimationHierachy[frameIndex].myChildren[childIndex]].myMeshes.size();meshIndex++)
				{
					bool toAdd=true;
					for(unsigned int parentMeshIndex=0;parentMeshIndex<myAnimationHierachy[frameIndex].myMeshes.size();parentMeshIndex++)
					{
						if(myAnimationHierachy[frameIndex].myMeshes[parentMeshIndex]==myAnimationHierachy[myAnimationHierachy[frameIndex].myChildren[childIndex]].myMeshes[meshIndex])
						{
							toAdd=false;
						}
					}
					if(toAdd==true)
					{
						meshAdded=true;
						myAnimationHierachy[frameIndex].myMeshes.push_back(myAnimationHierachy[myAnimationHierachy[frameIndex].myChildren[childIndex]].myMeshes[meshIndex]);
					}
				}
			}
		}
	} while(meshAdded==true);

	// flag all bones appropriately


	for(unsigned int i=0; i <aFileHeader.myNumFrames;i++)
	{
		if((myAnimationHierachy[i].myMeshes.size()==0) && (myAnimationHierachy[i].myRenderMeshes.size()==0))
		{
			myAnimationHierachy[i].myIsVoidFlag=true;
		}
		if((myAnimationHierachy[i].myMeshes.size()>0) && (myAnimationHierachy[i].myRenderMeshes.size()==0))
		{
			myAnimationHierachy[i].myIsNullFlag=true;
		}
		if(myAnimationHierachy[i].myRenderMeshes.size()>1)
		{
			myAnimationHierachy[i].myIsSharedFlag=true;
		}

	}

	//calculate the number of real bones and build a translation table

	myBonesCount=0;
	int meshes=0;
	myFullToRealTranslationArray = new int [aFileHeader.myNumFrames] ();

	for(unsigned int i=0; i <aFileHeader.myNumFrames;i++)
	{
		if(myAnimationHierachy[i].myMeshes.size()>0)
		{
			myFullToRealTranslationArray[i]=myBonesCount;
			myBonesCount++;
		}
		else
		{
			myFullToRealTranslationArray[i]=-1;
		}

	}

// build translation array for going the other way
	myRealToFullTranslationArray = new int [myBonesCount] ();

	for(unsigned int i=0; i <aFileHeader.myNumFrames;i++)
	{
		if(myFullToRealTranslationArray[i]!=-1)
		{
			myRealToFullTranslationArray[myFullToRealTranslationArray[i]]=i;
		}
	}


//

	aModel->myBones.ReInit(myBonesCount,10);
	
	for(int i=0;i<myBonesCount;i++)
	{
		Bone tempBone;
		aModel->myBones.Add(tempBone);
		for(unsigned int j=0;j<myAnimationHierachy[myRealToFullTranslationArray[i]].myChildren.size();j++)
		{
			int id=myFullToRealTranslationArray[myAnimationHierachy[myRealToFullTranslationArray[i]].myChildren[j]];
			if(id!=-1)
			{
				aModel->myBones[i].AddChild(id);
			}
		}
		
		assert(myFullToRealTranslationArray[myRealToFullTranslationArray[i]]!=-1);

		Matrix44f tempMatrix;
		tempMatrix.ConvertD3D(reinterpret_cast<float*>(&frameArray[myRealToFullTranslationArray[i]].Matrix));
		aModel->myBones[i].SetBindPose(tempMatrix);
//		myBones[i].m_OriginalFrameMatrix=m_pFrameArray[myRealToFullTranslationArray[i]].Matrix;
	}

	return(true);
}






bool DXSDKMeshFactory::DecodeBonesData(Model* aModel,SDKMeshMesh* aMesh,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	aModel->myBoneList.ReInit(aMesh->NumFrameInfluences,20);
	for(unsigned int j =0;j<aMesh->NumFrameInfluences;j++)
	{
		aModel->myBoneList.Add((myFullToRealTranslationArray[aMesh->pFrameInfluences[j]]));
	}
	return(true);
}




bool DXSDKMeshFactory::DecodeVertexData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	SDKMeshVertexHeader* meshVertexHeader;
	bool myRecalculateNormalsFlag=false;

	meshVertexHeader=reinterpret_cast<SDKMeshVertexHeader*>(&myRawFileData[aFileHeader.myVertexStreamHeadersOffset]);

	unsigned char* bufferData=myRawFileData+aFileHeader.myHeaderSize+aFileHeader.myNonBufferDataSize;
	unsigned int bufferStartPosition=static_cast<int>(aFileHeader.myHeaderSize+aFileHeader.myNonBufferDataSize);

	if(DetermineVertexLayout(aModel,&meshVertexHeader[aMeshIndex])==false)
	{
		return(false);
	}

	if(DetermineVertexType(aModel)==false)
	{
		return(false);
	}




	unsigned char* vertexData=reinterpret_cast<unsigned char*>(bufferData+meshVertexHeader[aMeshIndex].myDataOffset -bufferStartPosition);

	aModel->myVertexData.myNrOfVertexes=static_cast<int>(meshVertexHeader[aMeshIndex].NumVertices);
	aModel->myVertexData.mySize=static_cast<int>(meshVertexHeader[aMeshIndex].SizeBytes);
	aModel->myVertexData.myStride=static_cast<int>(meshVertexHeader[aMeshIndex].StrideBytes);
	aModel->myVertexData.myVertexData=new char[aModel->myVertexData.mySize] ();
	memcpy(aModel->myVertexData.myVertexData,vertexData,aModel->myVertexData.mySize);
	aModel->myVertexData.myType=aModel->myVertexType;
	
	if(aModel->myVertexData.myType==VertexTypePosNormUV)
	{
		VertexPosNormUV* data=reinterpret_cast<VertexPosNormUV*>(aModel->myVertexData.myVertexData);
		for(int i=0;i<meshVertexHeader[aMeshIndex].NumVertices;i++)
		{
			if(data[i].myNormal.Length2()==0) myRecalculateNormalsFlag=true;
		}

		if(myRecalculateNormalsFlag==true)
		{
			RecalculateNormals(aModel);
		}

	}
	


	return(true);
}


bool DXSDKMeshFactory::DecodeIndexData(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	SDKMeshIndexHeader* meshIndexHeader;

	meshIndexHeader=reinterpret_cast<SDKMeshIndexHeader*>(&myRawFileData[aFileHeader.myIndexStreamHeadersOffset]);

	unsigned char* bufferData=myRawFileData+aFileHeader.myHeaderSize+aFileHeader.myNonBufferDataSize;
	int bufferStartPosition=static_cast<int>(aFileHeader.myHeaderSize+aFileHeader.myNonBufferDataSize);
	unsigned char* indexData=bufferData+meshIndexHeader[aMeshIndex].myDataOffset -bufferStartPosition;
//	aModel->myVerticeIndexes.ReInit(meshIndexHeader[aMeshIndex].NumIndices,100);
	


	aModel->myVertexIndexData.myNrOfIndexes=static_cast<int>(meshIndexHeader[aMeshIndex].NumIndices);
	aModel->myVertexIndexData.mySize=static_cast<int>(meshIndexHeader[aMeshIndex].SizeBytes);
	aModel->myVertexIndexData.myIndexData= new char [aModel->myVertexIndexData.mySize] ();
	memcpy(aModel->myVertexIndexData.myIndexData,indexData,aModel->myVertexIndexData.mySize);
	

	if(meshIndexHeader[aMeshIndex].IndexType==0)
	{
		aModel->myVertexIndexData.myFormat=DXGI_FORMAT_R16_UINT;

	}
	else
	{
		aModel->myVertexIndexData.myFormat=DXGI_FORMAT_R32_UINT;
	}
	return(true);
}


bool DXSDKMeshFactory::SelectEffect(Model* aModel)
{
	if(aModel->myIsNULLObject==false)
	{
		Effect* effect=Engine::GetInstance()->GetEffectContainer().GetEffect(ShaderFiles[aModel->myVertexType]);
		if(effect==NULL)
		{
			return(false);
		}
		aModel->SetEffect(effect);
	};
	return(true);
}


bool DXSDKMeshFactory::DecodeSubsetAndMaterialData(Model* aModel,SDKMeshMesh* aMesh,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	SDKMeshSubset*  subsetArray= reinterpret_cast<SDKMeshSubset*>(&myRawFileData[aFileHeader.mySubsetDataOffset]);
	SDKMeshMaterial* materialArray=reinterpret_cast<SDKMeshMaterial*>(&myRawFileData[aFileHeader.myMaterialDataOffset]);


	for(unsigned int i=0;i<aMesh->NumSubsets;i++)
	{
		Surface surf;
		int index=aMesh->pSubsets[i];
		surf.SetIndexCount(subsetArray[index].IndexCount);
		surf.SetIndexStart(subsetArray[index].IndexStart);
		surf.SetVertexStart(subsetArray[index].VertexStart);
		surf.SetPrimologyType(GetPrimitiveType(static_cast<SDKMeshPrimitiveType>(subsetArray[index].PrimitiveType)));
		surf.SetAdjacentPrimologyType(GetAdjacentPrimitiveType(static_cast<SDKMeshPrimitiveType>(subsetArray[index].PrimitiveType)));
		surf.SetDiffuseTexture(materialArray[subsetArray[index].MaterialID].DiffuseTexture);
		if(materialArray[subsetArray[index].MaterialID].NormalTexture!=0)
		{
			surf.SetNormalMapTexture(materialArray[subsetArray[index].MaterialID].NormalTexture);
		}
		surf.SetDiffuseTexture(materialArray[subsetArray[index].MaterialID].DiffuseTexture);
// TEMP HACK FOR ENVMAP		
		surf.SetEnviromentalCubeMapTexture("Lobby\\lobbycube.dds");
// TEMP HACK FOR ENVMAP		

		aModel->mySurfaces.Add(surf);
	}

	return(true);
}



struct SDKAnimationFileHeader
{
    UINT Version;
    BYTE IsBigEndian;
    UINT FrameTransformType;
    UINT NumFrames;
    UINT NumAnimationKeys;
    UINT AnimationFPS;
    UINT64 AnimationDataSize;
    UINT64 AnimationDataOffset;
};

struct SDKAnimationKeyFrame
{
    D3DXVECTOR3 Translation;
    D3DXVECTOR4 Orientation;
    D3DXVECTOR3 Scaling;
};

struct SDKAnimationKeyFrameSet
{
    char FrameName[MAX_NAME_LENGTH];
    union
    {
        UINT64 DataOffset;
        SDKAnimationKeyFrame* pAnimationData;
    };
};


bool DXSDKMeshFactory::CreateAnimation(Model& aTargetModel,std::string aFileName)
{
	if(LoadAnimationData(aFileName)==false)
	{
		return(false);
	}
	if(DecodeAnimationData(aTargetModel)==false)
	{
		return(false);
	}
	CleanUp();
	aTargetModel.TransformBindPose(Matrix44f::Identity());
	return(true);
}


bool DXSDKMeshFactory::LoadAnimationData(std::string aFileName)
{
	FI::File tempFile;

	if(tempFile.Open(aFileName, ((1 << FI::READ) + (1 << FI::BINARY)))==false)
	{
		return(false);
	}
	int fileSize=tempFile.GetFileSize();


	myRawAnimationFileData=new unsigned char [fileSize] ();

	if(tempFile.ReadData(myRawAnimationFileData,fileSize)==false)
	{
		return(false);
	}
	tempFile.Close();
	return(true);

}

bool DXSDKMeshFactory::DecodeAnimationData(Model& aTargetModel)
{
	SDKAnimationFileHeader* fileHeader=reinterpret_cast<SDKAnimationFileHeader*>(myRawAnimationFileData);
	SDKAnimationKeyFrameSet* animationKeyFrameSet=reinterpret_cast<SDKAnimationKeyFrameSet*>(myRawAnimationFileData+fileHeader->AnimationDataOffset);
	int BaseOffset = sizeof( SDKAnimationFileHeader );

    for( UINT i = 0; i < fileHeader->NumFrames; i++ )
    {
        animationKeyFrameSet[i].pAnimationData = ( SDKAnimationKeyFrame* )( myRawAnimationFileData +
                                                                          animationKeyFrameSet[i].DataOffset +
                                                                          BaseOffset );
    }



	aTargetModel.myAnimation=new BoneHierarchy();
	
	aTargetModel.myAnimation->Init(fileHeader->NumFrames);


	myBoneAnimations = new TransformationNode_KeyFrames [fileHeader->NumFrames] ();

    for( UINT i = 0; i < fileHeader->NumFrames; i++ )
    {
		if(fileHeader->FrameTransformType==0)
		{
			myBoneAnimations[i].Init(RELATIVE_TRANSFORM,fileHeader->NumAnimationKeys,fileHeader->AnimationFPS);
		}
		else
		{
			myBoneAnimations[i].Init(ABSOLUTE_TRANSFORM,fileHeader->NumAnimationKeys,fileHeader->AnimationFPS);
		}
		for(unsigned int j=0;j<fileHeader->NumAnimationKeys;j++)
		{
			AnimationFrame aFrame;

			aFrame.myOrientation.x=animationKeyFrameSet[i].pAnimationData[j].Orientation.x;
			aFrame.myOrientation.y=animationKeyFrameSet[i].pAnimationData[j].Orientation.y;
			aFrame.myOrientation.z=animationKeyFrameSet[i].pAnimationData[j].Orientation.z;
			aFrame.myOrientation.W=animationKeyFrameSet[i].pAnimationData[j].Orientation.w;

			aFrame.myTranslation.x=animationKeyFrameSet[i].pAnimationData[j].Translation.x;
			aFrame.myTranslation.y=animationKeyFrameSet[i].pAnimationData[j].Translation.y;
			aFrame.myTranslation.z=animationKeyFrameSet[i].pAnimationData[j].Translation.z;

			aFrame.myScaling.x=animationKeyFrameSet[i].pAnimationData[j].Scaling.x;
			aFrame.myScaling.y=animationKeyFrameSet[i].pAnimationData[j].Scaling.y;
			aFrame.myScaling.z=animationKeyFrameSet[i].pAnimationData[j].Scaling.z;

			aFrame.myQuaternion.w = aFrame.myOrientation.W;
			aFrame.myQuaternion.x = aFrame.myOrientation.x;
			aFrame.myQuaternion.y = aFrame.myOrientation.y;
			aFrame.myQuaternion.z = aFrame.myOrientation.z;
			if( aFrame.myQuaternion.w == 0 && aFrame.myQuaternion.x == 0 && aFrame.myQuaternion.y == 0 && aFrame.myQuaternion.z == 0 )
				D3DXQuaternionIdentity( &aFrame.myQuaternion );
			D3DXQuaternionNormalize( &aFrame.myQuaternion, &aFrame.myQuaternion );

			
			myBoneAnimations[i].AddAnimationFrame(&aFrame);
		}
		
	  }


	for(int i=0;i<myBonesCount;i++)
	{
		aTargetModel.myAnimation->AddTransformationNode(NULL);
	}

	for(int i=0;i<myBonesCount;i++)
	{

		int	boneID=FindFrameID(animationKeyFrameSet[i].FrameName);
  	    boneID=myFullToRealTranslationArray[boneID];

		if ( boneID > 1000 )
		{
			const int i = 0;
		}

		aTargetModel.myAnimation->GetTransformationNodes()[boneID]=&myBoneAnimations[i];

	}
	


	myBoneAnimations=NULL;  // memory owned elsewhere
	return(true);
}


int DXSDKMeshFactory::FindFrameID( char* pszName )
{
	SDKMeshFileHeader fileHeader;
	memcpy(&fileHeader,myRawFileData,sizeof(SDKMeshFileHeader));

	SDKMeshFrame* frameArray = reinterpret_cast<SDKMeshFrame*>(myRawFileData + fileHeader.myFrameDataOffset);

    
	for( UINT i = 0; i < fileHeader.myNumFrames; i++ )
    {
        if( _stricmp( frameArray[i].Name, pszName ) == 0 )
        {
            return i;
        }
    }
    return -1;
}


Bone* DXSDKMeshFactory::FindBoneByName(Model& aModel,std::string aFileName)
{
/*
for(int i=0;i<aModel.myBones.Count();i++)
	{
		if(aModel.myBones[i].GetName()==aFileName)
		{
			return(&aModel.myBones[i]);
		}
	}
	for(int i=0;i<aModel.myChilds.Count();i++)
	{
		Bone* bone=NULL;
		bone=FindBoneByName(*aModel.myChilds[i],aFileName);
		if(bone!=NULL)
		{
			return(bone);
		}
	}
*/
	return(NULL);
}


bool DXSDKMeshFactory::DecodeAdjacencyIndices( Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	return(true);
	SDKMeshMesh* meshArray=reinterpret_cast<SDKMeshMesh*>(&myRawFileData[aFileHeader.myMeshDataOffset]);
	SDKMeshIndexHeader* meshIndexHeader=reinterpret_cast<SDKMeshIndexHeader*>(&myRawFileData[aFileHeader.myIndexStreamHeadersOffset]);
	SDKMeshVertexHeader* meshVertexHeader=reinterpret_cast<SDKMeshVertexHeader*>(&myRawFileData[aFileHeader.myVertexStreamHeadersOffset]);

	
    UINT IBIndex = 0;
    UINT VBIndex = 0;
	VBIndex = meshArray[aMeshIndex].VertexBuffers[0];
	IBIndex = meshArray[aMeshIndex].IndexBuffer;

    ID3DX10Mesh* tempMesh = NULL;


    D3D10_INPUT_ELEMENT_DESC layout[2] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
            { "END", 0, DXGI_FORMAT_R8_UINT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        };
	   layout[1].AlignedByteOffset = static_cast<int>(meshVertexHeader[VBIndex].StrideBytes - 1);

    // create the mesh 
	
    

	unsigned int options=0;
    if( 1==  meshIndexHeader[IBIndex].IndexType )
        options |= D3DX10_MESH_32_BIT;
	D3DX10CreateMesh( Engine::GetInstance()->GetDevice(),
                                layout,
                                2,
                                layout[0].SemanticName,
								meshVertexHeader[VBIndex].NumVertices,
                                meshIndexHeader[IBIndex].NumIndices / 3,
                                options,
                                &tempMesh );

    //set the VB
	tempMesh->SetVertexData( 0, ( void* )( myRawFileData + meshVertexHeader[VBIndex].myDataOffset ) );
	 //set the IB
    tempMesh->SetIndexData( ( void* ) (myRawFileData + meshIndexHeader[IBIndex].myDataOffset ), meshIndexHeader[IBIndex].NumIndices );
    //generate adjacency
    tempMesh->GenerateAdjacencyAndPointReps( 0.001f );
    //generate adjacency indices
    tempMesh->GenerateGSAdjacency();


    //get the adjacency data out of the mesh
    ID3DX10MeshBuffer* adjacentIndexBuffer = NULL;
    BYTE* adjacentIndexData = NULL;
    SIZE_T bufferSize = 0;
    tempMesh->GetIndexBuffer( &adjacentIndexBuffer) ;
    adjacentIndexBuffer->Map( ( void** )&adjacentIndexData, &bufferSize);


    //Copy info about the original IB with a few modifications
	
	
	
    aModel->myAdjacencyIndexBuffer.myByteOffset=0;
	aModel->myAdjacencyIndexBuffer.myIndexBuffer=0;
	aModel->myAdjacencyIndexBuffer.myIndexBufferFormat=aModel->myIndexBuffer.myIndexBufferFormat;
	aModel->myAdjacencyVertexIndexData.mySize=static_cast<int>(meshIndexHeader[IBIndex].SizeBytes*2);
	aModel->myAdjacencyVertexIndexData.myNrOfIndexes=static_cast<int>(meshIndexHeader[IBIndex].NumIndices);
	aModel->myAdjacencyVertexIndexData.myIndexData=new char [aModel->myAdjacencyVertexIndexData.mySize] ();
	memcpy(aModel->myAdjacencyVertexIndexData.myIndexData,adjacentIndexData,aModel->myAdjacencyVertexIndexData.mySize);

    adjacentIndexBuffer->Unmap();
    adjacentIndexBuffer->Release();

    //release the ID3DX10Mesh
    tempMesh->Release();

	return(true);
}


struct EdgeMapping
{
    int m_anOldEdge[2];  // vertex index of the original edge
    int m_aanNewEdge[2][2]; // vertex indexes of the new edge
    // First subscript = index of the new edge
    // Second subscript = index of the vertex for the edge

public:
        EdgeMapping()
        {
            FillMemory( m_anOldEdge, sizeof( m_anOldEdge ), -1 );
            FillMemory( m_aanNewEdge, sizeof( m_aanNewEdge ), -1 );
        }
};


struct SHADOWVERT
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
};


int FindEdgeInMappingTable( int nV1, int nV2, EdgeMapping* pMapping, int nCount )
{
    for( int i = 0; i < nCount; ++i )
    {
        // If both vertex indexes of the old edge in mapping entry are -1, then
        // we have searched every valid entry without finding a match.  Return
        // this index as a newly created entry.
        if( ( pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1 ) ||

            // Or if we find a match, return the index.
            ( pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2 ) )
        {
            return i;
        }
    }

    return -1;  // We should never reach this line
}




bool DXSDKMeshFactory::CreateShadowMesh(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	int i;
    ID3DX10Mesh* tempMesh = NULL;

	const D3D10_INPUT_ELEMENT_DESC shadowVertLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

   	SHADOWVERT* baseVertexBuffer=new SHADOWVERT[aModel->myVertexData.myNrOfVertexes] ();
	unsigned int* baseIndexBuffer=  new unsigned int [aModel->myVertexIndexData.myNrOfIndexes] ();

	unsigned char* copybaseVertexBuffer=(unsigned char*)baseVertexBuffer;
	unsigned short* copybaseInitBuffer=(unsigned short*)aModel->myVertexIndexData.myIndexData;
	if(aModel->myVertexIndexData.myFormat!=DXGI_FORMAT_R32_UINT )
	{
		for(i=0;i<aModel->myVertexIndexData.myNrOfIndexes;i++)
		{
				baseIndexBuffer[i]=copybaseInitBuffer[i];
		}
	}
	else
	{
		memcpy(baseIndexBuffer,copybaseInitBuffer,4*aModel->myVertexIndexData.myNrOfIndexes);
	}

	for(i=0;i<aModel->myVertexData.myNrOfVertexes;i++)
	{
		memcpy((void*)&baseVertexBuffer[i],(void*)&aModel->myVertexData.myVertexData[i*aModel->myVertexData.myStride],sizeof(SHADOWVERT));
	}


	


	
	

	unsigned int options=0;
//    if( 1==  meshIndexHeader[IBIndex].IndexType )
        options |= D3DX10_MESH_32_BIT;
	D3DX10CreateMesh( Engine::GetInstance()->GetDevice(),
								shadowVertLayout,
                                2,
                                shadowVertLayout[0].SemanticName,
                                aModel->myVertexData.myNrOfVertexes,
								aModel->myVertexIndexData.myNrOfIndexes / 3,
                                options,
                                &tempMesh );

    //set the VB // got to convert for real :/
	tempMesh->SetVertexData( 0, ( void* )(baseVertexBuffer) );
	 //set the IB
    tempMesh->SetIndexData( ( void* ) (baseIndexBuffer ),aModel->myVertexIndexData.myNrOfIndexes);
    //generate adjacency
	HRESULT hr = S_OK;
    hr =tempMesh->GenerateAdjacencyAndPointReps( 0.0001f );
    //generate adjacency indices
    tempMesh->GenerateGSAdjacency();


  
	
	//get the adjacency data out of the mesh

    ID3DX10MeshBuffer* adjacentBuffer = NULL;
    unsigned int* adjacentData = NULL;
    SIZE_T bufferSize = 0;
    hr =tempMesh->GetAdjacencyBuffer(&adjacentBuffer) ;
    hr =adjacentBuffer->Map( ( void** )&adjacentData, &bufferSize);


    ID3DX10MeshBuffer* adjacentPointRepBuffer = NULL;
    unsigned int* adjacentPointRepData = NULL;
    SIZE_T bufferSize2 = 0;
    hr =tempMesh->GetPointRepBuffer(&adjacentPointRepBuffer) ;
    hr =adjacentPointRepBuffer->Map( ( void** )&adjacentPointRepData, &bufferSize2);


	


	
	int numEdges=tempMesh->GetFaceCount()*3;
	int numMaps=0;
	int nextIndex=0;

    EdgeMapping* edgeMapping = new EdgeMapping[numEdges];


	int faceCount=tempMesh->GetFaceCount();
	SHADOWVERT* newVertBuffer=new SHADOWVERT [(faceCount * 3)] ();
	unsigned int* newIndexBuffer= new unsigned int[((faceCount + numEdges * 2)*3)] ();

    ZeroMemory( newVertBuffer,sizeof(SHADOWVERT)*(faceCount * 3) );
    ZeroMemory( newIndexBuffer,4*((faceCount + numEdges * 2)*3));

	int newVert=faceCount * 3;
	int newIndex=(faceCount + numEdges * 2)*3;


     SHADOWVERT* nextOutVertex = (SHADOWVERT*)newVertBuffer;



    // Iterate through the faces.  For each face, output new
    // vertices and face in the new mesh, and write its edges
    // to the mapping table.

    for( int f = 0; f < faceCount; ++f )
    {
        // Copy the vertex data for all 3 vertices
        CopyMemory( nextOutVertex, baseVertexBuffer + baseIndexBuffer[f * 3], sizeof( SHADOWVERT ) );
        CopyMemory( nextOutVertex + 1, baseVertexBuffer + baseIndexBuffer[f * 3 + 1], sizeof( SHADOWVERT ) );
        CopyMemory( nextOutVertex + 2, baseVertexBuffer + baseIndexBuffer[f * 3 + 2], sizeof( SHADOWVERT ) );

        // Write out the face
        newIndexBuffer[nextIndex++] = f * 3;
        newIndexBuffer[nextIndex++] = f * 3 + 1;
        newIndexBuffer[nextIndex++] = f * 3 + 2;

        // Compute the face normal and assign it to
        // the normals of the vertices.
        D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
        D3DXVECTOR3 vNormal;
        v1 = *( D3DXVECTOR3* )( nextOutVertex + 1 ) - *( D3DXVECTOR3* )nextOutVertex;
        v2 = *( D3DXVECTOR3* )( nextOutVertex + 2 ) - *( D3DXVECTOR3* )( nextOutVertex + 1 );
        D3DXVec3Cross( &vNormal, &v1, &v2 );
        D3DXVec3Normalize( &vNormal, &vNormal );

        nextOutVertex->Normal = -vNormal;
        ( nextOutVertex + 1 )->Normal = -vNormal;
        ( nextOutVertex + 2 )->Normal = -vNormal;

        nextOutVertex += 3;

        // Add the face's edges to the edge mapping table

        // Edge 1
        int nIndex;
        int nVertIndex[3] =
        {
            adjacentPointRepData[baseIndexBuffer[f * 3]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 1]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 2]]
        };
        nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            // No entry for this edge yet.  Initialize one.
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 2
        nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
            assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 3
        nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }
    }

    // Now the entries in the edge mapping table represent
    // non-shared edges.  What they mean is that the original
    // mesh has openings (holes), so we attempt to patch them.
    // First we need to recreate our mesh with a larger vertex
    // and index buffers so the patching geometry could fit.


	SHADOWVERT* patchVertBuffer=new SHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* patchIndexBuffer= new unsigned int[( nextIndex + 3 * numMaps * 7 )] ();

    ZeroMemory( patchVertBuffer,sizeof(SHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( patchIndexBuffer, 4 * ( nextIndex + 3 * numMaps * 7 ) );

    CopyMemory( patchVertBuffer, newVertBuffer, sizeof( SHADOWVERT ) * faceCount * 3 );
    CopyMemory( patchIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

	int patchVert=( faceCount + numMaps ) *3;
	int patchIndex=( nextIndex + 3 * numMaps * 7 );

	delete(newIndexBuffer);
	delete(newVertBuffer);
    newVertBuffer = patchVertBuffer;
    newIndexBuffer = patchIndexBuffer;
	patchVertBuffer=NULL;
	patchIndexBuffer=NULL;
	



	int nextVertex = faceCount * 3;

	for( int i = 0; i < numMaps; ++i )
	{
		if( edgeMapping[i].m_anOldEdge[0] != -1 &&
			edgeMapping[i].m_anOldEdge[1] != -1 )
		{
			// If the 2nd new edge indexes is -1,
			// this edge is a non-shared one.
			// We patch the opening by creating new
			// faces.
			if( edgeMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
				edgeMapping[i].m_aanNewEdge[1][1] == -1 )
			{
				// Find another non-shared edge that
				// shares a vertex with the current edge.
				for( int i2 = i + 1; i2 < numMaps; ++i2 )
				{
					if( edgeMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
						edgeMapping[i2].m_anOldEdge[1] != -1 &&
						( edgeMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
						  edgeMapping[i2].m_aanNewEdge[1][1] == -1 ) )
					{
						int nVertShared = 0;
						if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							++nVertShared;
						if( edgeMapping[i2].m_anOldEdge[1] == edgeMapping[i].m_anOldEdge[0] )
							++nVertShared;

						if( 2 == nVertShared )
						{
							// These are the last two edges of this particular
							// opening. Mark this edge as shared so that a degenerate
							// quad can be created for it.

							edgeMapping[i2].m_aanNewEdge[1][0] = edgeMapping[i].m_aanNewEdge[0][0];
							edgeMapping[i2].m_aanNewEdge[1][1] = edgeMapping[i].m_aanNewEdge[0][1];
							break;
						}
						else if( 1 == nVertShared )
						{
							// nBefore and nAfter tell us which edge comes before the other.
							int nBefore, nAfter;
							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								nBefore = i;
								nAfter = i2;
							}
							else
							{
								nBefore = i2;
								nAfter = i;
							}

							// Found such an edge. Now create a face along with two
							// degenerate quads from these two edges.

							newVertBuffer[nextVertex] = newVertBuffer[edgeMapping[nAfter].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								1] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								2] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][0]];
							// Recompute the normal
							D3DXVECTOR3 v1 = newVertBuffer[nextVertex + 1].Position -
								newVertBuffer[nextVertex].Position;
							D3DXVECTOR3 v2 = newVertBuffer[nextVertex + 2].Position -
								newVertBuffer[nextVertex + 1].Position;
							D3DXVec3Normalize( &v1, &v1 );
							D3DXVec3Normalize( &v2, &v2 );
							D3DXVec3Cross( &newVertBuffer[nextVertex].Normal, &v1, &v2 );
							newVertBuffer[nextVertex + 1].Normal = newVertBuffer[nextVertex +
								2].Normal = newVertBuffer[nextVertex].Normal;

							newIndexBuffer[nextIndex] = nextVertex;
							newIndexBuffer[nextIndex + 1] = nextVertex + 1;
							newIndexBuffer[nextIndex + 2] = nextVertex + 2;

							// 1st quad

							newIndexBuffer[nextIndex + 3] = edgeMapping[nBefore].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 4] = edgeMapping[nBefore].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 5] = nextVertex + 1;

							newIndexBuffer[nextIndex + 6] = nextVertex + 2;
							newIndexBuffer[nextIndex + 7] = nextVertex + 1;
							newIndexBuffer[nextIndex + 8] = edgeMapping[nBefore].m_aanNewEdge[0][0];

							// 2nd quad

							newIndexBuffer[nextIndex + 9] = edgeMapping[nAfter].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 10] = edgeMapping[nAfter].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 11] = nextVertex;

							newIndexBuffer[nextIndex + 12] = nextVertex + 1;
							newIndexBuffer[nextIndex + 13] = nextVertex;
							newIndexBuffer[nextIndex + 14] = edgeMapping[nAfter].m_aanNewEdge[0][0];

							// Modify mapping entry i2 to reflect the third edge
							// of the newly added face.

							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								edgeMapping[i2].m_anOldEdge[0] = edgeMapping[i].m_anOldEdge[0];
							}
							else
							{
								edgeMapping[i2].m_anOldEdge[1] = edgeMapping[i].m_anOldEdge[1];
							}
							edgeMapping[i2].m_aanNewEdge[0][0] = nextVertex + 2;
							edgeMapping[i2].m_aanNewEdge[0][1] = nextVertex;

							// Update next vertex/index positions

							nextVertex += 3;
							nextIndex += 15;

							break;
						}
					}
				}
			}
			else
			{
				// This is a shared edge.  Create the degenerate quad.

				// First triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];

				// Second triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
			}
		}
	}



    // At this time, the output mesh may have an index buffer
    // bigger than what is actually needed, so we create yet
    // another mesh with the exact IB size that we need and
    // output it.  This mesh also uses 16-bit index if
    // 32-bit is not necessary.


	int numVertices=(( faceCount + numMaps ) *3);
	int numIndices=( nextIndex );
	SHADOWVERT* finalVertBuffer=new SHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* finalIndexBuffer= new unsigned int[( nextIndex)] ();

    ZeroMemory( finalVertBuffer,sizeof(SHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( finalIndexBuffer, 4 * ( nextIndex) );

    CopyMemory( finalVertBuffer, newVertBuffer, sizeof( SHADOWVERT ) * (( faceCount + numMaps ) *3));
    CopyMemory( finalIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

      
	int finalVert=( faceCount + numMaps ) *3;
	int finalIndex=nextIndex;
    //release the ID3DX10Mesh
    adjacentPointRepBuffer->Unmap();
    adjacentPointRepBuffer->Release();

    tempMesh->Release();
	delete(edgeMapping);
	
	delete(newIndexBuffer);
	delete(newVertBuffer);
	delete(patchIndexBuffer);
	delete(patchVertBuffer);







	aModel->myShadowModel= new Model();
	aModel->myShadowModel->myCastsShadowFlag=aModel->myCastsShadowFlag;
	aModel->myShadowModel->myIsNULLObject=aModel->myIsNULLObject;
	aModel->myShadowModel->myFileName=aModel->myFileName;

	aModel->myShadowModel->myAnimation=aModel->myAnimation;
	aModel->myShadowModel->myOrientation=aModel->myOrientation;

	aModel->myShadowModel->myVertexIndexData.myNrOfIndexes=numIndices;
	aModel->myShadowModel->myVertexIndexData.myFormat=DXGI_FORMAT_R32_UINT;
	aModel->myShadowModel->myVertexIndexData.mySize=aModel->myShadowModel->myVertexIndexData.myNrOfIndexes*sizeof(unsigned int);
	
	aModel->myShadowModel->myVertexIndexData.myIndexData= new  char [aModel->myShadowModel->myVertexIndexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexIndexData.myIndexData,finalIndexBuffer,aModel->myShadowModel->myVertexIndexData.mySize);

	aModel->myShadowModel->myVertexData.myNrOfVertexes=numVertices;
	aModel->myShadowModel->myVertexData.mySize=aModel->myShadowModel->myVertexData.myNrOfVertexes*sizeof(SHADOWVERT);
	aModel->myShadowModel->myVertexData.myStride=sizeof(SHADOWVERT);
	aModel->myShadowModel->myVertexData.myType=VertexTypePosNorm;
	aModel->myShadowModel->myVertexData.myVertexData = new  char [aModel->myShadowModel->myVertexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexData.myVertexData,finalVertBuffer,aModel->myShadowModel->myVertexData.mySize);

	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[0]);
	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[1]);

	aModel->myShadowModel->myVertexType=VertexTypePosNorm;
	aModel->myShadowModel->myBoneList=aModel->myBoneList;


	Surface surf;
	surf.SetDiffuseTexture(aModel->mySurfaces[0].GetDiffuseTexture()->GetFileName());

	surf.SetPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	surf.SetVertexStart(0);
	surf.SetIndexStart(0);
	surf.SetIndexCount(numIndices);
	surf.SetAdjacentPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
	aModel->myShadowModel->mySurfaces.Add(surf);
	
	SelectEffect(aModel->myShadowModel);

	delete(finalIndexBuffer);
	delete(finalVertBuffer);

	return(true);

}

struct SKINNEDSHADOWVERT
{
    D3DXVECTOR3 Position;
	unsigned char myWeights[4];
	unsigned char myBones[4];
    D3DXVECTOR3 Normal;
};





bool DXSDKMeshFactory::CreateSkinnedShadowMesh(Model* aModel,SDKMeshFileHeader& aFileHeader,int aMeshIndex)
{
	int i;
    ID3DX10Mesh* tempMesh = NULL;

	const D3D10_INPUT_ELEMENT_DESC shadowVertLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,      12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,         16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,      20, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

   	SKINNEDSHADOWVERT* baseVertexBuffer=new SKINNEDSHADOWVERT[aModel->myVertexData.myNrOfVertexes] ();
	unsigned int* baseIndexBuffer=  new unsigned int [aModel->myVertexIndexData.myNrOfIndexes] ();

	unsigned char* copybaseVertexBuffer=(unsigned char*)baseVertexBuffer;
	unsigned short* copybaseInitBuffer=(unsigned short*)aModel->myVertexIndexData.myIndexData;
	if(aModel->myVertexIndexData.myFormat!=DXGI_FORMAT_R32_UINT )
	{
		for(i=0;i<aModel->myVertexIndexData.myNrOfIndexes;i++)
		{
				baseIndexBuffer[i]=copybaseInitBuffer[i];
		}
	}
	else
	{
		memcpy(baseIndexBuffer,copybaseInitBuffer,4*aModel->myVertexIndexData.myNrOfIndexes);
	}

	for(i=0;i<aModel->myVertexData.myNrOfVertexes;i++)
	{
		memcpy((void*)&baseVertexBuffer[i],(void*)&aModel->myVertexData.myVertexData[i*aModel->myVertexData.myStride],sizeof(SKINNEDSHADOWVERT));
	}


	


	
	

	unsigned int options=0;
//    if( 1==  meshIndexHeader[IBIndex].IndexType )
        options |= D3DX10_MESH_32_BIT;
	D3DX10CreateMesh( Engine::GetInstance()->GetDevice(),
								shadowVertLayout,
                                4,
                                shadowVertLayout[0].SemanticName,
                                aModel->myVertexData.myNrOfVertexes,
								aModel->myVertexIndexData.myNrOfIndexes / 3,
                                options,
                                &tempMesh );

    //set the VB // got to convert for real :/
	tempMesh->SetVertexData( 0, ( void* )(baseVertexBuffer) );
	 //set the IB
    tempMesh->SetIndexData( ( void* ) (baseIndexBuffer ),aModel->myVertexIndexData.myNrOfIndexes);
    //generate adjacency
	HRESULT hr = S_OK;
    hr =tempMesh->GenerateAdjacencyAndPointReps( 0.0001f );
    //generate adjacency indices
    tempMesh->GenerateGSAdjacency();


  
	
	//get the adjacency data out of the mesh

    ID3DX10MeshBuffer* adjacentBuffer = NULL;
    unsigned int* adjacentData = NULL;
    SIZE_T bufferSize = 0;
    hr =tempMesh->GetAdjacencyBuffer(&adjacentBuffer) ;
    hr =adjacentBuffer->Map( ( void** )&adjacentData, &bufferSize);


    ID3DX10MeshBuffer* adjacentPointRepBuffer = NULL;
    unsigned int* adjacentPointRepData = NULL;
    SIZE_T bufferSize2 = 0;
    hr =tempMesh->GetPointRepBuffer(&adjacentPointRepBuffer) ;
    hr =adjacentPointRepBuffer->Map( ( void** )&adjacentPointRepData, &bufferSize2);


	


	
	int numEdges=tempMesh->GetFaceCount()*3;
	int numMaps=0;
	int nextIndex=0;

    EdgeMapping* edgeMapping = new EdgeMapping[numEdges];


	int faceCount=tempMesh->GetFaceCount();
	SKINNEDSHADOWVERT* newVertBuffer=new SKINNEDSHADOWVERT [(faceCount * 3)] ();
	unsigned int* newIndexBuffer= new unsigned int[((faceCount + numEdges * 2)*3)] ();

    ZeroMemory( newVertBuffer,sizeof(SKINNEDSHADOWVERT)*(faceCount * 3) );
    ZeroMemory( newIndexBuffer,4*((faceCount + numEdges * 2)*3));

	int newVert=faceCount * 3;
	int newIndex=(faceCount + numEdges * 2)*3;


     SKINNEDSHADOWVERT* nextOutVertex = (SKINNEDSHADOWVERT*)newVertBuffer;



    // Iterate through the faces.  For each face, output new
    // vertices and face in the new mesh, and write its edges
    // to the mapping table.

    for( int f = 0; f < faceCount; ++f )
    {
        // Copy the vertex data for all 3 vertices
        CopyMemory( nextOutVertex, baseVertexBuffer + baseIndexBuffer[f * 3], sizeof( SKINNEDSHADOWVERT ) );
        CopyMemory( nextOutVertex + 1, baseVertexBuffer + baseIndexBuffer[f * 3 + 1], sizeof( SKINNEDSHADOWVERT ) );
        CopyMemory( nextOutVertex + 2, baseVertexBuffer + baseIndexBuffer[f * 3 + 2], sizeof( SKINNEDSHADOWVERT ) );

        // Write out the face
        newIndexBuffer[nextIndex++] = f * 3;
        newIndexBuffer[nextIndex++] = f * 3 + 1;
        newIndexBuffer[nextIndex++] = f * 3 + 2;

        // Compute the face normal and assign it to
        // the normals of the vertices.
        D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
        D3DXVECTOR3 vNormal;
        v1 = *( D3DXVECTOR3* )( nextOutVertex + 1 ) - *( D3DXVECTOR3* )nextOutVertex;
        v2 = *( D3DXVECTOR3* )( nextOutVertex + 2 ) - *( D3DXVECTOR3* )( nextOutVertex + 1 );
        D3DXVec3Cross( &vNormal, &v1, &v2 );
        D3DXVec3Normalize( &vNormal, &vNormal );

        nextOutVertex->Normal = -vNormal;
        ( nextOutVertex + 1 )->Normal = -vNormal;
        ( nextOutVertex + 2 )->Normal = -vNormal;

        nextOutVertex += 3;

        // Add the face's edges to the edge mapping table

        // Edge 1
        int nIndex;
        int nVertIndex[3] =
        {
            adjacentPointRepData[baseIndexBuffer[f * 3]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 1]],
            adjacentPointRepData[baseIndexBuffer[f * 3 + 2]]
        };
        nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            // No entry for this edge yet.  Initialize one.
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 2
        nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
            hr = E_INVALIDARG;
            assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }

        // Edge 3
        nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], edgeMapping, numEdges );

        // If error, we are not able to proceed, so abort.
        if( -1 == nIndex )
        {
			assert(0);
        }

        if( edgeMapping[nIndex].m_anOldEdge[0] == -1 && edgeMapping[nIndex].m_anOldEdge[1] == -1 )
        {
            edgeMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
            edgeMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
            edgeMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

            ++numMaps;
        }
        else
        {
            // An entry is found for this edge.  Create
            // a quad and output it.
            assert( numMaps > 0 );

            edgeMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
            edgeMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

            // First triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];

            // Second triangle
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][1];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[1][0];
            newIndexBuffer[nextIndex++] = edgeMapping[nIndex].m_aanNewEdge[0][0];

            // edgeMapping[nIndex] is no longer needed. Copy the last map entry
            // over and decrement the map count.

            edgeMapping[nIndex] = edgeMapping[numMaps - 1];
            FillMemory( &edgeMapping[numMaps - 1], sizeof( edgeMapping[numMaps - 1] ), 0xFF );
            --numMaps;
        }
    }

    // Now the entries in the edge mapping table represent
    // non-shared edges.  What they mean is that the original
    // mesh has openings (holes), so we attempt to patch them.
    // First we need to recreate our mesh with a larger vertex
    // and index buffers so the patching geometry could fit.


	SKINNEDSHADOWVERT* patchVertBuffer=new SKINNEDSHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* patchIndexBuffer= new unsigned int[( nextIndex + 3 * numMaps * 7 )] ();

    ZeroMemory( patchVertBuffer,sizeof(SKINNEDSHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( patchIndexBuffer, 4 * ( nextIndex + 3 * numMaps * 7 ) );

    CopyMemory( patchVertBuffer, newVertBuffer, sizeof( SKINNEDSHADOWVERT ) * faceCount * 3 );
    CopyMemory( patchIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

	int patchVert=( faceCount + numMaps ) *3;
	int patchIndex=( nextIndex + 3 * numMaps * 7 );

	delete(newIndexBuffer);
	delete(newVertBuffer);
    newVertBuffer = patchVertBuffer;
    newIndexBuffer = patchIndexBuffer;
	patchVertBuffer=NULL;
	patchIndexBuffer=NULL;
	



	int nextVertex = faceCount * 3;

	for( int i = 0; i < numMaps; ++i )
	{
		if( edgeMapping[i].m_anOldEdge[0] != -1 &&
			edgeMapping[i].m_anOldEdge[1] != -1 )
		{
			// If the 2nd new edge indexes is -1,
			// this edge is a non-shared one.
			// We patch the opening by creating new
			// faces.
			if( edgeMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
				edgeMapping[i].m_aanNewEdge[1][1] == -1 )
			{
				// Find another non-shared edge that
				// shares a vertex with the current edge.
				for( int i2 = i + 1; i2 < numMaps; ++i2 )
				{
					if( edgeMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
						edgeMapping[i2].m_anOldEdge[1] != -1 &&
						( edgeMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
						  edgeMapping[i2].m_aanNewEdge[1][1] == -1 ) )
					{
						int nVertShared = 0;
						if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							++nVertShared;
						if( edgeMapping[i2].m_anOldEdge[1] == edgeMapping[i].m_anOldEdge[0] )
							++nVertShared;

						if( 2 == nVertShared )
						{
							// These are the last two edges of this particular
							// opening. Mark this edge as shared so that a degenerate
							// quad can be created for it.

							edgeMapping[i2].m_aanNewEdge[1][0] = edgeMapping[i].m_aanNewEdge[0][0];
							edgeMapping[i2].m_aanNewEdge[1][1] = edgeMapping[i].m_aanNewEdge[0][1];
							break;
						}
						else if( 1 == nVertShared )
						{
							// nBefore and nAfter tell us which edge comes before the other.
							int nBefore, nAfter;
							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								nBefore = i;
								nAfter = i2;
							}
							else
							{
								nBefore = i2;
								nAfter = i;
							}

							// Found such an edge. Now create a face along with two
							// degenerate quads from these two edges.

							newVertBuffer[nextVertex] = newVertBuffer[edgeMapping[nAfter].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								1] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][1]];
							newVertBuffer[nextVertex +
								2] = newVertBuffer[edgeMapping[nBefore].m_aanNewEdge[0][0]];
							// Recompute the normal
							D3DXVECTOR3 v1 = newVertBuffer[nextVertex + 1].Position -
								newVertBuffer[nextVertex].Position;
							D3DXVECTOR3 v2 = newVertBuffer[nextVertex + 2].Position -
								newVertBuffer[nextVertex + 1].Position;
							D3DXVec3Normalize( &v1, &v1 );
							D3DXVec3Normalize( &v2, &v2 );
							D3DXVec3Cross( &newVertBuffer[nextVertex].Normal, &v1, &v2 );
							newVertBuffer[nextVertex + 1].Normal = newVertBuffer[nextVertex +
								2].Normal = newVertBuffer[nextVertex].Normal;

							newIndexBuffer[nextIndex] = nextVertex;
							newIndexBuffer[nextIndex + 1] = nextVertex + 1;
							newIndexBuffer[nextIndex + 2] = nextVertex + 2;

							// 1st quad

							newIndexBuffer[nextIndex + 3] = edgeMapping[nBefore].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 4] = edgeMapping[nBefore].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 5] = nextVertex + 1;

							newIndexBuffer[nextIndex + 6] = nextVertex + 2;
							newIndexBuffer[nextIndex + 7] = nextVertex + 1;
							newIndexBuffer[nextIndex + 8] = edgeMapping[nBefore].m_aanNewEdge[0][0];

							// 2nd quad

							newIndexBuffer[nextIndex + 9] = edgeMapping[nAfter].m_aanNewEdge[0][1];
							newIndexBuffer[nextIndex + 10] = edgeMapping[nAfter].m_aanNewEdge[0][0];
							newIndexBuffer[nextIndex + 11] = nextVertex;

							newIndexBuffer[nextIndex + 12] = nextVertex + 1;
							newIndexBuffer[nextIndex + 13] = nextVertex;
							newIndexBuffer[nextIndex + 14] = edgeMapping[nAfter].m_aanNewEdge[0][0];

							// Modify mapping entry i2 to reflect the third edge
							// of the newly added face.

							if( edgeMapping[i2].m_anOldEdge[0] == edgeMapping[i].m_anOldEdge[1] )
							{
								edgeMapping[i2].m_anOldEdge[0] = edgeMapping[i].m_anOldEdge[0];
							}
							else
							{
								edgeMapping[i2].m_anOldEdge[1] = edgeMapping[i].m_anOldEdge[1];
							}
							edgeMapping[i2].m_aanNewEdge[0][0] = nextVertex + 2;
							edgeMapping[i2].m_aanNewEdge[0][1] = nextVertex;

							// Update next vertex/index positions

							nextVertex += 3;
							nextIndex += 15;

							break;
						}
					}
				}
			}
			else
			{
				// This is a shared edge.  Create the degenerate quad.

				// First triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];

				// Second triangle
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][1];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[1][0];
				newIndexBuffer[nextIndex++] = edgeMapping[i].m_aanNewEdge[0][0];
			}
		}
	}



    // At this time, the output mesh may have an index buffer
    // bigger than what is actually needed, so we create yet
    // another mesh with the exact IB size that we need and
    // output it.  This mesh also uses 16-bit index if
    // 32-bit is not necessary.


	int numVertices=(( faceCount + numMaps ) *3);
	int numIndices=( nextIndex );
	SKINNEDSHADOWVERT* finalVertBuffer=new SKINNEDSHADOWVERT[(( faceCount + numMaps ) *3)] ();
	unsigned int* finalIndexBuffer= new unsigned int[( nextIndex)] ();

    ZeroMemory( finalVertBuffer,sizeof(SKINNEDSHADOWVERT)*(( faceCount + numMaps ) *3));
    ZeroMemory( finalIndexBuffer, 4 * ( nextIndex) );

    CopyMemory( finalVertBuffer, newVertBuffer, sizeof( SKINNEDSHADOWVERT ) * (( faceCount + numMaps ) *3));
    CopyMemory( finalIndexBuffer, newIndexBuffer, sizeof( DWORD ) * nextIndex );

      
	int finalVert=( faceCount + numMaps ) *3;
	int finalIndex=nextIndex;
    //release the ID3DX10Mesh
    adjacentPointRepBuffer->Unmap();
    adjacentPointRepBuffer->Release();

    tempMesh->Release();
	delete(edgeMapping);
	
	delete(newIndexBuffer);
	delete(newVertBuffer);
	delete(patchIndexBuffer);
	delete(patchVertBuffer);







	aModel->myShadowModel= new Model();
	aModel->myShadowModel->myCastsShadowFlag=aModel->myCastsShadowFlag;
	aModel->myShadowModel->myIsNULLObject=aModel->myIsNULLObject;
	aModel->myShadowModel->myFileName=aModel->myFileName;

	aModel->myShadowModel->myAnimation=aModel->myAnimation;
	aModel->myShadowModel->myOrientation=aModel->myOrientation;

	aModel->myShadowModel->myVertexIndexData.myNrOfIndexes=numIndices;
	aModel->myShadowModel->myVertexIndexData.myFormat=DXGI_FORMAT_R32_UINT;
	aModel->myShadowModel->myVertexIndexData.mySize=aModel->myShadowModel->myVertexIndexData.myNrOfIndexes*sizeof(unsigned int);
	
	aModel->myShadowModel->myVertexIndexData.myIndexData= new  char [aModel->myShadowModel->myVertexIndexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexIndexData.myIndexData,finalIndexBuffer,aModel->myShadowModel->myVertexIndexData.mySize);

	aModel->myShadowModel->myVertexData.myNrOfVertexes=numVertices;
	aModel->myShadowModel->myVertexData.mySize=aModel->myShadowModel->myVertexData.myNrOfVertexes*sizeof(SKINNEDSHADOWVERT);
	aModel->myShadowModel->myVertexData.myStride=sizeof(SKINNEDSHADOWVERT);
	aModel->myShadowModel->myVertexData.myType=VertexTypePosWeightBonesNorm;
	aModel->myShadowModel->myVertexData.myVertexData = new  char [aModel->myShadowModel->myVertexData.mySize] ();
	memcpy(aModel->myShadowModel->myVertexData.myVertexData,finalVertBuffer,aModel->myShadowModel->myVertexData.mySize);

	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[0]);
	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[1]);
	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[2]);
	aModel->myShadowModel->myVertexFormat.Add(shadowVertLayout[3]);

	aModel->myShadowModel->myVertexType=VertexTypePosWeightBonesNorm;
	aModel->myShadowModel->myBoneList=aModel->myBoneList;


	Surface surf;
	surf.SetDiffuseTexture(aModel->mySurfaces[0].GetDiffuseTexture()->GetFileName());

	surf.SetPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	surf.SetVertexStart(0);
	surf.SetIndexStart(0);
	surf.SetIndexCount(numIndices);
	surf.SetAdjacentPrimologyType(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);
	aModel->myShadowModel->mySurfaces.Add(surf);
	
	SelectEffect(aModel->myShadowModel);

	delete(finalIndexBuffer);
	delete(finalVertBuffer);

	return(true);

}

