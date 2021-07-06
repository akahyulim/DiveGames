#pragma once
#include "Component.h"
#include "Renderer.h"
#include "Font.h"
#include <string>
#include <vector>

namespace dive
{
	class GameObject;

	// �̰� 3d ���� text�̴�. gui���� ���� ������ �Ѵ�...

	// ���� Mesh Base�� ������� �ִ�.
	class TextMesh : public Component
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
		TextMesh(GameObject* owner);
		~TextMesh();

		bool SetFont(const Font* pFont);
		bool SetFont(const std::string& name);

		// positiond�� transform���� �����Ѵ�.
		// ������ z���� ������ �� �غ��� �Ѵ�.
		bool SetText(const std::wstring& text, const DirectX::XMFLOAT2& position);

		ID3D11ShaderResourceView* GetAtlas();

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer.Get(); }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer.Get();; }
		
		unsigned int GetStride() const { return sizeof(VertexType); }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indices.size()); }

		void SetLineSpacing(unsigned int size) { m_lineSpacing = size; }
		unsigned int GetLineSpacing() const { return m_lineSpacing; }

		void SetTabSize(unsigned int size) { m_tabSize = size; }
		unsigned int GetTabSize() const { return m_tabSize; }

		void SetFontSize(unsigned int size);
		unsigned int GetFontSize() const { return m_fontSize; }


	private:
		bool updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

		std::vector<VertexType> m_vertices;
		std::vector<unsigned int> m_indices;

		std::wstring m_text;

		// Ÿ���� �ٲ� �� �ִ�.
		unsigned int m_lineSpacing;
		unsigned int m_tabSize;
		unsigned int m_fontSize;

		Font* m_pFont;
	};
}