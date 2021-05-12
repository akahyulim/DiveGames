#pragma once
#include "Renderer.h"
#include "Dive_Font.h"
#include <string>
#include <vector>

namespace Dive
{
	// ���� Mesh�� ������???
	// �ᱹ GameObject�� ���� Component�� �Ǵ°ǰ�?
	class TextMesh
	{
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
		TextMesh();
		~TextMesh();

		bool SetFont(const Dive_Font* pFont);

		bool SetText(const std::wstring& text, const DirectX::XMFLOAT2& position);

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer.Get(); }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer.Get();; }
		// �����Լ� ȣ���� �� �ۿ� ����.
		ID3D11ShaderResourceView* GetAtlas();

		unsigned int GetStride() const { return sizeof(VertexType); }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indices.size()); }

	private:
		bool updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

		std::vector<VertexType> m_vertices;
		std::vector<unsigned int> m_indices;

		// Font�� �ᱹ Resource�� �Ǿ�� �Ѵ�.
		Dive_Font* m_pFont = nullptr;

		// state�� ���� ������ �ϳ�? ������ �� ������ �ϳ�?

		unsigned int m_fontSize = 0;
		std::wstring m_oldText = L"";
	};
}