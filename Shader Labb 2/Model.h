#ifndef _MODEL_H_
#define _MODEL_H_

#include "Vector.h"
#include "Matrix.h"
#include "CU_GrowingArray.h"
#include "Effect.h"
#include "Surface.h"
#include "BufferWrapper.h"
#include "TransformationNode.h"
#include "BoneHierarchy.h"
#include "Bone.h"

class RenderProcessTarget;

class Model
{
	friend class DXSDKMeshFactory;
	friend class ColladaMeshFactory;
	friend class Instance;

public:
	Model(void);
	~Model(void);

	bool InitBuffers();
	bool InitIndexBuffer();
	bool InitVertexBuffer();
	bool InitAdjacencyIndexBuffer();

	Effect* GetEffect();
	CommonUtilities::GrowingArray<Surface>& GetSurfaces();
	Vector3f GetDimensions() const;
	std::string GetFileName();
	float GetRadius();
	BoneHierarchy* GetAnimation();

	void SetEffect(Effect* aEffect);
	void SetFileName(std::string aFileName);
	
	void AddChild(Model* aModel, TransformationNode* aOrientationNode);
	void Render( Matrix44< float > anOrientation, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray, const EffectTechniques::TechniqueType aTechniqueType = EffectTechniques::NORMAL );
	void RenderUsingEffect( Matrix44< float > anOrientation, const EffectTechniques::TechniqueType aTechniqueType = EffectTechniques::NORMAL, Effect* anEffect = NULL );

	void SendLayoutAndBuffersToGPU();

	void RenderToTarget( const Matrix44f anOrientation, CommonUtilities::StaticArray< Matrix44f, 255 >& aBoneMatrixArray, RenderProcessTarget& aRenderTarget, const EffectTechniques::TechniqueType aTechniqueType = EffectTechniques::NORMAL );
	
	void DisableShadowCasting();
	void TransformBindPose(Matrix44f aMatrix, int aBoneIndex = 0);
	void SetEnviromentMap( ID3D10ShaderResourceView* aCubeMap );
	
	const int GetID();

private:
	void CalculateRadius();

	void RenderShadow(EffectTechniques::TechniqueType aTechinqueType);


	Effect* myEffect;
	VertexType myVertexType;

	CommonUtilities::GrowingArray<Surface> mySurfaces;
	//CommonUtilities::GrowingArray<unsigned int> myVerticeIndexes;
	//CommonUtilities::GrowingArray<VertexPosNormUV> myVertices;
	CommonUtilities::GrowingArray<Model*> myChilds;
	CommonUtilities::GrowingArray<TransformationNode*> myChildsTransformations;

	CommonUtilities::GrowingArray<D3D10_INPUT_ELEMENT_DESC, short> myVertexFormat;

	std::string myFileName;
	bool myIsNULLObject;
	Matrix44f myOrientation;
	
	IndexBufferWrapper myIndexBuffer;
	IndexBufferWrapper myAdjacencyIndexBuffer;
	VertexIndexWrapper myVertexIndexData;
	VertexIndexWrapper myAdjacencyVertexIndexData;
	VertexDataWrapper myVertexData;
	VertexBufferWrapper myVertexBuffer;
	ID3D10InputLayout* myVertexLayout;

	Model* myShadowModel;
	Model* myShadowModelRef;

	CommonUtilities::GrowingArray<Bone> myBones;
	CommonUtilities::GrowingArray<int> myBoneList;
	BoneHierarchy* myAnimation;

	float myRadius;
	bool myRadiusCalculated;
	bool myCastsShadowFlag;

	Matrix44f myOldOrientation;

	static int ourIDPool;
	int myID;

};

#endif