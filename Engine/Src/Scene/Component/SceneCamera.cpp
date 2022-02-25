#include "divepch.h"
#include "SceneCamera.h"
#include "Scene/GameObject.h"

namespace Dive
{
	SceneCamera::SceneCamera(GameObject* pGameObject)
		: Camera(pGameObject)
	{
	}

	SceneCamera::SceneCamera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat)
		: Camera(pGameObject, mat)
	{
	}
}