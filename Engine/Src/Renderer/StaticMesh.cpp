#include "divepch.h"
#include "StaticMesh.h"
#include "Base/DiveCore.h"

namespace Dive
{
	StaticMesh::StaticMesh()
	{
		m_Type = eResourceType::Static_Mesh;
	}

	StaticMesh::~StaticMesh()
	{
		Clear();
	}

	void StaticMesh::SetVertices(const std::vector<Vertex_StaticMesh>& vertices)
	{
		if (vertices.empty())
			return;

		if (!m_vertices.empty())
		{
			m_vertices.clear();
			m_vertices.shrink_to_fit();
		}

		m_vertices = vertices;
	}

	bool StaticMesh::CreateBuffers(ID3D11Device* pDevice)
	{
		// vertex buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;		// 이건 무조건 0인가?
			desc.ByteWidth = sizeof(Vertex_StaticMesh) * m_vertices.size();

			D3D11_SUBRESOURCE_DATA data;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			data.pSysMem = m_vertices.data();

			if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer)))
			{
				return false;
			}
		}

		// index buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;	// rastertek, sparatan 둘 다 0이다. 이건 structured buffer와 관련있는 듯 하다.
			desc.ByteWidth = static_cast<UINT>(GetIndexStride() * m_indices.size());
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			if (DXGI_FORMAT_R32_UINT == GetIndexFormat())
			{
				data.pSysMem = m_indices.data();

				if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer)))
				{
					return false;
				}
			}
			else
			{
				std::vector<uint16_t> indices(m_indices.size());
				std::copy(m_indices.begin(), m_indices.end(), indices.begin());

				data.pSysMem = indices.data();

				if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer)))
				{
					return false;
				}
			}
		}

		return true;
	}

	// 이것두 Clear에서 Buffer 부분은 동일할 것이다.
	// 결국 vertices, indices clear만 구분 구현해야 한다.
	void StaticMesh::Clear()
	{
		// buffers
		{
			DV_RELEASE(m_pVertexBuffer);
			DV_RELEASE(m_pIndexBuffer);
		}

		// vertices & indices
		{
			if (!m_vertices.empty())
			{
				m_vertices.clear();
				m_vertices.shrink_to_fit();
			}

			if (!m_indices.empty())
			{
				m_indices.clear();
				m_indices.shrink_to_fit();
			}
		}
	}
}