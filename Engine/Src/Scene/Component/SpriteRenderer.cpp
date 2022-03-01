#include "divepch.h"
#include "SpriteRenderer.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	SpriteRenderer::SpriteRenderer(GameObject* pGameObject)
		: RendererComponent(pGameObject)
	{
		m_Type = eComponentType::SpriteRenderer;
	}

	SpriteRenderer::~SpriteRenderer()
	{
		Shutdown();
	}
	
	void SpriteRenderer::Shutdown()
	{
		DV_RELEASE(m_pIndexBuffer);
		DV_RELEASE(m_pVertexBuffer);

		m_pTexture = nullptr;

		m_Width = 0;
		m_Height = 0;
	}

	void SpriteRenderer::SetTexture(Texture2D* pTexture)
	{
		if (!pTexture)
			return;

		Shutdown();

		m_pTexture	= pTexture;
		m_Width		= static_cast<int>(pTexture->GetWidth());
		m_Height	= static_cast<int>(pTexture->GetHeight());

		if (createBuffer())
			Shutdown();
	}

	ID3D11ShaderResourceView* SpriteRenderer::GetShaderResourceView()
	{
		if(!m_pTexture)
			return nullptr;

		return m_pTexture->GetShaderResourceView();
	}

	bool SpriteRenderer::createBuffer()
	{
		// vertex buffer
		{
			// 굳이 동적생성할 필요도 없을 것 같다.
			auto pVertices = new VertexType[m_VertexCount];
			DV_ASSERT(pVertices);

			float left = static_cast<float>((m_Width / 2) * -1);
			float right = left + static_cast<float>(m_Width);
			float top = static_cast<float>((m_Height / 2));
			float bottom = top - static_cast<float>(m_Height);

			// top left
			pVertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
			pVertices[0].texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
			// bottom right
			pVertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
			pVertices[1].texCoords = DirectX::XMFLOAT2(1.0f, 1.0f);
			// bottom left
			pVertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
			pVertices[2].texCoords = DirectX::XMFLOAT2(0.0f, 1.0f);
			
			// top left
			pVertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);
			pVertices[3].texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
			// top right
			pVertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);
			pVertices[4].texCoords = DirectX::XMFLOAT2(1.0f, 0.0f);
			// bottom right
			pVertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
			pVertices[5].texCoords = DirectX::XMFLOAT2(1.0f, 1.0f);

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT; ;// D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(VertexType) * m_VertexCount;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pVertices;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			auto& graphicsDevice = Renderer::GetGraphicsDevice();
			if (!graphicsDevice.CreateBuffer(&desc, &data, &m_pVertexBuffer))
			{
				DV_DELETE_ARRAY(pVertices);
				return false;
			}

			DV_DELETE_ARRAY(pVertices);
		}

		// index buffer
		{
			auto pIndices = new unsigned long[m_IndexCount];
			DV_ASSERT(pIndices);

			for (int i = 0; i < m_IndexCount; i++)
			{
				pIndices[i] = i;
			}

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pIndices;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			auto& graphicsDevice = Renderer::GetGraphicsDevice();
			if (!graphicsDevice.CreateBuffer(&desc, &data, &m_pIndexBuffer))
			{
				DV_RELEASE(m_pVertexBuffer);
				DV_DELETE_ARRAY(pIndices);
				return false;
			}

			DV_DELETE_ARRAY(pIndices);
		}

		return true;
	}
}