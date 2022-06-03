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

// �̰� �� ����?
#include "Base/Engine.h"
#include "Dive.h"

namespace Dive
{
    Model::Model()
    {
    }

    Model::Model(unsigned long long id)
        : Resource(id)
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

    // ���� ���� ���� ��θ� �޴´�.
    bool Model::SaveToFile(const std::string& filepath)
    {
        FileStream fout(filepath, eFileStreamMode::Write);
        if (!fout.IsOpen())
            return false;

        // �ܺ� ���� ���?
        fout.Write(m_NormalizedScale);
        fout.Write(m_Mesh.GetVertices());
        fout.Write(m_Mesh.GetIndices());

        fout.Close();

        return true;
    }

    // �ϴ� ���ĸ�ź�� �ܺ�, ���� ���� �������� ����.
    bool Model::LoadFromFile(const std::string& filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            DV_CORE_ERROR("������ �������� �ʽ��ϴ� - {:s}", filepath);
            return false;
        }

        if (Helper::FileSystem::GetFileExtension(filepath) == EXTENSION_MODEL)
        {
            FileStream fin(filepath, eFileStreamMode::Read);
            if (!fin.IsOpen())
                return false;

            fin.Read(&m_NormalizedScale);
            fin.Read(&m_Mesh.GetVertices());
            fin.Read(&m_Mesh.GetIndices());

            fin.Close();

            // file path ����
            // ��� �� �̸��� �����ϴ� �뵵��.
            {
                m_ExportedFilepath.clear();
                m_EngineFilepath = filepath;
                m_Name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);
            }

            UpdateGeometry();
        }
        else
        {
            // ���⵵ file path ����?
            // Engine Format path�� �����ϴ� �뵵��.
            {
                m_ExportedFilepath = filepath;
                m_EngineFilepath = Helper::FileSystem::GetFilepathWithoutExtension(filepath) + ".model";
                m_Name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);
            }

            ModelImporter importer(GetCurrentEngine()->GetActiveScene());
            if (!importer.Load(this, filepath))
                return false;
        }

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