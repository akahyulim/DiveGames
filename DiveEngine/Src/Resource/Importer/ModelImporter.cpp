#include "DivePch.h"
#include "ModelImporter.h"
#include "Core/CoreDefs.h"
#include "Renderer/RendererDefs.h"
#include "Renderer/Model.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Drawable.h"
#include "Scene/Components/Transform.h"
#include "IO/Log.h"
#include "IO/FileSystem.h"

namespace Dive
{
	ModelImporter::ModelImporter()
		: m_pAiScene(nullptr),
		m_pModel(nullptr),
		m_bAnimation(false)
	{
	}

	bool ModelImporter::Load(Model* pModel, const std::string& filePath)
	{
        DV_ASSERT(pModel);

		if (!FileSystem::FileExists(filePath))
		{
			DV_CORE_ERROR("존재하지 않는 파일 경로({:s})를 전달받아 모델 로드에 실패하였습니다.", filePath);
			return false;
		}

		m_FilePath = filePath;
		m_Name = FileSystem::GetFileName(filePath);
		m_pModel = pModel;
		m_pModel->SetName(m_Name);

        Assimp::Importer importer;
        // Set normal smoothing angle
        importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
        // Set tangent smoothing angle
        importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
        // Maximum number of triangles in a mesh (before splitting)
        importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1000000);
        // Maximum number of vertices in a mesh (before splitting)
        importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1000000);
        // Remove points and lines.
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
        // Remove cameras and lights
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
        // Enable progress tracking
        importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
        //importer.SetProgressHandler(new AssimpProgress(filePath));

        const auto importer_flags =
            aiProcess_MakeLeftHanded |           // directx style.
            aiProcess_FlipUVs |                  // directx style.
            aiProcess_FlipWindingOrder |         // directx style.
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_GenUVCoords |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality |     // re-order triangles for better vertex cache locality.
            aiProcess_LimitBoneWeights |
            aiProcess_Triangulate |
            aiProcess_SortByPType |              // splits meshes with more than one primitive type in homogeneous sub-meshes.
            aiProcess_FindDegenerates |          // convert degenerate primitives to proper lines or points.
            aiProcess_FindInvalidData |
            aiProcess_FindInstances |
            aiProcess_ValidateDataStructure;

        if (const aiScene* pScene = importer.ReadFile(filePath, importer_flags))
        {
            m_pAiScene = pScene;
            m_bAnimation = pScene->mNumAnimations != 0;

            parseNode(pScene->mRootNode);

            if (!m_pModel->CreateBuffers())
            {
                m_pModel->Clear();
                return false;
            }
        }
        else
        {
            DV_CORE_ERROR("ModelImporter::Load - {:s}", importer.GetErrorString());
        }

        importer.FreeScene();

		DV_CORE_INFO("ModelImporter가 Model({:s})(을)를 구성을 완료하였습니다.", filePath);

		return m_pAiScene != nullptr;
	}

    void ModelImporter::parseNode(const aiNode* pNode, GameObject* pParentGameObject)
    {
        DV_ASSERT(pNode);

        auto pNodeGameObject = Scene::CreateGameObject();

        bool bRootNode = pParentGameObject == nullptr;
        if (bRootNode) 
        {
            m_pModel->SetRootGameObject(pNodeGameObject);
            pNodeGameObject->GetTransform()->SetParent(nullptr);
            pNodeGameObject->SetName(m_Name);
        }
        else
        {
            pNodeGameObject->GetTransform()->SetParent(pParentGameObject->GetTransform());
            pNodeGameObject->SetName(pNode->mName.C_Str());
        }

        if (pNode->mNumMeshes > 0)
            parseNodeMeshes(pNode, pNodeGameObject);

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
            parseNode(pNode->mChildren[i], pNodeGameObject);
    }

    void ModelImporter::parseNodeMeshes(const aiNode* pNode, GameObject* pNodeGameObject)
    {
        DV_ASSERT(pNode);
        DV_ASSERT(pNodeGameObject);

        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            auto pMeshGameObject = pNodeGameObject;
            auto pNodeMesh = m_pAiScene->mMeshes[pNode->mMeshes[i]];
            std::string name = pNodeMesh->mName.C_Str();

            if (pNode->mNumMeshes > 1)
            {
                pMeshGameObject = Scene::CreateGameObject();
                pMeshGameObject->GetTransform()->SetParent(pNodeGameObject->GetTransform());

                name += "_" + std::to_string(i + 1);
            }

            pMeshGameObject->SetName(name);

            parseMesh(pNodeMesh, pMeshGameObject);
        }
    }

    void ModelImporter::parseMesh(aiMesh* pAiMesh, GameObject* pMeshGameObject)
    {
        DV_ASSERT(pAiMesh);
        DV_ASSERT(pMeshGameObject);

        uint32_t vertexCount = pAiMesh->mNumVertices;
        uint32_t indexCount = pAiMesh->mNumFaces * 3;

        std::vector<VertexModel> vertices;
        vertices.resize(vertexCount);
        for (uint32_t i = 0; i < vertexCount; ++i)
        {
            auto& vertex = vertices[i];

            const auto& pos = pAiMesh->mVertices[i];
            vertex.pos[0] = pos.x;
            vertex.pos[1] = pos.y;
            vertex.pos[2] = pos.z;

            if (pAiMesh->mNormals)
            {
                const auto& normal = pAiMesh->mNormals[i];
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
            }

            if (pAiMesh->mTangents)
            {
                const auto& tangent = pAiMesh->mTangents[i];
                vertex.tangent[0] = tangent.x;
                vertex.tangent[1] = tangent.y;
                vertex.tangent[2] = tangent.z;
            }

            const uint32_t uvChannel = 0;
            if (pAiMesh->HasTextureCoords(uvChannel))
            {
                const auto& texCoords = pAiMesh->mTextureCoords[uvChannel][i];
                vertex.tex[0] = texCoords.x;
                vertex.tex[1] = texCoords.y;
            }
        }

        std::vector<uint32_t> indices;
        indices.resize(indexCount);
        for (uint32_t i = 0; i < pAiMesh->mNumFaces; ++i)
        {
            const auto& face = pAiMesh->mFaces[i];
            const uint32_t index = (i * 3);
            indices[index + 0] = face.mIndices[0];
            indices[index + 1] = face.mIndices[1];
            indices[index + 2] = face.mIndices[2];
        }

        uint32_t vertexOffset = 0;
        m_pModel->AddVertices(vertices, &vertexOffset);

        uint32_t indexOffset = 0;
        m_pModel->AddIndices(indices, &indexOffset);

        auto pDrawable = pMeshGameObject->AddComponent<Drawable>();
        pDrawable->SetMesh(
            m_pModel, 
            pAiMesh->mName.C_Str(), 
            vertexOffset, 
            static_cast<uint32_t>(vertices.size()),
            indexOffset,
            static_cast<uint32_t>(indices.size()));

        DV_CORE_INFO("{0:s}'s VertexCount: {1:d}, IndexCount: {2:d}", pAiMesh->mName.C_Str(), vertexCount, indexCount);
    }
}