#include "Font.h"
#include "Renderer.h"
#include "DirectXTex/DirectXTex.h"
#include <fstream>

namespace Dive
{
	bool Font::Initialize(const std::wstring& fontFileName, const std::wstring& fontAtlasName)
	{
		assert(!fontFileName.empty());
		assert(!fontAtlasName.empty());

		bool bResult = false;

		bResult = loadFontData(fontFileName);
		assert(bResult);

		bResult = loadFontAtlasName(fontAtlasName);
		assert(bResult);

		return true;
	}
	
	void Font::Destroy()
	{
		if (m_pFontData)
		{
			delete[] m_pFontData;
			m_pFontData = nullptr;
		}

		if (m_pFontAtlas)
		{
			m_pFontAtlas->Release();
			m_pFontAtlas = nullptr;
		}
	}

	void Font::SetText(const std::string& text, const DirectX::XMFLOAT2& position)
	{
		const bool sameText = m_currentText == text;
		//const bool hasBuffers = (m_pVertexBuffer && m_pIndexBuffer);

		// 스파르탄은 버퍼 객체가 따로 있었기에 비교가 가능했다.
		if (sameText)// || !hasBuffers)
			return;

		m_vertices.shrink_to_fit();

		m_currentText = text;
		DirectX::XMFLOAT2 pen = position;

		int letter;
		for (int i = 0; i != (int)m_currentText.size(); i++)
		{
			letter = ((int)m_currentText[i]) - 32;

			if (letter == 0)
			{
				pen.x = pen.x + 3.0f;
			}
			else
			{
				m_vertices.emplace_back(pen.x,								pen.y,				0.0f, m_pFontData[letter].left, 0.0f);	// top left
				m_vertices.emplace_back(pen.x + m_pFontData[letter].size,	pen.y - 16.0f,		0.0f, m_pFontData[letter].right, 1.0f);	// bottom right
				m_vertices.emplace_back(pen.x,								pen.y - 16.0f,		0.0f, m_pFontData[letter].left, 1.0f);	// bottom left

				m_vertices.emplace_back(pen.x,								pen.y,				0.0f, m_pFontData[letter].left, 0.0f);	// top left
				m_vertices.emplace_back(pen.x + m_pFontData[letter].size,	pen.y,				0.0f, m_pFontData[letter].right, 0.0f);	// top right
				m_vertices.emplace_back(pen.x + m_pFontData[letter].size,	pen.y - 16.0f,		0.0f, m_pFontData[letter].right, 1.0f);	// bottom right

				pen.x = pen.x + m_pFontData[letter].size + 1.0f;
			}
		}

		for (int i = 0; i != (int)m_vertices.size(); i++)
		{
			m_indices.emplace_back(i);
		}

		auto result = updateBuffers(m_vertices, m_indices);
		assert(result);
	}

	bool Font::loadFontData(const std::wstring& filename)
	{
		m_pFontData = new Glyph[95];

		std::ifstream fin;
		fin.open(filename.c_str());
		assert(!fin.fail());

		char temp;
		for (int i = 0; i != 95; i++)
		{
			fin.get(temp);
			while (temp != L' ')
			{
				fin.get(temp);
			}
			fin.get(temp);
			while (temp != L' ')
			{
				fin.get(temp);
			}
			fin >> m_pFontData[i].left;
			fin >> m_pFontData[i].right;
			fin >> m_pFontData[i].size;
		}

		fin.close();

		return true;
	}

	bool Font::loadFontAtlasName(const std::wstring& filename)
	{
		HRESULT hr = E_FAIL;
		DirectX::ScratchImage image;

		hr = DirectX::LoadFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));

		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		hr = DirectX::CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_pFontAtlas);
		assert(SUCCEEDED(hr));

		return true;
	}

	bool Font::updateBuffers(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices)
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