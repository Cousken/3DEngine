#include "BoneInstanceHierarchy.h"

BoneInstanceHierarchy::~BoneInstanceHierarchy(void)
{
}

void BoneInstanceHierarchy::Update( float aElapsedTime )
{
	myElapsedTime+=aElapsedTime;
	for (int i =0;i<myTransformationInstances.Count();i++)
	{
		myTransformationInstances[i]->Update(aElapsedTime);
	}
}

const CommonUtilities::GrowingArray<TransformationNodeInstance*>& BoneInstanceHierarchy::GetTransformationAnimations()
{
	return myTransformationInstances;
}

BoneInstanceHierarchy::BoneInstanceHierarchy( BoneHierarchy* aAnimation )
{
	myAnimation = aAnimation;
	myElapsedTime = 0;

	myTransformationInstances.Init( myAnimation->GetTransformationNodes().Count(), 1 );

	for( int i = 0; i < myAnimation->GetTransformationNodes().Count(); i++ )
	{
		TransformationNodeInstance* temp = new TransformationNodeInstance;
		temp->SetTransformationNode( myAnimation->GetTransformationNodes()[i] );
		myTransformationInstances.Add( temp );
	}
}
