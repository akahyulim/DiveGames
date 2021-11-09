#include "TextMesh.h"

namespace DiveEngine
{
	TextMesh::TextMesh(GameObject* gameObject)
		: Component(gameObject)
	{
		m_lineSpacing	= 1;
		m_tabSize		= 4;
		m_fontSize		= 9;

		m_pFont = nullptr;
	}

	TextMesh::~TextMesh()
	{
	}
	
	bool TextMesh::SetFont(const Font* pFont)
	{
		assert(pFont != nullptr);

		if (m_pFont != pFont)
			m_pFont = const_cast<Font*>(pFont);

		return true;
	}

	bool TextMesh::SetFont(const std::string& name)
	{
		// ��Ʈ ���ҽ��� �ش� �̸��� �ִ��� Ȯ���Ѵ�.
		// ���ٸ� ���ҽ� �������� �����Ѵ�.
		// ���ٸ� �⺻ �ý��� �������� �����Ѵ�.
		// ���ٸ� false

		return false;
	}

	// �ᱹ ascii�� unicode�� ������� �Ѵ�.
	bool TextMesh::SetText(const std::wstring& text, const DirectX::XMFLOAT2& position)
	{
		if (m_text == text)
			return false;

		m_vertices.shrink_to_fit();
		m_text = text;

		// Transform���κ��� �������ų�
		// 0, 0���� ������ �� Transform���� �̿��� ��ȯ�ؾ� �Ѵ�.
		DirectX::XMFLOAT2 pen = position;

		for (unsigned int i = 0; i != m_text.size(); i++)
		{
			auto index = m_text[i];
			auto letter = m_pFont->GetGlyph(index);

			// ��, �ٹٲ�, �����̽��� ���� ���� �����ؾ� �Ѵ�.
			// �׸��� latain�� local ���� ������ �ʿ��� �� �ִ�.
			// �����ε� ���� �ٸ� �� �ִ�.
			if (index == ' ')
			{
				// ���� ������ ���� ����ȭ�ؾ� �Ѵ�.
				pen.x += 10.0f;
			}
			else
			{
				m_vertices.emplace_back(pen.x + letter.offsetX,					pen.y + letter.offsetY,					0.0f, letter.uvLeft,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.offsetX + letter.width,	pen.y + letter.offsetY - letter.height,	0.0f, letter.uvRight,	letter.uvBottom);
				m_vertices.emplace_back(pen.x + letter.offsetX,					pen.y + letter.offsetY - letter.height,	0.0f, letter.uvLeft,	letter.uvBottom);

				m_vertices.emplace_back(pen.x + letter.offsetX,					pen.y + letter.offsetY,					0.0f, letter.uvLeft,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.offsetX + letter.width,	pen.y + letter.offsetY,					0.0f, letter.uvRight,	letter.uvTop);
				m_vertices.emplace_back(pen.x + letter.offsetX + letter.width,	pen.y + letter.offsetY - letter.height,	0.0f, letter.uvRight,	letter.uvBottom);

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

	void TextMesh::SetFontSize(unsigned int size)
	{
		// ��� size���� Atlas�� �����ϴ� �͵� ���ٴ�.
		// �� �ٸ� ����� ������?
	}

	bool TextMesh::updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices)
	{
		auto pDev = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDev != nullptr);

		HRESULT hResult = E_FAIL;

		// create vertex buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth				= sizeof(VertexType) * static_cast<unsigned int>(vertices.size());
			desc.Usage					= D3D11_USAGE_DEFAULT;
			desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags			= 0;
			desc.MiscFlags				= 0;
			desc.StructureByteStride	= sizeof(VertexType);

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem				= vertices.data();
			data.SysMemPitch			= 0;
			data.SysMemSlicePitch		= 0;

			hResult = pDev->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		// create index buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth				= sizeof(unsigned int) * static_cast<unsigned int>(indices.size());
			desc.Usage					= D3D11_USAGE_DEFAULT;
			desc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags			= 0;
			desc.MiscFlags				= 0;
			desc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem				= indices.data();
			data.SysMemPitch			= 0;
			data.SysMemSlicePitch		= 0;

			hResult = pDev->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());
			assert(SUCCEEDED(hResult));
		}

		return true;
	}
}