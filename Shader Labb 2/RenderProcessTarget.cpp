#include "RenderProcessTarget.h"
#include "Model.h"
#include "Engine.h"

RenderProcessTarget::RenderProcessTarget(void)
{
}

RenderProcessTarget::~RenderProcessTarget(void)
{
}

void RenderProcessTarget::Init()
{
	myModelMeshInstancingData.Init(1,1);
}

void RenderProcessTarget::AddInstance( Model* aModel, Matrix44f aOrientation )
{
	const int id = aModel->GetID();

	while ( id >= myModelMeshInstancingData.Count() )
	{
		myModelMeshInstancingData.Add(ModelMeshInstancingData());
	}

	if ( myModelMeshInstancingData[id].myModelMesh == NULL )
	{
		myModelMeshInstancingData[id].myModelMesh = aModel;
	}
	myModelMeshInstancingData[id].myInstanceWorldMatrix.Add(aOrientation);
}

void RenderProcessTarget::Render()
{
	const int nbrOfModels = myModelMeshInstancingData.Count();
	for ( int renderedModel = 0; renderedModel < nbrOfModels; renderedModel++)
	{
		if ( myModelMeshInstancingData[renderedModel].myModelMesh != NULL )
		{
			Model* model = myModelMeshInstancingData[renderedModel].myModelMesh;
			model->SendLayoutAndBuffersToGPU();

			const int nbrOfSurfaces = model->GetSurfaces().Count();
			for(int surfaceRendered=0; surfaceRendered < nbrOfSurfaces; surfaceRendered++)
			{
				Surface& surface = model->GetSurfaces()[surfaceRendered];
				Engine::GetInstance()->GetDevice()->IASetPrimitiveTopology(surface.GetPrimologyType());
				Engine::GetInstance()->GetEffectInput().SetDiffuseMap(surface.GetDiffuseTexture());
				Texture* envirMap = surface.GetEnviromentMap();
				if (envirMap != 0)
				{
					Engine::GetInstance()->GetEffectInput().SetEnviromentMap(envirMap);
				}

				Texture* normalMap =  surface.GetNormalMap();
				if (normalMap != 0)
				{
					Engine::GetInstance()->GetEffectInput().SetNormalMap(normalMap);
				}

				model->GetEffect()->GetTechnique()->GetPassByIndex( 0 )->Apply( 0 );

				CommonUtilities::GrowingArray<Matrix44f>& instances = myModelMeshInstancingData[renderedModel].myInstanceWorldMatrix;
				const int nbrOfInstances = instances.Count();
				for( int instanceRendered = 0; instanceRendered < nbrOfInstances; instanceRendered++)
				{
					Engine::GetInstance()->GetEffectInput().UpdateWorld(instances[instanceRendered]);
					Engine::GetInstance()->GetDevice()->DrawIndexed( surface.GetIndexCount(), surface.GetIndexStart(), surface.GetVertexStart());
				}
			}
		}
	}
}

void RenderProcessTarget::Clear()
{
	myModelMeshInstancingData.RemoveAll();
}
