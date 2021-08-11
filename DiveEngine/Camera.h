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

	// �� ������ �Է��� �޾� Transform�� ���ѽ�Ű��
	// �̸� �̿��Ͽ� View, Projection Matrix�� ����Ѵ�.
	// �̿ܿ��� �ø�, ��ŷ ���� ����� �����Ѵ�.
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
		// �̰� ��� ����ұ...
		// ���� �� ������ ���� ����� �ʿ䰡 �ֳ�?
		// Viewport�� ������ �� Inspector���� ����ص� �� �� ������...
		// �׷� ���� Get, SetView Width, Height �Լ��� �ʿ��������...
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

		// �߰��ؾ��� �� ���� ������
		// rect : ī�޶��� ������ ������� ��Ÿ�� ȭ�鿡���� ��ġ 0 ~ 1
		// �̰� �ᱹ Viewport��.
		D3D11_VIEWPORT m_Viewport;

		// pixel width, height : pixel ������ ����, ����

		// ������ ������Ʈ ���� ���?
	};
}