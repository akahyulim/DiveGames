#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "DiveCore.h"
#include "FileStream.h"
#include "dvRenderTexture.h"

// View Width, View Height�� ���� �𸣰ڴ�.
// => Orthographic���� ����ϸ� �ػ� ����.
// => ���� �ٸ� ������ ������� �ʴ´ٸ� ���� ��� ������ ������ �ʿ䰡 ����. �׳� ���� �������� �ȴ�.
// Viewport Rect�� ��� �۵��ϴ��� �𸣰ڴ�.
// => TargetTexture �κ��� ���� �⺻������ ��ũ���� �׸��ٰ� �Ѵ�.
// => ���������� TargetTexture ���� ���� �� ȭ�鿡 ī�޶� �������� �����ϰ� �ִ�.
// ViewMatrix ����� ����� �Ȱ��� �𸣰ڴ�.(Forward, Up�� Transform���κ��� �����Դµ�, ����� ��ķ� �ߴ�.)
namespace dive
{
	std::vector<Camera*> Camera::s_Cameras;

	Camera::Camera(GameObject* pGameObject)
		: Component(typeid(Camera).hash_code(), pGameObject)
	{
		m_ProjectionType = eProjectionType::Perspective;

		m_FieldOfView = 60.0f;
		m_ViewWidth = 0.0f;
		m_ViewHeight = 0.0f;
		m_NearPlane = 0.3f;
		m_FarPlane = 1000.0f;

		m_bUpdated = false;

		m_BackgroundColor = DirectX::XMFLOAT4(0.396f, 0.611f, 0.937f, 1.0f);
		DirectX::XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());

		// �ܺο��� ������ �ؽ��ĸ� �������� �� ����.
		// �� �� �˾ƺ��߰���?
		m_pTargetTexture = nullptr;

		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = 400.0f;
		m_Viewport.Height = 300.0f;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScreenRect.x = 0.0f;
		m_ScreenRect.y = 0.0f;
		m_ScreenRect.width= 1.0f;
		m_ScreenRect.height = 1.0f;

