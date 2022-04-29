#pragma once
#include "Renderable.h"
#include "Renderer/Graphics/Texture.h"

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

		SpriteMaterial* GetMaterial() { return m_pMaterial; }
		void SetMaterial(SpriteMaterial* pMaterial) { m_pMaterial = pMaterial; }
		bool HasMaterial() const { return m_pMaterial != nullptr; }

		Texture2D* GetSprite();
		void SetSprite(Texture2D* pSprite);
		bool HasSprite() const;

		DirectX::XMFLOAT4 GetColor() const;
		void SetColor(const DirectX::XMFLOAT4& color);

		bool IsFlipX() const;
		void SetFlipX(bool flip);
		bool IsFlipY() const;
		void SetFlipY(bool flip);

	private:
		bool createBuffer(const unsigned int width, const unsigned int height);

	private:
		struct VertexType
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 texCoord;
		};
		VertexBuffer* m_pVertexBuffer = nullptr;
		IndexBuffer* m_pIndexBuffer = nullptr; 

		SpriteMaterial* m_pMaterial = nullptr;
	};
}