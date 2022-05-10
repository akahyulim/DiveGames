#include "divepch.h"
#include "Model.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Scene/GameObject.h"
#include "Base/Base.h"
#include "Helper/FileSystem.h"
#include "Resource/FileStream.h"
#include "Resource/Importer/ModelImporter.h"

// 이건 좀 에바?
#include "Base/Engine.h"
#include "Dive.h"

namespace Dive
{
    Model::Model(const std::string& name, unsigned long long id)
        : Resource(name, id)
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

    bool Model::LoadFromFile(const std::string& filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            DV_CORE_ERROR("파일이 존재하지 않습니다 - {:s}", filepath);
            return false;
        }

        // 1. GetCurrentEngine은 사용자를 위한 API였는데... 접근 방식이 좀 에바다.
        // 2. ResourceManager에서 .dat를 이용해 로드할 경우
        // 새로운 GameObject 계층 구조가 생성된다.
        // 이를 구분하려면 별도 포멧으로 Vertices, Indices 등을 저장한 후
        // Load시 버퍼만 생성하는 수 밖에 없다.
        // 그리고 이렇게 생성된 Model 객체를 Scene의 GameObject에 직접 연결해야 한다.
        ModelImporter importer(GetCurrentEngine()->GetActiveScene());
        if (!importer.Load(this, filepath))
            return false;

        SetFilepath(filepath);

        UpdateGeometry();

        return true;
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
        if (!m_pIndexBuffer->Create<unsigned int>(pIndices))
        {
            return false;
        }

        return true;
    }
}