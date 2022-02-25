#pragma once
#include "Camera.h"

namespace Dive
{
	class GameObject;

	enum class eProjectionType
	{
		Perspective,
		Orthographic
	};

	class SceneCamera : public Camera
	{
	public:
		SceneCamera(GameObject* pGameObject);
		SceneCamera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat);
		~SceneCamera() = default;

	private:

	private:
		eProjectionType m_ProjectionType;
	};
}