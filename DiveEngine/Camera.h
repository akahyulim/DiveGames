#pragma once
#include "Component.h"
#include <DirectXMath.h>

namespace dive
{
	class GameObject;

	enum class eProjectionType
	{
		Perspective,
		Orthographic,
	};

	// 주 역할은 입력을 받아 Transform을 변한시키고
	// 이를 이용하여 View, Projection Matrix를 계산한다.
	// 이외에도 컬링, 픽킹 등의 기능을 수행한다.
	class Camera : public Component
	{
	public:
		Camera(GameObject* owner);
		~Camera();

		void Update(float deltaTime) override;

		eProjectionType GetProjectionType() const { return mProjectionType; }
		void SetProjectionType(eProjectionType type) { mProjectionType = type; }

		float GetNearPlane() const { return mNearPlane; }
		void SetNearPlane(float nearPlane) { mNearPlane = nearPlane; }
		float GetFarPlane() const { return mFarPlane; }
		void SetFarPlane(float farPlane) { mFarPlane = farPlane; }
		
		DirectX::XMFLOAT4 GetClearClor() const { return mClearColor; }
		void SetClearColor(const DirectX::XMFLOAT4& color) { mClearColor = color; }
		void SetClearColor(float r, float g, float b, float a) { mClearColor = DirectX::XMFLOAT4(r, g, b, a); }

	private:
		DirectX::XMMATRIX computeViewMatrix() const;

	private:
		eProjectionType mProjectionType;

		float mNearPlane;
		float mFarPlane;

		DirectX::XMFLOAT3 mPosition;
		DirectX::XMFLOAT4 mClearColor;
		DirectX::XMFLOAT4X4 mView;
		DirectX::XMFLOAT4X4 mProjection;
	};
}