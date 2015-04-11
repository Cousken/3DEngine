#include "Model.h"
#include "Engine.h"
#include "EffectTechnique.h"

int Model::ourIDPool = 0;

Model::Model(void)
{
	mySurfaces.Init(1,1);
	myChilds.Init(1,1);
	myChildsTransformations.Init(1,1);
	myVertexFormat.Init(1,1);
	myBones.Init(1,1);
	myBoneList.Init(1,1);

	myRadius = 0;
	myRadiusCalculated = false;
	myIsNULLObject = false;
	myCastsShadowFlag = true;
	myOrientation = Matrix44f::Identity();
	
	myVertexLayout = NULL;
	myAnimation = NULL;
	myEffect = NULL;

	myIndexBuffer.myIndexBuffer	= NULL;
	myIndexBuffer.myByteOffset = 0;
	myIndexBuffer.myIndexBufferFormat = DXGI_FORMAT_R32_UINT;

	myVertexBuffer.myVertexBuffer	= NULL;
	myVertexBuffer.myByteOffset = 0;
	myVertexBuffer.myNbrOfBuffers = 1;
	myVertexBuffer.myStartSlot = 0;
	myVertexBuffer.myStride = sizeof( VertexPosNormUV );

	myID = ourIDPool;
	ourIDPool++;

	myOldOrientation = Matrix44f::Identity();
}

Model::~Model(void)
{
}

Effect* Model::GetEffect()
{
	return myEffect;
}

void Model::SetEffect( Effect* aEffect )
{
	myEffect = aEffect;
	Model::InitBuffers();
}

CommonUtilities::GrowingArray<Surface>& Model::GetSurfaces()
{
	return mySurfaces;
}

float Model::GetRadius()
{
	if ( myRadiusCalculated == true ) 
	{
		return myRadius;
	}
	CalculateRadius();
	return myRadius;
}

void Model::CalculateRadius()
{
// 	float longestDistanceToOrigo = 0;
// 
// 	const int nbrOfVertices = myVertexData.myNrOfVertexes;
// 	for (int vertexMeasured = 0; vertexMeasured < nbrOfVertices; vertexMeasured++)
// 	{
// 		float currentVertexLength = myVertexData.myVertexData[vertexMeasured].myPos.Length();
// 		if ( currentVertexLength > longestDistanceToOrigo)
// 		{
// 			longestDistanceToOrigo = currentVertexLength;
// 		}
// 	}
// 	
// 	const int nbrOfChildren = myChilds.Count();
// 	for (int childChecked = 0; childChecked < nbrOfChildren; childChecked++)
// 	{
// 		float radius = myChilds[childChecked]->GetRadius();
// 		if ( radius > longestDistanceToOrigo)
// 		{
// 			longestDistanceToOrigo = radius;
// 		}
// 	}	
// 	
// 	myRadius = longestDistanceToOrigo;
	myRadius = 1000;

	myRadiusCalculated = true;
}

bool Model::InitBuffers()
{
	HRESULT hr = S_OK;

 	if ( myIsNULLObject == false)
	{
		// Create the input layout
		D3D10_PASS_DESC PassDesc;
		myEffect->GetTechnique()->GetPassByIndex( 0 )->GetDesc( &PassDesc );
		UINT nbrOfElements = myVertexFormat.Count();
		hr = Engine::GetInstance()->GetDevice()->CreateInputLayout( &myVertexFormat[0], nbrOfElements, 
			PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize, &myVertexLayout );

		if( FAILED( hr ) )
		{
			return false;
		}

		if (InitVertexBuffer() == false)
		{
			return false;
		}

		if (InitIndexBuffer() == false)
		{
			return false;
		}
	}

	const int nbrOfChildren = myChilds.Count();
	bool succes = false;
	for (int childInitiated = 0; childInitiated < nbrOfChildren; childInitiated++)
	{
		succes = myChilds[childInitiated]->InitBuffers();
		assert( (succes == true) && "Failed to initiate the child buffer!");
		if ( succes == false)
		{
			return false;
		}		
	}

	return true;
}

bool Model::InitIndexBuffer()
{
	HRESULT hr = S_OK;

	D3D10_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D10_USAGE_DEFAULT;
	bufferDescription.ByteWidth = myVertexIndexData.mySize;        // 36 vertices needed for 12 triangles in a triangle list
	bufferDescription.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0; 
	bufferDescription.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = myVertexIndexData.myIndexData;
	myIndexBuffer.myIndexBufferFormat = myVertexIndexData.myFormat;

	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bufferDescription, &initData, &myIndexBuffer.myIndexBuffer );
	if( FAILED( hr ) )
		return hr;

	return true;
}

