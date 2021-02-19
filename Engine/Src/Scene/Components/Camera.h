#pragma once
#include "Component.h"


namespace Dive
{
	class Context;
	class GameObject;
	class FileStream;
	class Transform;

	enum class eProjectionType
	{
		Perspective,
		Orthographic,
	};

	class Camera : public Component
	{
		DIVE_OBJECT(Camera, Component);

	public:
		Camera(Context* context, GameObject* owner);
		~Camera();

		// serialize
		// 왜인지 모르겠지만 에러가 난다...
		//void Serialize(FileStream& stream) override;
		//void Deserialize(FileStream& stream) override;

		void OnUpdate() override;

		// get & set
		eProjectionType GetProjectionType() const { return m_projectionType; }
		void SetProjectionType(eProjectionType type) { m_projectionType = type; }

		// XMFLOAT4는 에바다.
		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_backgroundColor; }
		void SetBackgroundColor(DirectX::XMFLOAT4 color) { m_backgroundColor = std::move(color); }

		DirectX::XMMATRIX GetViewMatrix() const { return DirectX::XMLoadFloat4x4(&m_view); }
		DirectX::XMMATRIX GetProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&m_projection); }
		DirectX::XMMATRIX GetViewProjectionMatrix() const { return GetViewMatrix() * GetProjectionMatrix(); }

	private:
		void computeViewMatrix();
		void computeProjectionMatrix();

	private:
		DirectX::XMFLOAT4X4 m_view;
		DirectX::XMFLOAT4X4 m_projection;

		eProjectionType m_projectionType;

		D3D11_VIEWPORT m_viewPort;

		float m_fovHorizontal;
		float m_nearPlane;
		float m_farPlane;

		DirectX::XMFLOAT4 m_backgroundColor;

		Transform* m_transform;
	};
}
