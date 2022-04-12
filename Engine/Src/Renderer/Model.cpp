#include "divepch.h"
#include "Model.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
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
        DV_DELETE(m_pIndexBuffer);
        DV_DELETE(m_pVertexBuffer);
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
        m_pVertexBuffer = new VertexBuffer;
        if (!m_pVertexBuffer->Create<VertexType>(pVertices))
        {
            return false;
        }

        // index buffer
        m_pIndexBuffer = new IndexBuffer;
        if (!m_pIndexBuffer->Create(pIndices))
        {
            return false;
        }

        return true;
    }
}