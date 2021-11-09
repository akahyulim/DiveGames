#include "Mesh.h"

namespace DiveEngine
{
	void Mesh::Clear()
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();

		m_Indices.clear();
		m_Indices.shrink_to_fit();
	}

	bool Mesh::CreateGeometryBuffers(ID3D11Device* pDevice)
	{
		// data
		{
			m_VertexStride = static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan));
			m_VertexBufferOffset = 0;
			m_IndexBufferOffset = 0;
		}

		// vertex buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = m_VertexStride * m_Vertices.size();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_Vertices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer)))
			{
				return false;
			}
		}

		// index buffer
		{
			unsigned int stride = static_cast<unsigned int>(sizeof(unsigned int));

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = stride * m_Indices.size();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_Indices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer)))
			{
				return false;
			}
		}

		return true;
	}
}