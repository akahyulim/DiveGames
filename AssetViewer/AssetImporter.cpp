#include "AssetImporter.h"

AssetImporter::AssetImporter()
	: m_pModel(nullptr)
{
}

AssetImporter::~AssetImporter()
{
	Clear();
}

bool AssetImporter::LoadFromFile(const std::string& filePath)
{
	Clear();
   // DV_DELETE(m_pModel);

	return loadExternalExtension(filePath);
}

bool AssetImporter::SaveToFile(const std::string& filePath)
{
	return false;
}

void AssetImporter::Clear()
{
    DV_DELETE(m_pModel);
}

bool AssetImporter::loadExternalExtension(const std::string& filePath)
{
	Assimp::Importer importer;

	uint32_t flags =
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace;
	
	const aiScene* pScene = importer.ReadFile(filePath, flags);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		DV_ERROR("{:s}", importer.GetErrorString());
		return false;
	}

    DV_ASSERT(!m_pModel);
    m_pModel = new Dive::Model();
    m_pModel->SetName(Dive::FileSystem::GetFileName(filePath));

    Dive::GameObject* pRoot = Dive::Scene::CreateGameObject(Dive::FileSystem::GetFileName(filePath));
    parseAndCreateNode(pScene, pScene->mRootNode, pRoot);
    m_pModel->SetRootGameObject(pRoot);
    m_pModel->BuildMeshBuffers();

	importer.FreeScene();

	return true;
}

bool AssetImporter::loadEngineExtension(const std::string& filePath)
{
	return false;
}

void AssetImporter::parseAndCreateNode(const aiScene* pScene, aiNode* pNode, Dive::GameObject* pNodeGameObject, Dive::Transform* pParent)
{
    if (pParent)
    {
        pNodeGameObject->SetName(pNode->mName.C_Str());
        pNodeGameObject->GetTransform()->SetParent(pParent);
    }

    auto transform = pNode->mTransformation;
    DirectX::XMFLOAT4X4 localMatrix = {
        transform.a1, transform.b1, transform.c1, transform.d1,
        transform.a2, transform.b2, transform.c2, transform.d2,
        transform.a3, transform.b3, transform.c3, transform.d3,
        transform.a4, transform.b4, transform.c4, transform.d4
    };
    pNodeGameObject->GetComponent<Dive::Transform>()->SetLocalMatrix(localMatrix);

    if (pNode->mMeshes)
        parseAndCreateMeshes(pScene, pNode, pNodeGameObject);

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
    {
        auto pChild = Dive::Scene::CreateGameObject();
        parseAndCreateNode(pScene, pNode->mChildren[i], pChild, pNodeGameObject->GetTransform());
    }
}

void AssetImporter::parseAndCreateMeshes(const aiScene* pScene, aiNode* pNode, Dive::GameObject* pGameObject)
{
	uint32_t numMeshes = pNode->mNumMeshes;
    for (uint32_t i = 0; i < numMeshes; ++i)
    {
        aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

        // indices
        uint32_t numIndices = pMesh->mNumFaces * 3;
        std::vector<uint32_t> indices;
        indices.resize(numIndices);

        for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            const auto& face = pMesh->mFaces[i];
            const uint32_t index = (i * 3);
            indices[index] = face.mIndices[0];
            indices[index + 1] = face.mIndices[1];
            indices[index + 2] = face.mIndices[2];
        }

        // vertices
        uint32_t numVertices = pMesh->mNumVertices;
        if (!pMesh->mNumBones)
        {
            std::vector<Dive::VertexStatic> vertices;
            vertices.resize(numVertices);

            for (uint32_t i = 0; i < numVertices; ++i)
            {
                Dive::VertexStatic& vertex = vertices[i];

                // position
                const auto& position = pMesh->mVertices[i];
                vertex.position[0] = position.x;
                vertex.position[1] = position.y;
                vertex.position[2] = position.z;

                // texCoords
                const uint32_t uvChannel = 0;
                if (pMesh->HasTextureCoords(uvChannel))
                {
                    const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                    vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                    vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                }

                // normal
                if (pMesh->mNormals)
                {
                    const auto& normal = pMesh->mNormals[i];
                    vertex.normal[0] = normal.x;
                    vertex.normal[1] = normal.y;
                    vertex.normal[2] = normal.z;
                }

                // tangent
                if (pMesh->mTangents)
                {
                    const auto& tangent = pMesh->mTangents[i];
                    vertex.tangent[0] = tangent.x;
                    vertex.tangent[1] = tangent.y;
                    vertex.tangent[2] = tangent.z;
                }
            }

            // Model의 Mesh에 vertices, indices를 넣는다.
            auto pStaticMesh = m_pModel->InsertStaticMesh(new Dive::DvStaticMesh(pMesh->mName.C_Str(), vertices, indices));

            // 현재 GameObject에 MeshRenderer를 생성한 후 Mesh를 연결한다.
            auto pMeshRenderer = pGameObject->AddComponent<Dive::MeshRenderer>();
            pMeshRenderer->SetMesh(pStaticMesh);
            pMeshRenderer->SetMaterial(parseAndCreateMaterials(pScene, pMesh));
        }
        else
        {
            std::vector<Dive::VertexSkinned> vertices;
            vertices.resize(numVertices);

            for (uint32_t i = 0; i < numVertices; ++i)
            {
                Dive::VertexSkinned& vertex = vertices[i];

                // position
                const auto& position = pMesh->mVertices[i];
                vertex.position[0] = position.x;
                vertex.position[1] = position.y;
                vertex.position[2] = position.z;

                // texCoords
                const uint32_t uvChannel = 0;
                if (pMesh->HasTextureCoords(uvChannel))
                {
                    const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                    vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                    vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                }

                // normal
                if (pMesh->mNormals)
                {
                    const auto& normal = pMesh->mNormals[i];
                    vertex.normal[0] = normal.x;
                    vertex.normal[1] = normal.y;
                    vertex.normal[2] = normal.z;
                }

                // tangent
                if (pMesh->mTangents)
                {
                    const auto& tangent = pMesh->mTangents[i];
                    vertex.tangent[0] = tangent.x;
                    vertex.tangent[1] = tangent.y;
                    vertex.tangent[2] = tangent.z;
                }
            }

            // Model의 Mesh에 vertices, indices를 넣는다.
            auto pSkinnedMesh = new Dive::SkinnedMesh(pMesh->mName.C_Str(), vertices, indices);
            m_pModel->InsertSkinnedMesh(pSkinnedMesh);

            // 현재 GameObject에 MeshRenderer를 생성한 후 Mesh를 연결한다.
            auto pMeshRenderer = pGameObject->AddComponent<Dive::SkinnedMeshRenderer>();
            pMeshRenderer->SetMesh(pSkinnedMesh);
            pMeshRenderer->SetMaterial(parseAndCreateMaterials(pScene, pMesh));
        }
    }
}

Dive::Material* AssetImporter::parseAndCreateMaterials(const aiScene* pScene, aiMesh* pMesh)
{
    const auto* pAiMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
    if (!pAiMaterial)
        return Dive::ResourceCache::GetResourceByPath<Dive::Material>("Assets/Materials/Default.yaml");

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
    return Dive::ResourceCache::GetResourceByPath<Dive::Material>("Assets/Materials/Default.yaml");
}

