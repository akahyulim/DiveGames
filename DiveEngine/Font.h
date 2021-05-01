#pragma once
#include "GraphicsInclude.h"
#include "Texture.h"
#include <string>
#include <memory>


namespace Dive
{
	// 나중엔 리소스로 변경시켜야 한다.
	// 결과적으로보면 Atlas와 Glyph만 제공하게 되는건가?
	class Font
	{
		struct Glyph
		{
			float left, right;
			int size;
		};

		struct VertexType
		{
			VertexType(float x, float y, float z, float u, float v)
			{
				position = DirectX::XMFLOAT3(x, y, z);
				tex = DirectX::XMFLOAT2(u, v);
			}

			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 tex;
		};

	public:
		Font() = default;
		~Font() = default;

		bool Initialize(const std::wstring& fontFileName, const std::wstring& fontAtlasName);
		void Destroy();

		void SetText(const std::string& text, const DirectX::XMFLOAT2& position);

		ID3D11ShaderResourceView* GetFontAtlas() { return m_pFontAtlas; }

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer.Get(); }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer.Get(); }

		unsigned int GetStride() const { return sizeof(VertexType); }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indices.size()); }

	private:
		bool loadFontData(const std::wstring& filename);
		bool loadFontAtlasName(const std::wstring& filename);
		bool updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices);

	private:
		Glyph* m_pFontData = nullptr;

		ID3D11ShaderResourceView* m_pFontAtlas = nullptr;

		// 버퍼는 Mesh로 옮기는 게 맞다.
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

		std::vector<VertexType> m_vertices;
		std::vector<unsigned int> m_indices;

		std::string m_currentText;
	};
}