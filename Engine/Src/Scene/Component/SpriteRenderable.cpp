#include "divepch.h"
#include "SpriteRenderable.h"
#include "Renderer/Renderer.h"
#include "Renderer/Graphics/VertexBuffer.h"
#include "Renderer/Graphics/IndexBuffer.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
	SpriteRenderable::SpriteRenderable(GameObject* pGameObject, unsigned long long id)
		: Renderable(pGameObject, id)
	{
		// 유니티를 기준으로
		// Sprite와 Material은 별개다.
		// => 애초에 Sprite는 Texture도 아니다. 개별 class다.
		// 따라서 Sprite, Flip, Color는 전부 SpriteRenderer의,
		// Material은 Renderer의 멤버 변수이다.
		// 그리고 Sprite의 Material이 사용하는 Shader는
		// Default: 알파 블렌딩된 쉐이더, 라이팅과는 연동 X
		// Diffuse: 간이?적인 알파 블렌딩된 쉐이더, 라이팅과 연동(정면 방향 법선 벡터)
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

	void SpriteRenderable::SetSprite(Texture2D* pSprite)
	{
		if (pSprite == nullptr)
			return;

		m_pSprite = pSprite;
		
		createBuffer(m_pSprite->GetWidth(), m_pSprite->GetHeight());
	}

	bool SpriteRenderable::HasSprite() const
	{
		return m_pSprite != nullptr;
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