		s_Cameras.emplace_back(this);
	}
	
	Camera::~Camera()
	{
	}

	void Camera::Update(float deltaTime)
	{
		// viewport�� position, rotation���ε� updated�� Ȯ���Ѵ�...

		if (!m_bUpdated)
			return;

		m_Viewport.TopLeftX = m_ScreenRect.x * static_cast<float>(m_ScreenWidth);
		m_Viewport.TopLeftY = m_ScreenRect.y * static_cast<float>(m_ScreenHeight);
		m_Viewport.Width = m_ScreenRect.width * static_cast<float>(m_ScreenWidth);
		m_Viewport.Height = m_ScreenRect.height * static_cast<float>(m_ScreenHeight);

		DirectX::XMStoreFloat4x4(&m_View, computeViewMatrix());
		DirectX::XMStoreFloat4x4(&m_Projection, computeProjectionMatrix());
		// frustum??

		m_bUpdated = false;
	}

	void Camera::Serialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);

		pFileStream->Write(static_cast<int>(m_ProjectionType));
		pFileStream->Write(m_FieldOfView);
		pFileStream->Write(m_ViewWidth);
		pFileStream->Write(m_ViewHeight);
		pFileStream->Write(m_NearPlane);
		pFileStream->Write(m_FarPlane);
		pFileStream->Write(m_BackgroundColor);
	}

	void Camera::Deserialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);
		
		int projectionType = 0;
		pFileStream->Read(&projectionType);
		m_ProjectionType = static_cast<eProjectionType>(projectionType);
		pFileStream->Read(&m_FieldOfView);
		pFileStream->Read(&m_ViewWidth);
		pFileStream->Read(&m_ViewHeight);
		pFileStream->Read(&m_NearPlane);
		pFileStream->Read(&m_FarPlane);
		pFileStream->Read(&m_BackgroundColor);
	}

	void Camera::SetProjectionType(eProjectionType type)
	{
		if (m_ProjectionType == type)
			return;

		m_ProjectionType = type;
		m_bUpdated = true;
	}

	// 0 ~ 180�� ����. �⺻�� 60
	void Camera::SetFieldOfView(float angle)
	{
		if (angle < 0.0f || angle > 180.0f)
			return;

		if (m_FieldOfView == angle)
			return;

		m_FieldOfView = angle;
		m_bUpdated = true;
	}

	void Camera::SetScreenSize(unsigned int width, unsigned int height)
	{
		if (m_ScreenWidth == width && m_ScreenHeight == height)
			return;

		if (width <= 0 || height <= 0)
		{
			return;
		}

		m_ScreenWidth = width;
		m_ScreenHeight = height;

		m_bUpdated = true;
	}

	void Camera::SetScreenRect(float x, float y, float width, float height)
	{
		if (m_ScreenRect.x == x && m_ScreenRect.y == y && m_ScreenRect.width == width && m_ScreenRect.height == height)
			return;

		if (x < 0.0f || x > 1.0f || y < 0.0f || y > 1.0f || width < 0.0f || width > 1.0f || height < 0.0f || height > 1.0f)
		{
			return;
		}

		if (x >= width || y >= height)
		{
			return;
		}

		m_ScreenRect.x = x;
		m_ScreenRect.y = y;
		m_ScreenRect.width = width;
		m_ScreenRect.height = height;

		m_bUpdated = true;
	}


	// �̰� Viewport�� Width�� ���� �ɱ�?
	void Camera::SetViewWidth(float width)
	{
		if (m_ViewWidth == width)
			return;

		m_ViewWidth = width;
		m_bUpdated = true;
	}

	void Camera::SetViewHeight(float height)
	{
		if (m_ViewHeight == height)
			return;

		m_ViewHeight = height;
		m_bUpdated = true;
	}

	void Camera::SetNearPlane(float nearPlane)
	{
		if (nearPlane < 0.1f)
			return;

		if (m_NearPlane == nearPlane)
			return;

		m_NearPlane = nearPlane;
		m_bUpdated = true;
	}

	void Camera::SetFarPlane(float farPlane)
	{
		if (farPlane <= m_NearPlane)
			return;

		if (m_FarPlane == farPlane)
			return;

		m_FarPlane = farPlane;
		m_bUpdated = true;
	}

	void Camera::SetBackgroundColor(const DirectX::XMFLOAT4& color)
	{
		if (DirectX::XMVector4Equal(DirectX::XMLoadFloat4(&m_BackgroundColor), DirectX::XMLoadFloat4(&color)))
			return;

		m_BackgroundColor = color;
	}

	DirectX::XMFLOAT4X4 Camera::GetViewProjection() const
	{
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMStoreFloat4x4(&viewProj, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m_View), DirectX::XMLoadFloat4x4(&m_Projection)));

		return viewProj;
	}

	DirectX::XMMATRIX Camera::computeViewMatrix() const
	{
		auto transform = GetGameObject()->GetTransform();
		
		auto pos = transform->GetPositionVector();
		auto up = transform->UpVector();
		auto lookAt = transform->ForwardVector();
		lookAt += pos;

		return DirectX::XMMatrixLookAtLH(pos, lookAt, up);
	}

	DirectX::XMMATRIX Camera::computeProjectionMatrix() const
	{
		if (m_ProjectionType == eProjectionType::Perspective)
		{
			return DirectX::XMMatrixPerspectiveFovLH(m_FieldOfView, GetAspect(), m_NearPlane, m_FarPlane);
		}
		else if (m_ProjectionType == eProjectionType::Orthographic)
		{
			return DirectX::XMMatrixOrthographicLH(m_ViewWidth, m_ViewHeight, m_NearPlane, m_FarPlane);
		}

		CORE_ERROR("");
		return DirectX::XMMatrixIdentity();
	}
}