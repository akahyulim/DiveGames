#include "divepch.h"
#include "SpriteRenderer.h"

namespace Dive
{
	SpriteRenderer::~SpriteRenderer()
	{
		Shutdown();
	}
	
	void SpriteRenderer::Shutdown()
	{
		DV_RELEASE(m_pIndexBuffer);
		DV_RELEASE(m_pVertexBuffer);

		DV_DELETE(m_pTexture);

		m_Width = 0;
		m_Height = 0;

		m_VertexCount = 0;
		m_IndexCount = 0;
	}

	void SpriteRenderer::SetTexture(Texture2D* pTexture)
	{
		if (!pTexture)
			return;

		Shutdown();

		m_Width = pTexture->GetWidth();
		m_Height = pTexture->GetHeight();

		createBuffer();
	}

	ID3D11ShaderResourceView* SpriteRenderer::GetShaderResourceView()
	{
		if(!m_pTexture)
			return nullptr;

		return m_pTexture->GetShaderResourceView();
	}

	void SpriteRenderer::createBuffer()
	{
		// 1. GraphicsDevice를 어떻게 획득할 것인가?
		// => Dive API는 외부에서 사용하는 게 맞는 것 같다.
		// 2. 버퍼를 직접 생성하는 것이 맞는가?
		// => Begin / End 처럼 Renderer에서 처리하고 싶은데...

		// vertex buffer
		{
			auto pVertices = new VertexType[m_VertexCount];
			DV_ASSERT(pVertices);
			memcpy(pVertices, 0, sizeof(VertexType) * m_VertexCount);

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(VertexType) * m_VertexCount;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pVertices;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			
		}

		// index buffer
		{
			D3D11_BUFFER_DESC desc;

			D3D11_SUBRESOURCE_DATA data;
		}
	}
}