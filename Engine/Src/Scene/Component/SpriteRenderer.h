#pragma once
#include "Renderer.h"
#include "Renderer/Graphics/Texture.h"

namespace Dive
{
	class GameObject;
	class Texture2D;

	class SpriteRenderer : public RendererComponent
	{
	public:
		SpriteRenderer(GameObject* pGameObject) : RendererComponent(pGameObject) {}
		~SpriteRenderer();

		void Shutdown();

		Texture2D* GetTexture() { return m_pTexture; }
		void SetTexture(Texture2D* pTexture);

		ID3D11ShaderResourceView* GetShaderResourceView();

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		int GetVertexCount() const { return m_VertexCount; }
		int GetIdexCount() const { return m_IndexCount; }

	private:
		void createBuffer();

	private:
		DirectX::XMFLOAT4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Texture2D* m_pTexture = nullptr;

		int m_Width = 0;
		int m_Height = 0;

		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIndexBuffer = nullptr;
		int m_VertexCount = 6;
		int m_IndexCount = 6;

		struct VertexType
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 texCoord;
		};


	};
}