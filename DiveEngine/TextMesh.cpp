#include "TextMesh.h"

namespace Dive
{
	TextMesh::TextMesh()
	{
		// ��� ���� �ʱ�ȭ

		// Resource Manager �����κ��� Default Font�� ���´�.
		// Font��ü�� Renderer ��� Face���� �����ȴ�.
		// �׸��� �ش� Font�κ��� Size ������ �����Ѵ�.
	}

	TextMesh::~TextMesh()
	{
	}
	
	bool TextMesh::SetFont(const Dive_Font* pFont)
	{
		assert(pFont != nullptr);

		if (m_pFont != pFont)
			m_pFont = const_cast<Dive_Font*>(pFont);

		return true;
	}

	// �ᱹ ascii�� unicode�� ������� �Ѵ�.
	bool TextMesh::SetText(const std::wstring& text, const DirectX::XMFLOAT2& position)
	{
		if (m_oldText == text)
			return false;

		m_vertices.shrink_to_fit();
		m_oldText = text;

		DirectX::XMFLOAT2 pen = position;

		for (unsigned int i = 0; i != text.size(); i++)
		{
			auto index = text[i];
			auto letter = m_pFont->GetGlyph(index);

			// �����ε� ���� �ٸ� �� �ִ�.
			if (index == ' ')
			{
				// ���� ������ ���� ����ȭ�ؾ� �Ѵ�.
				pen.x += 10.0f;
			}
			else
			{
				m_vertices.emplace_back(pen.x,					pen.y,					0.0f, letter.uvLeft,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.width,	pen.y - letter.height,	0.0f, letter.uvRight,	letter.uvBottom);
				m_vertices.emplace_back(pen.x,					pen.y - letter.height,	0.0f, letter.uvLeft,	letter.uvBottom);

				m_vertices.emplace_back(pen.x,					pen.y,					0.0f, letter.uvLeft,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.width,	pen.y,					0.0f, letter.uvRight,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.width,	pen.y - letter.height,	0.0f, letter.uvRight,	letter.uvBottom);

				pen.x += letter.width + 1.0f;	// 1.0f�� ����??? ��� ���� ���� ���� ����.
			}
		}

		for (int i = 0; i != (int)m_vertices.size(); i++)
		{
			m_indices.emplace_back(i);
		}

		auto result = updateBuffers(m_vertices, m_indices);
		assert(result);

		return true;
	}

	ID3D11ShaderResourceView* TextMesh::GetAtlas()
	{
		assert(m_pFont);

		return m_pFont->GetAtlas()->GetShaderResourceView();
	}

	bool TextMesh::updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices)
	{
		auto pDev = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDev != nullptr);

		HRESULT hResult = E_FAIL;

		// create vertex buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = (unsigned int)(sizeof(VertexType) * vertices.size());
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = sizeof(VertexType);

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = vertices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			hResult = pDev->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		// create index buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = (unsigned int)(sizeof(unsigned int) * indices.size());
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = indices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			hResult = pDev->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		return true;
	}
}