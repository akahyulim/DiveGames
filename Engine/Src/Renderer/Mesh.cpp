#include "divepch.h"
#include "Mesh.h"

namespace Dive
{
    void Mesh::Clear()
    {
        m_Vertices.clear();
        m_Vertices.shrink_to_fit();

        m_Indices.clear();
        m_Indices.shrink_to_fit();
    }

    void Mesh::GetGeometry(unsigned int vertexOffset, unsigned int vertexCount, std::vector<VertexType>* pOutVertices, unsigned int indexOffset, unsigned int indexCount, std::vector<unsigned int>* pOutIndices)
    {
        if ((!pOutVertices) || (!pOutIndices) || (vertexOffset == 0 && vertexCount == 0) || (indexOffset == 0 && indexCount == 0))
        {
            return;
        }

        const auto vertexBegin = m_Vertices.begin() + vertexOffset;
        const auto vertexEnd = vertexBegin + vertexCount;
        *pOutVertices = std::vector<VertexType>(vertexBegin, vertexEnd);

        const auto indexBegin = m_Indices.begin() + indexOffset;
        const auto indexEnd = indexBegin + indexCount;
        *pOutIndices = std::vector<unsigned int>(indexBegin, indexEnd);
    }

    void Mesh::AddVertex(const VertexType& vertex)
    {
        m_Vertices.emplace_back(vertex);
    }
    
    void Mesh::AppendVertices(const std::vector<VertexType>& vertices, unsigned int* pOutOffset)
    {
        if (pOutOffset)
        {
            *pOutOffset = GetVertexCount();
        }

        m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
    }
    
    void Mesh::AddIndex(unsigned int index)
    {
        m_Indices.emplace_back(index);
    }
    
    void Mesh::AppendIndices(const std::vector<unsigned int>& indices, unsigned int* pOutOffset)
    {
        if (pOutOffset)
        {
            *pOutOffset = GetIndexCount();
        }

        m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
    }
}