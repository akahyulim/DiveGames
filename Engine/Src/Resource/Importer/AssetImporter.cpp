#include "DivePch.h"
#include "AssetImporter.h"
#include "Core/CoreDefs.h"
#include "Graphics/GraphicsDefs.h"
#include "Graphics/Texture2D.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Drawable.h"
#include "Scene/Components/Transform.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"
#include "IO/FileSystem.h"

namespace Dive
{
    static DirectX::XMFLOAT4X4 ConvertMatrix(const aiMatrix4x4& transform)
    {
        return DirectX::XMFLOAT4X4
        (
            transform.a1, transform.b1, transform.c1, transform.d1,
            transform.a2, transform.b2, transform.c2, transform.d2,
            transform.a3, transform.b3, transform.c3, transform.d3,
            transform.a4, transform.b4, transform.c4, transform.d4
        );
    }

    static aiNode* GetNodeByName(const std::string& name, aiNode* pNode)
    {
        if (!pNode)
            return nullptr;

        if (pNode->mName.C_Str() == name)
            return pNode;

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i) 
        {
            aiNode* pFound = GetNodeByName(name, pNode->mChildren[i]);
            if (pFound)
                return pFound;
        }

        return nullptr;
    }

	AssetImporter::AssetImporter()
		: m_pAiScene(nullptr),
		m_pModel(nullptr),
		m_bAnimation(false)
	{
	}

    // LoadAndBuild
	bool AssetImporter::Load(Model* pModel, const std::string& filePath)
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

        const auto flags =
            aiProcess_MakeLeftHanded |              // directx style.
            aiProcess_FlipUVs |                     // directx style.
            aiProcess_FlipWindingOrder |            // directx style.
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_OptimizeMeshes |              // reduce the number of meshes         
            aiProcess_ImproveCacheLocality |        // re-order triangles for better vertex cache locality.
            aiProcess_RemoveRedundantMaterials |    // remove redundant/unreferenced materials.
            aiProcess_LimitBoneWeights |
            aiProcess_SplitLargeMeshes |
            aiProcess_Triangulate |
            aiProcess_GenUVCoords |
            aiProcess_SortByPType |                 // splits meshes with more than one primitive type in homogeneous sub-meshes.
            aiProcess_FindDegenerates |             // convert degenerate primitives to proper lines or points.
            aiProcess_FindInvalidData |
            aiProcess_FindInstances |
            aiProcess_ValidateDataStructure |
            aiProcess_Debone;

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

        if (const aiScene* pScene = importer.ReadFile(filePath, flags))
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
            DV_CORE_ERROR("AssetImporter::Load - {:s}", importer.GetErrorString());
        }

        importer.FreeScene();

		DV_CORE_DEBUG("AssetImporter가 Model({:s})(을)를 구성을 완료하였습니다.", filePath);

		return m_pAiScene != nullptr;
	}

    // 루트노드부터 시작해 재귀적으로 수행
    // 노드마다 GameObject를 생성한 후 Transform을 추가해 계층구조를 구성
    // 메시가 존재한다면 파싱 호출
    // 이때 계층구조 노드는 bone의 노드일 수도 있다.
    void AssetImporter::parseNode(const aiNode* pNode, GameObject* pParentGameObject)
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

        pNodeGameObject->GetTransform()->SetMatrix(ConvertMatrix(pNode->mTransformation));

        if (pNode->mNumMeshes > 0)
            parseNodeMeshes(pNode, pNodeGameObject);

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
            parseNode(pNode->mChildren[i], pNodeGameObject);
    }

    // 노드 파싱 과정에서 메시가 존재할 때만 호출
    // 일단 해당 게임 오브젝트에 메시의 이름을 설정한 후
    // 본격적으로 메시를 파싱하는 함수를 호출
    void AssetImporter::parseNodeMeshes(const aiNode* pNode, GameObject* pNodeGameObject)
    {
        DV_ASSERT(pNode);
        DV_ASSERT(pNodeGameObject);

        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            auto pMeshGameObject = pNodeGameObject;
            aiMesh* pMesh = m_pAiScene->mMeshes[pNode->mMeshes[i]];
            std::string name = pMesh->mName.C_Str();

            if (pNode->mNumMeshes > 1)
            {
                pMeshGameObject = Scene::CreateGameObject();
                pMeshGameObject->GetTransform()->SetParent(pNodeGameObject->GetTransform());

                // 원래 이름이 다를 수 있는데 이건 너무 강제적이다.
                name += "_" + std::to_string(i + 1);
            }

            pMeshGameObject->SetName(name);

            if (pMesh->mBones)
                buildSkinnedMesh(pMesh, pNodeGameObject);
            else
                buildStaticMesh(pMesh, pNodeGameObject);
            //parseMesh(pAiMesh, pMeshGameObject);
        }
    }

    // 메시를 읽어들인 후 Model에 저장한다.
    // 그리고 이때 Drawable에 offset과 count가 저장된다.
    void AssetImporter::parseMesh(aiMesh* pAiMesh, GameObject* pMeshGameObject)
    {
        DV_ASSERT(pAiMesh);
        DV_ASSERT(pMeshGameObject);

        uint32_t vertexCount = pAiMesh->mNumVertices;
        uint32_t indexCount = pAiMesh->mNumFaces * 3;

        // 하나의 Model에 두 가지 타입의 Mesh가 존재할 수 있다.
        // 따라서 Mesh별로 타입을 확인한 후 MeshRenderer를 맞춰 생성해야 한다.
        std::vector<VertexStatic> vertices;
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
                vertex.tex[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                vertex.tex[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
             }
        }

        std::vector<uint32_t> indices;
        indices.resize(indexCount);
        for (uint32_t i = 0; i < pAiMesh->mNumFaces; ++i)
        {
            const auto& face = pAiMesh->mFaces[i];
            const uint32_t index = (i * 3);
            indices[index] = face.mIndices[0];
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

        collectBones(pAiMesh);

        pDrawable->SetMaterial(parseMaterial(pAiMesh));
    }

    // 현재 문서의 구현을 따랐지만 노드 누적변환에 대한 부분을 찾지 못했다.
    void AssetImporter::collectBones(aiMesh* pMesh)
    {
        auto boneInfoMap = m_pModel->GetBoneInfoMap();
        uint32_t boneCounter = 0;

        for (uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            aiBone* pBone = pMesh->mBones[boneIndex];
            std::string boneName = pBone->mName.C_Str();

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo boneInfo;
                boneInfo.id = boneCounter;
                boneInfo.offsetMatrix = ConvertMatrix(pBone->mOffsetMatrix);

                boneInfoMap[boneName] = boneInfo;

                boneID = boneCounter++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }

            auto weights = pBone->mWeights;
            uint32_t numWeights = pBone->mNumWeights;

            for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                uint32_t vertexIndex = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;

                // set vertex bone data
                // 이건 함수로 한다. 그런데 예제는 본 함수에 Vertices를 전달받았다.
                // model에서 해당 mesh의 raw data를 참조로 가져온다면... 좀 복잡한가...?
            }
        }
    }

    void AssetImporter::buildStaticMesh(aiMesh* pMesh, GameObject* pMeshGameObject)
    {
        uint32_t vertexCount = pMesh->mNumVertices;
        uint32_t indexCount = pMesh->mNumFaces * 3;

        std::vector<VertexStatic> vertices;
        vertices.resize(vertexCount);
        for (uint32_t i = 0; i < vertexCount; ++i)
        {
            auto& vertex = vertices[i];

            const auto& pos = pMesh->mVertices[i];
            vertex.pos[0] = pos.x;
            vertex.pos[1] = pos.y;
            vertex.pos[2] = pos.z;

            if (pMesh->mNormals)
            {
                const auto& normal = pMesh->mNormals[i];
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
            }

            if (pMesh->mTangents)
            {
                const auto& tangent = pMesh->mTangents[i];
                vertex.tangent[0] = tangent.x;
                vertex.tangent[1] = tangent.y;
                vertex.tangent[2] = tangent.z;
            }

            const uint32_t uvChannel = 0;
            if (pMesh->HasTextureCoords(uvChannel))
            {
                const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                vertex.tex[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                vertex.tex[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
            }
        }

        std::vector<uint32_t> indices;
        indices.resize(indexCount);
        for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            const auto& face = pMesh->mFaces[i];
            const uint32_t index = (i * 3);
            indices[index] = face.mIndices[0];
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
            pMesh->mName.C_Str(),
            vertexOffset,
            static_cast<uint32_t>(vertices.size()),
            indexOffset,
            static_cast<uint32_t>(indices.size()));

        pDrawable->SetMaterial(parseMaterial(pMesh));
    }

    void AssetImporter::buildSkinnedMesh(aiMesh* pMesh, GameObject* pMeshGameObject)
    {
        uint32_t vertexCount = pMesh->mNumVertices;
        uint32_t indexCount = pMesh->mNumFaces * 3;

        std::vector<VertexSkinned> vertices;
        vertices.resize(vertexCount);
        for (uint32_t i = 0; i < vertexCount; ++i)
        {
            auto& vertex = vertices[i];

            const auto& pos = pMesh->mVertices[i];
            vertex.pos[0] = pos.x;
            vertex.pos[1] = pos.y;
            vertex.pos[2] = pos.z;

            if (pMesh->mNormals)
            {
                const auto& normal = pMesh->mNormals[i];
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
            }

            if (pMesh->mTangents)
            {
                const auto& tangent = pMesh->mTangents[i];
                vertex.tangent[0] = tangent.x;
                vertex.tangent[1] = tangent.y;
                vertex.tangent[2] = tangent.z;
            }

            const uint32_t uvChannel = 0;
            if (pMesh->HasTextureCoords(uvChannel))
            {
                const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                vertex.tex[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                vertex.tex[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
            }
        }

        std::vector<uint32_t> indices;
        indices.resize(indexCount);
        for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            const auto& face = pMesh->mFaces[i];
            const uint32_t index = (i * 3);
            indices[index] = face.mIndices[0];
            indices[index + 1] = face.mIndices[1];
            indices[index + 2] = face.mIndices[2];
        }

        /*
        uint32_t vertexOffset = 0;
        m_pModel->AddVertices(vertices, &vertexOffset);

        uint32_t indexOffset = 0;
        m_pModel->AddIndices(indices, &indexOffset);

        auto pDrawable = pMeshGameObject->AddComponent<Drawable>();
        pDrawable->SetMesh(
            m_pModel,
            pMesh->mName.C_Str(),
            vertexOffset,
            static_cast<uint32_t>(vertices.size()),
            indexOffset,
            static_cast<uint32_t>(indices.size()));
        */

        collectBones(pMesh);

        //pDrawable->SetMaterial(parseMaterial(pMesh));
    }

    Material* AssetImporter::parseMaterial(aiMesh* pAiMesh)
    {
        DV_ASSERT(pAiMesh);

        const auto* pAiMaterial = m_pAiScene->mMaterials[pAiMesh->mMaterialIndex];
        if (!pAiMaterial)
            return ResourceCache::GetResourceByPath<Material>("Assets/Materials/Default.yaml");

        aiString name;
        aiGetMaterialString(pAiMaterial, AI_MATKEY_NAME, &name);

        aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
        aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);

        aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
        aiGetMaterialColor(pAiMaterial, AI_MATKEY_OPACITY, &opacity);

        // textures
        aiString texturePath;
        if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
            pAiMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);

        DV_CORE_INFO("Original Material Path: {:s}", texturePath.C_Str());

        /*
        * 1. 우선 파싱한 경로에서 파일을 찾아본다.
        * 2. texture 폴더에서 파일을 찾아본다.
        * 3. texture 폴더에서 다른 확장자로 파일을 찾아본다.
        * 4. 디폴트 재질을 전달한다.
        */

        /*
        auto diffTexPath = //FileSystem::GetFileNameAndExtension(texturePath.C_Str());
            //texturePath.C_Str();
            "Assets/Models/pilot-avatar/textures/Material.002_Base_Color.png";
            //"Assets/Models/dancing-stormtrooper/textures/stormtrooper_D.png";
        //diffTexPath = "Assets/Models/sponza-master/textures/" + diffTexPath;
        
        auto diffTex = ResourceCache::GetResourceByPath<Texture2D>(diffTexPath);
        if (diffTex)
        {
            auto pMaterial = new Material();
            pMaterial->SetName(name.C_Str());
            pMaterial->SetColorAlbedo(diffuse.r, diffuse.g, diffuse.b, opacity.r);
            pMaterial->SetTexture(eTextureUnit::Diffuse, diffTex);
            ResourceCache::AddManualResource<Material>(pMaterial);
            return pMaterial;
        }
        */
        return ResourceCache::GetResourceByPath<Material>("Assets/Materials/Default.yaml");
    }
}