bool Model::InitVertexBuffer()
{
	HRESULT hr = S_OK;

	// Create vertex buffer
	D3D10_BUFFER_DESC bufferDescription;
	bufferDescription.Usage = D3D10_USAGE_DEFAULT;
	bufferDescription.ByteWidth = myVertexData.mySize;
	bufferDescription.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = myVertexData.myVertexData;

	myVertexBuffer.myStride = myVertexData.myStride;

	hr = Engine::GetInstance()->GetDevice()->CreateBuffer( &bufferDescription, &initData, &myVertexBuffer.myVertexBuffer );
	if( FAILED( hr ) )
		return false;	

	return true;
}

void Model::Render( Matrix44< float > anOrientation, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray, const EffectTechniques::TechniqueType aTechniqueType /*= EffectTechniques::NORMAL */ )
{
	Matrix44f newOrientation = anOrientation;

	if ( myIsNULLObject == false )
	{
		SendLayoutAndBuffersToGPU();

		Engine::GetInstance()->GetEffectInput().UpdateWorld(newOrientation);
		//Engine::GetInstance()->GetEffectInput().UpdatePreviousWorld(myOldOrientation);
		//myOldOrientation = newOrientation;

		D3D10_TECHNIQUE_DESC techDesc;
		HRESULT hr = GetEffect()->GetTechnique( aTechniqueType )->GetDesc( &techDesc );
		if ( FAILED(hr) ) 
		{
			assert(false, "Probably could not find correct technique in shader.");
		}

		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			const int nbrOfSurfaces = mySurfaces.Count();
			for(int surfaceRendered=0; surfaceRendered < nbrOfSurfaces; surfaceRendered++)
			{
				Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology(mySurfaces[surfaceRendered].GetPrimologyType());
				Engine::GetInstance()->GetEffectInput().SetDiffuseMap(mySurfaces[surfaceRendered].GetDiffuseTexture());
				Texture* envirMap = mySurfaces[surfaceRendered].GetEnviromentMap();
				if (envirMap != 0)
				{
					Engine::GetInstance()->GetEffectInput().SetEnviromentMap(envirMap);
				}
				else
				{
					Engine::GetInstance()->GetEffectInput().SetEmptyEnviromentMap();
				}

				Texture* normalMap =  mySurfaces[surfaceRendered].GetNormalMap();
				if (normalMap != 0)
				{
					Engine::GetInstance()->GetEffectInput().SetNormalMap(normalMap);
				}

				CommonUtilities::StaticArray< Matrix44f, 255 > usedBones;
				for ( int checkedBone = 0; checkedBone < myBoneList.Count(); checkedBone++)
				{
					usedBones.Insert(checkedBone, aBoneMatrixArray[myBoneList[checkedBone]]);
				}

				GetEffect()->SetBoneMatrixVariable( usedBones );
				GetEffect()->GetTechnique(aTechniqueType)->GetPassByIndex( p )->Apply( 0 );
				Engine::GetInstance()->GetDevice()->DrawIndexed( mySurfaces[surfaceRendered].GetIndexCount(), mySurfaces[surfaceRendered].GetIndexStart(), mySurfaces[surfaceRendered].GetVertexStart());
			}
		}
	}
}

void Model::RenderUsingEffect( Matrix44< float > anOrientation, const EffectTechniques::TechniqueType aTechniqueType, Effect* anEffect)
{
	Matrix44f newOrientation = anOrientation;

	if ( myIsNULLObject == false )
	{
		SendLayoutAndBuffersToGPU();

		Engine::GetInstance()->GetEffectInput().UpdateWorld(newOrientation);


		D3D10_TECHNIQUE_DESC techDesc;
		anEffect->GetTechnique( aTechniqueType )->GetDesc( &techDesc );

		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			const int nbrOfSurfaces = mySurfaces.Count();
			for(int surfaceRendered=0; surfaceRendered < nbrOfSurfaces; surfaceRendered++)
			{
				Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology(mySurfaces[surfaceRendered].GetPrimologyType());
				anEffect->GetTechnique(aTechniqueType)->GetPassByIndex( p )->Apply( 0 );
				Engine::GetInstance()->GetDevice()->DrawIndexed( mySurfaces[surfaceRendered].GetIndexCount(), mySurfaces[surfaceRendered].GetIndexStart(), mySurfaces[surfaceRendered].GetVertexStart());
			}
		}
	}
}


