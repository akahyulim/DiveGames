#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"

namespace dive
{
	Transform::Transform()
		: Component(typeid(Transform).hash_code())
	{

	}

	Transform::~Transform()
	{
	}

	void Transform::Update(float deltaTime)
	{
	}

	void Transform::Serialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		fileStream->Write(mParent ? mParent->GetOwner()->GetID() : 0);
	}

	void Transform::Deserialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		unsigned int parentId = 0;
		fileStream->Read(&parentId);

		if (parentId != 0)
		{
			if (auto parentTransform = Scene::GetGlobalScene().GetGameObjectByID(parentId))
			{
				SetParent(parentTransform->GetTransform());
			}
		}

		// update transform
	}
}