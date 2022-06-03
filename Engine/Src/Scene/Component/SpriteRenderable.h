#pragma once
#include "Renderable.h"
#include "Renderer/Graphics/Texture2D.h"

namespace Dive
{
	class GameObject;
	class SpriteMaterial;
	class VertexBuffer;
	class IndexBuffer;

	class SpriteRenderable : public Renderable
	{
	public:
		COMPONENT_CLASS_TYPE(SpriteRenderable)

		SpriteRenderable(GameObject* pGameObject, unsigned long long id = 0);
		~SpriteRenderable();

		void Shutdown();

		VertexBuffer* GetVertexBuffer() { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }

		Texture2D* GetSprite() { return m_pSprite; }
		void SetSprite(Texture2D* pSprite);
		bool HasSprite() const;

		DirectX::XMFLOAT4 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

		bool IsFlipX() const { return m_bFlipX; }
		void SetFlipX(bool flip) { m_bFlipX = flip; }
		bool IsFlipY() const { return m_bFlipY; }
		void SetFlipY(bool flip) { m_bFlipY = flip; }

	private:
		bool createBuffer(const unsigned int width, const unsigned int height);

	private:
		struct VertexType
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 texCoord;
		};
		VertexBuffer* m_pVertexBuffer	= nullptr;
		IndexBuffer* m_pIndexBuffer		= nullptr; 

		Texture2D* m_pSprite			= nullptr;
		DirectX::XMFLOAT4 m_Color		= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		bool m_bFlipX					= false;
		bool m_bFlipY					= false;
	};
}