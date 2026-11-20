#include "Graphics/VertexBuffer.h"
#include "Graphics/Graphics.h"
#include "Core/Common.h"
#include "Core/LogManager.h"
#include <assert.h>

namespace Dive
{
    VertexBuffer::VertexBuffer(Graphics* graphics, uint32_t stride, uint32_t count, const void* data)
        : m_stride(stride),
        m_count(count)
    {
        assert(data != nullptr && "VertexBuffer (IMMUTABLE) requires initial data!");

        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = static_cast<UINT>(stride * count);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.MiscFlags = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subData{};
        subData.pSysMem = data;

        auto hr = graphics->GetDevice()->CreateBuffer(&desc, &subData, m_buffer.GetAddressOf());
        if (FAILED(hr))
        {
            DV_LOG(Graphics, err, "Failed to create VertexBuffer: (HRESULT: {})", ErrorUtils::ToVerbose(hr));
            assert(false && "VertexBuffer 생성 실패!");
        }
    }

	// 일단 살려두자.
	VertexBuffer::VertexBuffer(uint32_t stride, uint32_t count)
		: m_stride(stride)
		, m_count(count)
	{
	}

	// 1. 동적 버퍼를 지원하는 생성자 구현
	VertexBuffer::VertexBuffer(Graphics* graphics, uint32_t stride, uint32_t count, bool isDynamic)
		: m_stride(stride), m_count(count), m_isDynamic(isDynamic)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = stride * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		if (m_isDynamic)
		{
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		}

		// 초기 데이터가 있다면 함께 넣어줄 수 있습니다.
		HRESULT hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, m_buffer.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create VertexBuffer: (HRESULT: {})", ErrorUtils::ToVerbose(hr));
			assert(false && "VertexBuffer 생성 실패!");
		}
	}

	VertexBuffer::~VertexBuffer() = default;

	// 2. 렌더링 루프에서 매 프레임 호출할 데이터 갱신 함수
	void VertexBuffer::SetData(Graphics* graphics, const void* data, uint32_t count)
	{
		if (!m_isDynamic)
		{
			// 디버그Assert: 정적 버퍼에 SetData를 쓰려고 하면 에러 발생
			return;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		auto context = graphics->GetDeviceContext();

		// MAP_WRITE_DISCARD를 사용하여 GPU 병목(Stalling) 방지
		if (SUCCEEDED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			// 전달받은 데이터 크기만큼 복사 (현재 버퍼 크기를 초과하지 않도록 주의)
			uint32_t dataSize = m_stride * count;
			memcpy(mappedResource.pData, data, dataSize);

			context->Unmap(m_buffer.Get(), 0);
		}

		m_count = count; // 실제 그려야 할 정점 개수 갱신
	}
}