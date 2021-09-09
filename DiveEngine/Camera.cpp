#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "DiveCore.h"
#include "FileStream.h"
#include "Src/Renderer/Graphics/dvCubemap.h"

/*
* ClearFlags에 맞춰 RenderTargetView, DepthStencilView를 Clear 여부를 적용한다.
* 계산한 Viewport를 리턴한다.
* ConstantBuffer용 값들을 리턴한다.
*/
namespace dive
{
	std::vector<Camera*> Camera::s_Cameras;

	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_Type = eComponentType::Camera;

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

		// 외부에서 생성된 텍스쳐를 가져오는 것 같다.
		// 좀 더 알아봐야겠지?
		// 이걸 굳이 가져오는 이유는 직접 Clear하기 위함이 아닐까?
		// 아 물론 직관적으로 그려질 대상을 보여주는 것도 이유가 될 수 있다.
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

		m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		m_ClearFlags = eCameraClearFlags::SolidColor;
		
		s_Cameras.emplace_back(this);
	}
	
	Camera::~Camera()
	{
	}

	//======================================================================//
	// 현재 오버헤드를 작정하고 매 프레임 업데이트 중이다.					//
	// 이를 해소하려면 GameObject 내부 단위의 이벤트 시스템이 필요하다.		//
	// 그것도 아니라면 스파르탄처럼 위치, 회전값을 매 프레임 비교해야 한다.	//
	//======================================================================//
	void Camera::Update(float deltaTime)
	{
		//if (!m_bUpdated)
		//	return;

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

	// ClearCamera 혹은 BeginScene이 적당할 거 같다.
	// 아니면 Renderer::BegineScene()에서 Camera::ClearScene() 정도가 좋으려나...?
	void Camera::ClearRenderTarget()
	{
		// BackBuffer 혹은 GBuffer의 RenderTarget들을 Clear할 수 있어야 한다.
		// 개별 RenderTarget을 가진다면 이때 DepthStencilView도 함께 생성되었겠지?
		// 직접 Clear하려면 immediateContext가 필요하다.

		switch (m_ClearFlags)
		{
		case eCameraClearFlags::Skybox:
		{
			// 이건 아직 잘 모르겠다.
			break;
		}
		case eCameraClearFlags::SolidColor:
		{
			// 이건 아마 RenderTargetView, DepthStencilView 둘을 Clear할 거다.
			break;
		}
		case eCameraClearFlags::Depth:
		{
			// 이건 아마 DepthStencilView만 Clear할 거다.
			break;
		}
		case eCameraClearFlags::Nothing:
		{
			// 이건 RenderTargetView, DepthStencilView 둘 다 Clear하지 않는거다.
			break;
		}
		default:
			return;
		}
	}

	void Camera::SetProjectionType(eProjectionType type)
	{
		if (m_ProjectionType == type)
			return;

		m_ProjectionType = type;
		m_bUpdated = true;
	}

	// 0 ~ 180도 사이. 기본은 60
	// 그런데 0도는 또 안되는 것 같다.
	void Camera::SetFieldOfView(float angle)
	{
		if (angle <= 0.0f || angle > 180.0f)
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


	// 이건 Viewport의 Width와 같은 걸까?
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

	DirectX::XMMATRIX Camera::computeViewMatrix()
	{
		auto transform = GetGameObject()->GetTransform();
		
		auto pos = transform->GetPositionVector();
		auto up = transform->UpVector();
		auto lookAt = transform->ForwardVector();
		lookAt += pos;

		m_Position = transform->GetPosition();

		return DirectX::XMMatrixLookAtLH(pos, lookAt, up);
	}

	DirectX::XMMATRIX Camera::computeProjectionMatrix() const
	{
		if (m_ProjectionType == eProjectionType::Perspective)
		{
			return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_FieldOfView), GetAspect(), m_NearPlane, m_FarPlane);
		}
		else if (m_ProjectionType == eProjectionType::Orthographic)
		{
			// 일단 임시로 ScreenWidth, Height를 넣었다.
			return DirectX::XMMatrixOrthographicLH((float)m_ScreenWidth, (float)m_ScreenHeight, m_NearPlane, m_FarPlane);
		}

		CORE_ERROR("");
		return DirectX::XMMatrixIdentity();
	}
}