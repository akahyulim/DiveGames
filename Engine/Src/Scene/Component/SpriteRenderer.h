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
		SpriteRenderer(GameObject* pGameObject);
		~SpriteRenderer();

		void Shutdown();

		Texture2D* GetTexture() { return m_pTexture; }
		void SetTexture(Texture2D* pTexture);

		ID3D11ShaderResourceView* GetShaderResourceView();

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		int GetVertexCount() const { return m_VertexCount; }
		int GetIdexCount() const { return m_IndexCount; }

		unsigned int GetSizeWidth() const { return m_Width; }
		unsigned int GetSizeHeight() const { return m_Height; }
		
	private:
		bool createBuffer();

	private:
		DirectX::XMFLOAT4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Texture2D* m_pTexture = nullptr;

		int m_Width = 0;
		int m_Height = 0;

		struct VertexType
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 texCoords;
		};

		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIndexBuffer = nullptr;

		// 얘네는 그냥 static const가 나을듯...
		int m_VertexCount = 6;
		int m_IndexCount = 6;
	};
}