void Model::RenderToTarget( const Matrix44f anOrientation, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray, RenderProcessTarget& aRenderTarget, const EffectTechniques::TechniqueType aTechniqueType )
{
	Matrix44f newOrientation = anOrientation;

	if ( myIsNULLObject == false )
	{
		aRenderTarget.AddInstance(this, newOrientation);
	}
}


void Model::SetFileName( std::string aFileName )
{
	myFileName = aFileName;
}

void Model::AddChild( Model* aModel, TransformationNode* aOrientationNode )
{
	myChilds.Add(aModel);
	myChildsTransformations.Add(aOrientationNode);
}

std::string Model::GetFileName()
{
	return myFileName;
}

void Model::RenderShadow( EffectTechniques::TechniqueType aTechinqueType )
{
	if ( myIsNULLObject == false )
	{
		EffectInput& effectInput=Engine::GetInstance()->GetEffectInput();
		Engine::GetInstance()->GetDevice()->IASetInputLayout( myVertexLayout );
		Engine::GetInstance()->GetDevice()->IASetVertexBuffers( myVertexBuffer.myStartSlot,myVertexBuffer.myNbrOfBuffers , &(myVertexBuffer.myVertexBuffer), &myVertexBuffer.myStride, &myVertexBuffer.myByteOffset );

		Engine::GetInstance()->GetDevice()->IASetIndexBuffer(myIndexBuffer.myIndexBuffer,myIndexBuffer.myIndexBufferFormat,myIndexBuffer.myByteOffset);

		D3D10_TECHNIQUE_DESC techDesc;
		myEffect->GetTechnique(aTechinqueType)->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			for(int i=0;i<mySurfaces.Count();i++)
			{
				//			mySurfaces[i].TreatAsAdjacent(true);

				Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology(mySurfaces[i].GetPrimologyType());

				myEffect->GetTechnique(aTechinqueType)->GetPassByIndex( p )->Apply( 0 );
				Engine::GetInstance()->GetDevice()->DrawIndexed( mySurfaces[i].GetIndexCount(), mySurfaces[i].GetIndexStart(),mySurfaces[i].GetVertexStart());
				//Engine::GetInstance()->myStats.AddDrawCall();
				//			Engine::GetInstance()->myStats.AddTriangles(mySurfaces[i].GetIndexCount()/3);
			}
		}
	}

	const int nbrOfChildren = myChilds.Count();
	for (int childRendered = 0; childRendered < nbrOfChildren; childRendered++)
	{
		myChilds[childRendered]->RenderShadow(aTechinqueType);
	}
}

void Model::TransformBindPose( Matrix44f aMatrix, int aBoneIndex /*= 0*/ )
{
	myBones[aBoneIndex].TransformBindPose( aMatrix );

	for( int i = 0; i < myBones[aBoneIndex].GetChildren().Count(); i++ )
	{
		TransformBindPose( myBones[aBoneIndex].GetBindPose(), myBones[aBoneIndex].GetChildren()[i] );
	}
}

BoneHierarchy* Model::GetAnimation()
{
	return myAnimation;
}

const int Model::GetID()
{
	return myID;
}

void Model::SendLayoutAndBuffersToGPU()
{
	Engine::GetInstance()->GetDevice()->IASetInputLayout( myVertexLayout );
	Engine::GetInstance()->GetDevice()->IASetVertexBuffers( 0, 1, &myVertexBuffer.myVertexBuffer, 
		&myVertexBuffer.myStride, &myVertexBuffer.myByteOffset );
	Engine::GetInstance()->GetDevice()->IASetIndexBuffer( myIndexBuffer.myIndexBuffer, 
		myIndexBuffer.myIndexBufferFormat, myIndexBuffer.myByteOffset );
}

void Model::SetEnviromentMap( ID3D10ShaderResourceView* aCubeMap )
{
	if ( myIsNULLObject == false )
	{
		const int nbrOfSurfaces = mySurfaces.Count();
		for ( int surface = 0; surface < nbrOfSurfaces; surface++ )
		{
			mySurfaces[surface].SetEnviromentalCubeMapTexture( aCubeMap );
		}
	}

	const int nbrOfChildren = myChilds.Count();
	for ( int child = 0; child < nbrOfChildren; child++)
	{
		myChilds[child]->SetEnviromentMap( aCubeMap );
	}
}
