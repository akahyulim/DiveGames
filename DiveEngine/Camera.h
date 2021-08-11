#pragma once
#include "GraphicsInclude.h"
#include "Component.h"

namespace dive
{
	class GameObject;
	class dvRenderTexture;

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
		Camera(GameObject* pGameObject);
		~Camera();

		// override
		void Update(float deltaTime) override;
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		// functions
		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type);

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float angle);

		float GetAspect() const { return m_ViewWidth / m_ViewHeight; };

		float GetNearPlane() const { return m_NearPlane; }
		void SetNearPlane(float nearPlane);
		float GetFarPlane() const { return m_FarPlane; }
		void SetFarPlane(float farPlane);
		
		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color);
		void SetBackgroundColor(float r, float g, float b, float a) { SetBackgroundColor(DirectX::XMFLOAT4(r, g, b, a)); }

		DirectX::XMFLOAT4X4 GetView() const { return m_View; }
		DirectX::XMFLOAT4X4 GetProjection() const { return m_Projection; }
		DirectX::XMFLOAT4X4 GetViewProjection() const;

		dvRenderTexture* GetTargetTexture() { return m_pTargetTexture; }
		void SetTargetTexture(dvRenderTexture* pTexture) { m_pTargetTexture = pTexture; }

		// Viewport
		D3D11_VIEWPORT GetViewport() const { return m_Viewport; }
		//void SetViewport(float width, float height, float x = 0.0f, float y = 0.0f);
		float GetViewWidth() const { return m_ViewWidth; }
		void SetViewWidth(float width);
		float GetViewHeight() const { return m_ViewHeight; }
		void SetViewHeight(float height);
		// 이걸 어떻게 계산할까나...
		// 굳이 이 곳에서 직접 계산할 필요가 있나?
		// Viewport만 설정한 후 Inspector에서 계산해도 될 것 같은데...
		// 그럼 위의 Get, SetView Width, Height 함수도 필요없어지고...
		float GetNormalizedRectX();
		void SetNormalizedRectX(float x);
		float GetNormalizedRectY();
		void SetNormalizedRectY(float y);
		float GetNormalizedRectWidth();
		void SetNormalizedRectWidth(float width);
		float GetNormalizedRectHeight();
		void SetNormalizedRectHeight(float height);

		// static
		static std::vector<Camera*> GetAllCameras() { return s_Cameras; }
		static unsigned int GetAllCamerasCount() { return static_cast<unsigned int>(s_Cameras.size()); }

	private:
		DirectX::XMMATRIX computeViewMatrix() const;
		DirectX::XMMATRIX computeProjectionMatrix() const;

	private:
		eProjectionType m_ProjectionType;

		float m_FieldOfView;
		float m_ViewWidth;
		float m_ViewHeight;
		float m_NearPlane;
		float m_FarPlane;

		bool m_bUpdated;

		DirectX::XMFLOAT4 m_BackgroundColor;
		DirectX::XMFLOAT4X4 m_View;
		DirectX::XMFLOAT4X4 m_Projection;

		dvRenderTexture* m_pTargetTexture;

		static std::vector<Camera*> s_Cameras;

		// 추가해야할 것 같은 변수들
		// rect : 카메라의 렌더링 결과물이 나타날 화면에서의 위치 0 ~ 1
		// 이게 결국 Viewport다.
		D3D11_VIEWPORT m_Viewport;

		// pixel width, height : pixel 단위의 넒이, 높이

		// 불투명 오브젝트 정렬 모드?
	};
}