#ifndef NEMESIS3D_COLLADAMESHFACTORY_HEADER
#define NEMESIS3D_COLLADAMESHFACTORY_HEADER

#include <string>
#include "Matrix.h"
#include "debugHeader.h"

class FCDocument;
class FCDController;
class FCDControllerInstance;
class FCDSceneNode;
class FCDEntityInstance;
class FCDGeometryPolygons;
class FCDGeometryInstance;
class FCDGeometry;
class FCDGeometryMesh;
class FCDMaterial;
class FCDAnimationCurve;
class FCDTransform;


struct EdgeMapping;


	class AnimationCurve;
	class TransformationNode;
	class Model;
	class TransformationNode_Curves;

	class ColladaMeshFactory
	{
	public:
		ColladaMeshFactory();
		~ColladaMeshFactory();

		Model* CreateModel(std::string aFileName);

	protected:

		void BuildHierarchy(FCDocument* aColladaDocument,Model* aModel);
		void BuildHierarchyRecursive(FCDocument* aColladaDocument, FCDSceneNode* aSceneNode,Model* aModel);
		TransformationNode* GetTransForm(FCDSceneNode* aSceneNode);

		AnimationCurve* CreateAnimationCurve(FCDAnimationCurve* aCurve);
		AnimationCurve* CreateAnimationCurveRot(FCDAnimationCurve* aCurve);
		void FindAndDecodeInstance(Model* aModel,FCDEntityInstance* aInstance,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag);
//		void DecodeInstance(Model* aModel,FCDEntityInstance* aInstance,FCDocument* aColladaDocument);
		void DecodeInstance(Model* aModel,FCDGeometry* aGeometry,FCDGeometryInstance* aGeometryInstance,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag,FCDController* aController,FCDControllerInstance* aControllerInstance);
		void GenerateMeshDatafromPolygons(Model* aModel,FCDGeometryPolygons* somePolygons,FCDGeometryMesh* mesh,FCDocument* aColladaDocument,bool aHasBoneWeightsFlag,FCDController* aController,FCDControllerInstance* aControllerInstance);
		bool LoadModel(Model* aModel,std::string aFileName);
		bool DetermineVertexType(Model* aModel);
		void LoadMaterial(Model* aModel,FCDMaterial* aMaterial,FCDocument* aColladaDocument);
//		void DecodeSkinningData(Model* aModel);
		void BuildBoneHierarchy(Model* aModel,FCDocument* aColladaDocument, FCDSceneNode* aSceneNode,int aParentIndex,FCDController* aController,FCDControllerInstance* aControllerInstance);


		TransformationNode_Curves* GetTranslationTransform(FCDTransform* aTransform);
		TransformationNode_Curves* GetRotationTransformation(FCDTransform* aTransform);
		void ApplayTransformationTranslation(FCDTransform* aTransform, Matrix44f& aOrientation);
		void ApplayTransformationRotation(FCDTransform* aTransform, Matrix44f& aOrientation);
		
		bool SelectEffect(Model* aModel);

		bool CreateShadowMesh(Model* aModel);
//		bool CreateSkinnedShadowMesh(Model* aModel);
		int FindEdgeInMappingTable( int nV1, int nV2, EdgeMapping* pMapping, int nCount );
		Model* parentModel;

	};


#endif