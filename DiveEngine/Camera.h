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

	enum class eCameraClearFlags
	{
		Skybox,
		SolidColor,
		Depth,
		Nothing
	};

	struct ScreenRect
	{
		ScreenRect()
		{
			x = y = width = height = 0.0f;
		}

		ScreenRect(float x, float y, float width, float height)
		{
			this->x = x;
			this->y = y;
			this->width = width;
			this->height = height;
		}

		float x, y;
		float width, height;
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

		float GetAspect() const { return static_cast<float>(m_ScreenWidth / m_ScreenHeight); };

		float GetNearPlane() const { return m_NearPlane; }
		void SetNearPlane(float nearPlane);
		float GetFarPlane() const { return m_FarPlane; }
		void SetFarPlane(float farPlane);
		
		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color);
		void SetBackgroundColor(float r, float g, float b, float a) { SetBackgroundColor(DirectX::XMFLOAT4(r, g, b, a)); }

		DirectX::XMFLOAT4X4 GetView() const { return m_View; }
		DirectX::XMMATRIX GetViewMatrix() const { return DirectX::XMLoadFloat4x4(&m_View); }
		DirectX::XMFLOAT4X4 GetProjection() const { return m_Projection; }
		DirectX::XMMATRIX GetProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&m_Projection); }
		DirectX::XMFLOAT4X4 GetViewProjection() const;
		DirectX::XMMATRIX GetViewProjectionMatrix() const { return GetViewMatrix() * GetProjectionMatrix(); }

		dvRenderTexture* GetTargetTexture() { return m_pTargetTexture; }
		void SetTargetTexture(dvRenderTexture* pTexture) { m_pTargetTexture = pTexture; }

		// Viewport
		D3D11_VIEWPORT GetViewport() const { return m_Viewport; }
		D3D11_VIEWPORT* GetViewportPtr() { return &m_Viewport; }
		unsigned int GetScreenWidth() const { return m_ScreenWidth; }
		unsigned int SetScreenHeight() const { return m_ScreenHeight; }
		void SetScreenSize(unsigned int width, unsigned int height);
		ScreenRect GetScreenRect() const { return m_ScreenRect; }
		void SetScreenRect(float x, float y, float width, float height);

		// 이건 Orthographic 전용 같다. ScreenSize와는 다르다.
		float GetViewWidth() const { return m_ViewWidth; }
		void SetViewWidth(float width);
		float GetViewHeight() const { return m_ViewHeight; }
		void SetViewHeight(float height);

		eCameraClearFlags GetClearFlags() const { return m_ClearFlags; }
		void SetClearFlags(eCameraClearFlags flag) { m_ClearFlags = flag; }

		// static
		static std::vector<Camera*> GetAllCameras() { return s_Cameras; }
		static unsigned int GetAllCamerasCount() { return static_cast<unsigned int>(s_Cameras.size()); }

	private:
		DirectX::XMMATRIX computeViewMatrix();
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
		ScreenRect m_ScreenRect;
		unsigned int m_ScreenWidth;
		unsigned int m_ScreenHeight;

		DirectX::XMFLOAT3 m_Position;

		// pixel width, height : pixel 단위의 넒이, 높이

		// 불투명 오브젝트 정렬 모드?

		// CameraClearFlags
		// enum이고 Skybox, SolidColor, Depth, Nothing 으로 구성된다.
		// 선택된 대상으로 배경을 채운다.
		eCameraClearFlags m_ClearFlags;

		// Culling Mask
		// GameObject의 LayerMask와 어찌저찌 해서 그릴 대상을 추릴 수 있다.
		// Tag와 Layer는 따로 존재하며, 여기에 등록하면 GameObject에서 할당할 수 있다.
	};
}