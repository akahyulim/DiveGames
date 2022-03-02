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

		if (!createBuffer())
			Shutdown();
	}

	ID3D11ShaderResourceView* SpriteRenderer::GetShaderResourceView()
	{
		if(!m_pTexture)
			return nullptr;

		return m_pTexture->GetShaderResourceView();
	}

	// 일단 직접 MAP / UNMAP
	void SpriteRenderer::SetColor(DirectX::XMFLOAT4 color)
	{
		if (m_Color.x == color.x && m_Color.y == color.y && m_Color.z == color.z && m_Color.w == color.w)
			return;
		m_Color = color;

		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		pImmediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		VertexType* pVertex = static_cast<VertexType*>(mappedResource.pData);

		float left = static_cast<float>((m_Width / 2) * -1);
		float right = left + static_cast<float>(m_Width);
		float top = static_cast<float>((m_Height / 2));
		float bottom = top - static_cast<float>(m_Height);

		// top left
		pVertex[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
		pVertex[0].color = m_Color;
		pVertex[0].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
		// bottom right
		pVertex[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
		pVertex[1].color = m_Color;
		pVertex[1].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
		// bottom left
		pVertex[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
		pVertex[2].color = m_Color;
		pVertex[2].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);

		// top left
		pVertex[3].position = DirectX::XMFLOAT3(left, top, 0.0f);
		pVertex[3].color = m_Color;
		pVertex[3].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
		// top right
		pVertex[4].position = DirectX::XMFLOAT3(right, top, 0.0f);
		pVertex[4].color = m_Color;
		pVertex[4].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
		// bottom right
		pVertex[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
		pVertex[5].color = m_Color;
		pVertex[5].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);

		pImmediateContext->Unmap(m_pVertexBuffer, 0);
	}

	bool SpriteRenderer::createBuffer()
	{
		// vertex buffer
		{
			std::vector<VertexType> vertices;
			vertices.resize(m_VertexCount);

			float left = static_cast<float>((m_Width / 2) * -1);
			float right = left + static_cast<float>(m_Width);
			float top = static_cast<float>((m_Height / 2));
			float bottom = top - static_cast<float>(m_Height);

			// top left
			vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);
			vertices[0].color = m_Color;
			vertices[0].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
			// bottom right
			vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
			vertices[1].color = m_Color;
			vertices[1].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);
			// bottom left
			vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);
			vertices[2].color = m_Color;
			vertices[2].texCoord = DirectX::XMFLOAT2(0.0f, 1.0f);

			// top left
			vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);
			vertices[3].color = m_Color;
			vertices[3].texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
			// top right
			vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);
			vertices[4].color = m_Color;
			vertices[4].texCoord = DirectX::XMFLOAT2(1.0f, 0.0f);
			// bottom right
			vertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);
			vertices[5].color = m_Color;
			vertices[5].texCoord = DirectX::XMFLOAT2(1.0f, 1.0f);

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(VertexType) * m_VertexCount;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = vertices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			auto& graphicsDevice = Renderer::GetGraphicsDevice();
			if (!graphicsDevice.CreateBuffer(&desc, &data, &m_pVertexBuffer))
			{
				vertices.clear();
				return false;
			}

			vertices.clear();
		}

		// index buffer
		{
			std::vector<unsigned int> indices;
			for (int i = 0; i != m_IndexCount; i++)
			{
				indices.emplace_back(i);
			}

			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = indices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			auto& graphicsDevice = Renderer::GetGraphicsDevice();
			if (!graphicsDevice.CreateBuffer(&desc, &data, &m_pIndexBuffer))
			{
				DV_RELEASE(m_pVertexBuffer);
				indices.clear();
				return false;
			}

			indices.clear();
		}

		return true;
	}
}