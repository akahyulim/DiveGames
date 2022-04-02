#include "divepch.h"
#include "Model.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Scene/GameObject.h"
#include "Base/Base.h"

namespace Dive
{
    Model::Model()
    {
    }

    Model::~Model()
    {
        Clear();
    }
    
    void Model::Clear()
    {
        DV_RELEASE(m_pIndexBuffer);
        DV_RELEASE(m_pVertexBuffer);
        m_Mesh.Clear();

        m_pRootGameObject = nullptr;
    }
    
    void Model::UpdateGeometry()
    {
        if ((m_Mesh.GetVertexCount() == 0) || (m_Mesh.GetIndexCount() == 0))
        {
            return;
        }

        createBuffers();
    }

    void Model::AppendGeometry(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices, unsigned int* pOutVertexOffset, unsigned int* pOutIndexOffset)
    {
        if (vertices.empty() || indices.empty())
            return;

        m_Mesh.AppendVertices(vertices, pOutVertexOffset);
        m_Mesh.AppendIndices(indices, pOutIndexOffset);
    }

    void Model::GetGeometry(unsigned int vertexOffset, unsigned int vertexCount, std::vector<VertexType>* pOutVertices, unsigned int indexOffset, unsigned int indexCount, std::vector<unsigned int>* pOutIndices)
    {
        m_Mesh.GetGeometry(vertexOffset, vertexCount, pOutVertices, indexOffset, indexCount, pOutIndices);
    }

    bool Model::createBuffers()
    {
        auto pDevice = Renderer::GetGraphicsDevice().GetDevice();
        if (!pDevice)
            return false;

        const auto pVertices = m_Mesh.GetVertices();
        const auto pIndices = m_Mesh.GetIndices();

        // vertex buffer
        {
            if (pVertices.empty())
                return false;

            D3D11_BUFFER_DESC desc;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = static_cast<UINT>(sizeof(VertexType)) * static_cast<UINT>(pVertices.size());
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            desc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA data;
            data.pSysMem = pVertices.data();
            data.SysMemPitch = 0;
            data.SysMemSlicePitch = 0;

            if (FAILED(pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer)))
            {
                return false;
            }
        }

        // index buffer
        {
            if (pIndices.empty())
                return false;

            D3D11_BUFFER_DESC desc;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = static_cast<UINT>(sizeof(unsigned int)) * static_cast<UINT>(pIndices.size());
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            desc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA data;
            data.pSysMem = pIndices.data();
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