#include "divepch.h"
#include "VertexBuffer.h"
#include "GraphicsDevice.h"
#include "Renderer/Renderer.h"
#include "Base/Base.h"

namespace Dive 
{
    void VertexBuffer::Destroy()
    {
        DV_RELEASE(m_pBuffer);
    }

    void* VertexBuffer::Map()
    {
        if (!m_bDynamic)
        {
            DV_CORE_ERROR("Dynamic Buffer가 아닙니다. Vertex buffer Map에 실패하였습니다.");
            return nullptr;
        }

        auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
        DV_ASSERT(pImmediateContext != nullptr);
        DV_ASSERT(m_pBuffer != nullptr);

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if (FAILED(pImmediateContext->Map(static_cast<ID3D11Resource*>(m_pBuffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        {
            DV_CORE_ERROR("Vertex buffer Map에 실패하였습니다.");
            return nullptr;
        }

        return mappedResource.pData;
    }

    void VertexBuffer::Unmap()
    {
        if (!m_bDynamic)
            return;

        auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
        DV_ASSERT(pImmediateContext != nullptr);
        DV_ASSERT(m_pBuffer != nullptr);

        pImmediateContext->Unmap(static_cast<ID3D11Resource*>(m_pBuffer), 0);
    }

    bool VertexBuffer::createBuffer(const void* pData)
	{
		auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
        DV_ASSERT(pDevice != nullptr);

        Destroy();

        D3D11_BUFFER_DESC desc;
        desc.Usage                  = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        desc.ByteWidth              = m_Stride * m_Count;
        desc.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags         = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
        desc.MiscFlags              = 0;
        desc.StructureByteStride    = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem            = pData;
        data.SysMemPitch        = 0;
        data.SysMemSlicePitch   = 0;

        if (FAILED(pDevice->CreateBuffer(&desc, m_bDynamic ? nullptr : &data, &m_pBuffer)))
        {
            DV_CORE_ERROR("Vertex buffer 생성에 실패하였습니다.");
            return false;
        }

		return true;
	}
}