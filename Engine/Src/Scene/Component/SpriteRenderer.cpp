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
		// 1. GraphicsDevice�� ��� ȹ���� ���ΰ�?
		// => Dive API�� �ܺο��� ����ϴ� �� �´� �� ����.
		// 2. ���۸� ���� �����ϴ� ���� �´°�?
		// => Begin / End ó�� Renderer���� ó���ϰ� ������...

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