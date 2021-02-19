#include "DivePch.h"
#include "Legacy.h"
#include "Core/Log.h"
#include "D3D11/Texture2D.h"
#include "D3D11/ConstantBuffer.h"


namespace Dive
{
	Legacy::Legacy(Context * context)
		: Material(context),
		m_albedoTexture(nullptr),
		m_albedoColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		m_tiling(DirectX::XMFLOAT2(0.0f, 0.0f)),
		m_offset(DirectX::XMFLOAT2(0.0f, 0.0f))
	{
		m_renderingMode = eRenderingMode::Opaque;
	}

	Legacy::~Legacy()
	{
	}

	// 텍스쳐는 constant buffer가 아니라 다른 방법으로 전달한다.
	ConstantBuffer * Legacy::GetConstantBuffer()
	{
		if (!m_constantBuffer)
		{
			m_constantBuffer = new ConstantBuffer(m_context);
			if (!m_constantBuffer->Create<LegacyBuffer>())
			{
				return nullptr;
			}
		}

		// map & unmap
		if(m_bDirty)
		{
			auto ptr = static_cast<LegacyBuffer*>(m_constantBuffer->Map());
			if (!ptr)
			{
				CORE_ERROR("");
				return nullptr;
			}

			ptr->albedoColor	= m_albedoColor;
			ptr->tiling			= m_tiling;
			ptr->offset			= m_offset;

			if (!m_constantBuffer->Unmap())
			{
				return nullptr;
			}

			m_bDirty = false;
		}

		return m_constantBuffer;
	}

	void Legacy::SetRenderingMode(eRenderingMode mode)
	{
		if (m_renderingMode == mode)
			return;

		m_renderingMode = mode;
		m_bDirty		= true;
	}

	void Legacy::SetAlbedoTexture(Texture2D * texture)
	{
		if (!texture)
		{
			return;
		}

		if (!m_albedoTexture)
		{
			if (m_albedoTexture->GetID() == texture->GetID())
			{
				return;
			}
		}

		m_albedoTexture = texture;
		m_bDirty		= true;
	}

	void Legacy::SetAlbedoColor(DirectX::XMFLOAT4 color)
	{
		if ((m_albedoColor.x == color.x) && (m_albedoColor.y == color.y) && (m_albedoColor.z == color.z) && (m_albedoColor.w == color.w))
			return;

		m_albedoColor	= color;
		m_bDirty		= true;
	}
	
	void Legacy::SetTiling(DirectX::XMFLOAT2 tiling)
	{
		if ((m_tiling.x == tiling.x) && (m_tiling.y == m_tiling.y))
			return;

		m_tiling = tiling;
		m_bDirty = true;
	}

	void Legacy::SetOffset(DirectX::XMFLOAT2 offset)
	{
		if ((m_offset.x == offset.x) && (m_offset.y == m_offset.y))
			return;

		m_offset = offset;
		m_bDirty = true;
	}
}