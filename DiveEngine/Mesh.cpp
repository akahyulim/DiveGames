#include "Mesh.h"
#include "Renderer.h"
#include "Log.h"
//#include "VertexTypes.h"

using namespace std;

namespace Dive
{
	bool Mesh::CreateRenderDate()
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice != nullptr);

		// vertex buffer
		{
			// position buffer
			{
				if (m_positions.empty())
				{
					CORE_ERROR("");
					return false;
				}

				UINT stride = static_cast<UINT>(sizeof(DirectX::XMFLOAT3));

				D3D11_BUFFER_DESC desc;
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.CPUAccessFlags = 0;
				desc.StructureByteStride = stride;
				desc.ByteWidth = stride * static_cast<UINT>(m_positions.size());
				desc.MiscFlags = 0;
				
				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = m_positions.data();
				data.SysMemPitch = 0;
				data.SysMemSlicePitch = 0;

				if (FAILED(pDevice->CreateBuffer(&desc, &data, m_pVBPosition.GetAddressOf())))
				{
					CORE_ERROR("");
					return false;
				}

				m_vertexBufferStride = stride;
				
				// 배열에 추가하기(count 증가)
			}

			// color buffer
			{
				if (!m_colors.empty())
				{
					UINT stride = static_cast<UINT>(sizeof(DirectX::XMFLOAT4));

					D3D11_BUFFER_DESC desc;
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.CPUAccessFlags = 0;
					desc.StructureByteStride = stride;
					desc.ByteWidth = stride * static_cast<UINT>(m_colors.size());
					desc.MiscFlags = 0;

					D3D11_SUBRESOURCE_DATA data;
					data.pSysMem = m_colors.data();
					data.SysMemPitch = 0;
					data.SysMemSlicePitch = 0;

					if (FAILED(pDevice->CreateBuffer(&desc, &data, m_pVBColor.GetAddressOf())))
					{
						CORE_ERROR("");
						return false;
					}
				}
			}

			// uv buffer
			{
				if (!m_uv.empty())
				{

				}
			}

			// normal buffer
			{

			}

			// tangent buffer
			{

			}

		}

		// index buffer
		{
			UINT stride = static_cast<UINT>(sizeof(unsigned int));

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;// stride;
			desc.ByteWidth = stride * static_cast<UINT>(m_indices.size());
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_indices.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(pDevice->CreateBuffer(&desc, &data, m_pIB.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			// 흐음.. 이렇게 하는게 맞나
			// 그런데 indices를 계속 가지고 있는것도 좀 에바다.
			m_indexCount = static_cast<unsigned int>(m_indices.size());
		}

		return true;
	}
}