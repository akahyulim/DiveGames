#include "divepch.h"
#include "IndexBuffer.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	IndexBuffer::IndexBuffer()
		: m_pBuffer(nullptr)
		, m_Count(0)
	{
	}

	IndexBuffer::IndexBuffer(uint16_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R16_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint16_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	IndexBuffer::IndexBuffer(uint32_t* pIndices, uint32_t count)
		: m_Count(count)
		, m_Format(DXGI_FORMAT_R32_UINT)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(uint32_t) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if(FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
	}

	IndexBuffer::~IndexBuffer()
	{
		Destroy();
	}

	bool IndexBuffer::SetSize(uint32_t count, bool dynamic)
	{
		// urho3d에 있는 메서드다.
		// count, size, dynamic 여부를 저장하고 create 함수를 호출한다.
		// create함수는 일단 버퍼를 생성한다.

		return false;
	}
	// 외부에서 버퍼를 생성한 후 Mesh에 저장할 것이라면
	// 굳이 이 SetSize+SetData 콤보를 사용할 필요는 없을 것 같다.
	bool IndexBuffer::SetData(const void* pData)
	{
		// 생성된 버퍼에 데이터를 전달해 완성한다.
		// dynamic이라면 map / unmap으로,
		// 아니라면 UpdateSubResource로 처리한다.

		return false;
	}

	bool IndexBuffer::CreateBuffer(const void* pData, uint32_t count)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = (count > 65535 ? sizeof(uint32_t) : sizeof(uint16_t)) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer)))
		{
			DV_ERROR("IndexBuffer 생성에 실패하였습니다.");
			return false;
		}

		m_Count = count;

		return true;
	}

	void IndexBuffer::Destroy()
	{
		DV_RELEASE(m_pBuffer);
	}

	IndexBuffer* IndexBuffer::Create(uint16_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* pIndices, uint32_t count)
	{
		return new IndexBuffer(pIndices, count);
	}
}