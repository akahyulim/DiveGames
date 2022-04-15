#include "divepch.h"
#include "SpriteRenderable.h"
#include "Renderer/Renderer.h"
#include "Renderer/SpriteMaterial.h"
#include "Renderer/Graphics/VertexBuffer.h"
#include "Renderer/Graphics/IndexBuffer.h"

namespace Dive
{
	SpriteRenderable::SpriteRenderable(GameObject* pGameObject)
		: Renderable(pGameObject)
	{
		// 老窜 流立 积己茄促.
		m_pMaterial = new SpriteMaterial;
	}

	SpriteRenderable::~SpriteRenderable()
	{
		Shutdown();
	}
	
	void SpriteRenderable::Shutdown()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);
	}

	Texture2D* SpriteRenderable::GetSprite()
	{
		DV_ASSERT(m_pMaterial != nullptr);

		if(!m_pMaterial->HasSprite())
			return nullptr;

		return m_pMaterial->GetSprite();
	}

	void SpriteRenderable::SetSprite(Texture2D* pSprite)
	{
		DV_ASSERT(m_pMaterial != nullptr);

		if (pSprite == nullptr)
			return;

		m_pMaterial->SetSprite(pSprite);

		createBuffer(pSprite->GetWidth(), pSprite->GetHeight());
	}

	bool SpriteRenderable::HasSprite() const
	{
		DV_ASSERT(m_pMaterial != nullptr);

		return m_pMaterial->HasSprite();
	}

	DirectX::XMFLOAT4 SpriteRenderable::GetColor() const
	{
		DV_ASSERT(m_pMaterial != nullptr);

		return m_pMaterial->GetColor();
	}

	void SpriteRenderable::SetColor(const DirectX::XMFLOAT4& color)
	{
		DV_ASSERT(m_pMaterial != nullptr);

		m_pMaterial->SetColor(color);
	}

	bool SpriteRenderable::IsFlipX() const
	{
		DV_ASSERT(m_pMaterial != nullptr);

		return m_pMaterial->IsFlipX();
	}

	void SpriteRenderable::SetFlipX(bool flip)
	{
		DV_ASSERT(m_pMaterial != nullptr);

		m_pMaterial->SetFlipX(flip);
	}

	bool SpriteRenderable::IsFlipY() const
	{
		DV_ASSERT(m_pMaterial != nullptr);

		return m_pMaterial->IsFlipY();
	}

	void SpriteRenderable::SetFlipY(bool flip)
	{
		DV_ASSERT(m_pMaterial != nullptr);

		m_pMaterial->SetFlipY(flip);
	}

	bool SpriteRenderable::createBuffer(const unsigned int width, const unsigned int height)
	{
		Shutdown();

		float left		= static_cast<float>(width / 2) * -1.0f;
		float right		= left + static_cast<float>(width);
		float top		= static_cast<float>(height / 2);
		float bottom	= top - static_cast<float>(height);

		// vertex buffer
		{
			std::vector<VertexType> vertices(6);

			vertices[0].position = { left, top, 0.0f };			vertices[0].texCoord = { 0.0f, 0.0f };
			vertices[1].position = { right, bottom, 0.0f };		vertices[1].texCoord = { 1.0f, 1.0f };
			vertices[2].position = { left, bottom, 0.0f };		vertices[2].texCoord = { 0.0f, 1.0f };

			vertices[3].position = { left, top, 0.0f };			vertices[3].texCoord = { 0.0f, 0.0f };
			vertices[4].position = { right, top, 0.0f };		vertices[4].texCoord = { 1.0f, 0.0f };
			vertices[5].position = { right, bottom, 0.0f };		vertices[5].texCoord = { 1.0f, 1.0f };

			m_pVertexBuffer = new VertexBuffer;
			if (!m_pVertexBuffer->Create<VertexType>(vertices))
				return false;
		}

		// index buffer
		{
			std::vector<unsigned short> indices(6);

			for (unsigned int i = 0; i < 6; i++)
				indices[i] = i;

			m_pIndexBuffer = new IndexBuffer;
			if (!m_pIndexBuffer->Create<unsigned short>(indices))
				return false;
		}

		return true;
	}


}