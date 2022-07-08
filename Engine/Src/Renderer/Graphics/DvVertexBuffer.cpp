#include "divepch.h"
#include "DvVertexBuffer.h"
#include "Base/Base.h"
#include "Renderer/Renderer.h"
#include "GraphicsDevice.h"

namespace Dive
{
	DvVertexBuffer::~DvVertexBuffer()
	{
	}

	bool DvVertexBuffer::SetSize(unsigned int count, const std::vector<VertexElement>& elements, bool bDynamic)
	{
		m_Elements = elements;
		m_VertexCount = count;
		m_bDynamic = bDynamic;

		// UpdateOffsets
		// elements를 분석해 vertex size를 계산
		// 이외에도 hashmap이 있는데 이건 어디에 사용하는지 잘 모르겠고,
		// emelentMask는 LegacyType을 기준으로 요소를 파악하는 용도 같다.
		{
			m_VertexSize = 0;

			for (auto element : m_Elements)
			{
				switch (element.type)
				{
					// 함수화하는 편이 낫다?
				case eVertexElementType::Int:
					m_VertexSize += sizeof(int);
					break;
				case eVertexElementType::Float:
					m_VertexSize += sizeof(float);
					break;
				case eVertexElementType::Float2:
					m_VertexSize += sizeof(DirectX::XMFLOAT2);
					break;
				case eVertexElementType::Float3:
					m_VertexSize += sizeof(DirectX::XMFLOAT3);
					break;
				case eVertexElementType::Float4:
					m_VertexSize += sizeof(DirectX::XMFLOAT4);
					break;
				case eVertexElementType::UByte4:
					m_VertexSize += sizeof(unsigned int);
					break;
				}
			}
		}


		m_pData = new unsigned char[m_VertexCount * m_VertexSize];

		return create();
	}

	bool DvVertexBuffer::create()
	{
		DV_RELEASE(m_pBuffer);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = m_VertexCount * m_VertexSize;
			
		auto graphics = Renderer::GetGraphicsDevice();

		if (FAILED(graphics.GetDevice()->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_pBuffer)))
		{
			DV_RELEASE(m_pBuffer);
			DV_CORE_ERROR("정점 버퍼 생성에 실패하였습니다.");

			return false;
		}

		return true;
	}
}