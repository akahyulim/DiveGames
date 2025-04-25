#include "stdafx.h"
#include "Camera.h"
#include "core/CoreDefs.h"
#include "graphics/RenderTexture.h"
#include "graphics/Graphics.h"

namespace Dive
{
	void Camera::SetPerspective(float fov, float nearClip, float farClip)
	{
		m_ProjectionType = eProjectionType::Perspective;
		m_FieldOfView = fov;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		calculateProjection();
	}

	void Camera::SetOrthographic(float nearClip, float farClip)
	{
		m_ProjectionType = eProjectionType::Orthographic;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		calculateProjection();
	}

	void Camera::SetViewportSize(UINT32 width, UINT32 height)
	{
		DV_ASSERT(Camera, width > 0 && height > 0);
		m_AspectRatio = (float)width / (float)height;
		calculateProjection();
	}
	
	void Camera::calculateProjection()
	{
		if (m_ProjectionType == eProjectionType::Perspective)
		{
			DirectX::XMStoreFloat4x4(
				&m_Projection, 
				DirectX::XMMatrixPerspectiveFovLH(
					DirectX::XMConvertToRadians(m_FieldOfView), 
					m_AspectRatio, 
					m_NearClip, 
					m_FarClip
				));
		}
		else
		{
			float width = m_RenderTexture ? (float)m_RenderTexture->GetWidth() : (float)Graphics::GetResolutionWidth();
			float height = m_RenderTexture ? (float)m_RenderTexture->GetHeight() : (float)Graphics::GetResolutionHeight();

			DirectX::XMStoreFloat4x4(
				&m_Projection,
				DirectX::XMMatrixOrthographicLH(
					width,
					height,
					m_NearClip,
					m_FarClip
				));
		}
	}
}