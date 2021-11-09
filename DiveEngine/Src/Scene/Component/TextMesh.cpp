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
		// 폰트 리소스에 해당 이름이 있는지 확인한다.
		// 없다면 리소스 폴더에서 생성한다.
		// 없다면 기본 시스템 폴더에서 생성한다.
		// 없다면 false

		return false;
	}

	// 결국 ascii와 unicode로 나뉘어야 한다.
	bool TextMesh::SetText(const std::wstring& text, const DirectX::XMFLOAT2& position)
	{
		if (m_text == text)
			return false;

		m_vertices.shrink_to_fit();
		m_text = text;

		// Transform으로부터 가져오거나
		// 0, 0으로 설정한 후 Transform값을 이용해 변환해야 한다.
		DirectX::XMFLOAT2 pen = position;

		for (unsigned int i = 0; i != m_text.size(); i++)
		{
			auto index = m_text[i];
			auto letter = m_pFont->GetGlyph(index);

			// 탭, 줄바꿈, 스페이스바 등을 전부 구분해야 한다.
			// 그리고 latain과 local 역시 구분이 필요할 수 있다.
			// 공백인데 값이 다를 수 있다.
			if (index == ' ')
			{
				// 추후 간격을 따로 변수화해야 한다.
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

				pen.x += letter.width + 1.0f;	// 1.0f는 뭐냐??? 출력 문자 사이 간격 같다.
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
		// 사실 size별로 Atlas를 생성하는 것도 에바다.
		// 뭐 다른 방법이 없을까?
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