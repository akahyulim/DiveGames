#include "AssetImporter.h"

static DirectX::XMFLOAT4X4 aiMatrix4x4ToXMFLOAT4X4(aiMatrix4x4 matrix)
{
    return DirectX::XMFLOAT4X4(
        matrix.a1, matrix.b1, matrix.c1, matrix.d1,
        matrix.a2, matrix.b2, matrix.c2, matrix.d2,
        matrix.a3, matrix.b3, matrix.c3, matrix.d3,
        matrix.a4, matrix.b4, matrix.c4, matrix.d4
    );
}

static aiNode* GetNode(const std::string& name, aiNode* pRootNode)
{
    if (!pRootNode)
        return nullptr;

    if (name == pRootNode->mName.C_Str())
        return pRootNode;

    for (size_t i = 0; i < pRootNode->mNumChildren; ++i)
    {
        aiNode* pFound = GetNode(name, pRootNode->mChildren[i]);
        if (pFound)
            return pFound;
    }

    return nullptr;
}

static uint32_t GetBoneIndex(OutModel& model, const std::string& name)
{
    for (size_t i = 0; i < model.bones.size(); ++i)
    {
        if (model.bones[i]->mName.C_Str() == name)
            return i;
    }

    return 0xFFFFFFFF;
}

static void GetBlendData(OutModel& model, aiMesh* pMesh, aiNode* pMeshNode, std::vector<uint32_t>& boneMappings,
    std::vector<std::vector<unsigned char>>& blendIndices, std::vector<std::vector<float>>& blendWeights)
{
    boneMappings.clear();
    blendIndices.resize(pMesh->mNumVertices);
    blendWeights.resize(pMesh->mNumVertices);

    // bone�� 64���� �ʰ��ϸ� boneMappings�� �����Ѵٴ� ���� �� �ϴ�.
    if (model.bones.size() > 64)
    {
        if (pMesh->mNumBones > 64)
        {
            DV_ERROR("bone ������ 64�� �Ѱ���ϴ�.");
            return;
        }

        if (pMesh->mNumBones > 0)
        {
            boneMappings.resize(pMesh->mNumBones, 0);
            for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
            {
                aiBone* pBone = pMesh->mBones[i];
                std::string boneName = pBone->mName.C_Str();
                uint32_t index = GetBoneIndex(model, boneName);
                boneMappings[i] = index;

                for (uint32_t j = 0; j < pBone->mNumWeights; ++j)
                {
                    uint32_t vertexID = pBone->mWeights[j].mVertexId;
                    blendIndices[vertexID].emplace_back(i);
                    blendWeights[vertexID].emplace_back(pBone->mWeights[j].mWeight);
                }
            }
        }
        else
        {
            std::string boneName = pMesh->mName.C_Str();
            uint32_t index = GetBoneIndex(model, boneName);
            boneMappings.emplace_back(index);       // resize ���¶� �� �ָ��ϴ�.
            for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
            {
                blendIndices[i].emplace_back(0);
                blendWeights[i].emplace_back(1.0f);
            }
        }
    }
    else
    {
        if (pMesh->mNumBones > 0)
        {
            for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
            {
                aiBone* pBone = pMesh->mBones[i];
                std::string boneName = pBone->mName.C_Str();
                uint32_t index = GetBoneIndex(model, boneName);
               
                for (uint32_t j = 0; j < pBone->mNumWeights; ++j)
                {
                    uint32_t vertexID = pBone->mWeights[j].mVertexId;
                    blendIndices[vertexID].emplace_back(index);
                    blendWeights[vertexID].emplace_back(pBone->mWeights[j].mWeight);
                }
            }
        }
        else
        {
            std::string boneName = pMesh->mName.C_Str();
            uint32_t index = GetBoneIndex(model, boneName);
            for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
            {
                blendIndices[i].emplace_back(index);
                blendWeights[i].emplace_back(1.0f);
            }
        }
    }

    // weight�� ����ȭ
    for (size_t i = 0; i < blendWeights.size(); ++i)
    {
        // 1. 4���� �ʰ��� ���
        // 4���� �� �� ���� ���� ���� wegiht�� bone�� �����Ѵ�.
        if (blendWeights[i].size() > 4)
        {
            DV_WARN("{:d}��° ������ ������ �ִ� ���밡 4�� �̻��Դϴ�.", i);

            while (blendWeights[i].size() > 4)
            {
                size_t lowestIndex = 0;
                float lowest = std::numeric_limits<float>::infinity();
                for (size_t j = 0; j < blendWeights[i].size(); ++j)
                {
                    if (blendWeights[i][j] < lowest)
                    {
                        lowest = blendWeights[i][j];
                        lowestIndex = j;
                    }
                }

                // erase�� �� ������ ���� �� �ִ�.
                blendWeights[i].erase(blendWeights[i].begin() + lowestIndex);
                blendIndices[i].erase(blendIndices[i].begin() + lowestIndex);
            }
        }
        
        // 2. �� ���� 1.0 �̻��̰ų� 0.0�� �ƴ� ���
        float sum = 0.0f;
        for (uint32_t j = 0; j < blendWeights[i].size(); ++j)
            sum += blendWeights[i][j];
        if (sum != 1.0f && sum != 0.0f)
        {
            for (uint32_t j = 0; j < blendWeights[i].size(); ++j)
                blendWeights[i][j] /= sum;
        }
    }
}

static DirectX::XMFLOAT4X4 GetBoneOffset(OutModel& model, const std::string& boneName)
{
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        aiMesh* pMesh = model.meshes[i];
        aiNode* pMeshNode = model.meshNodes[i];

        for (uint32_t j = 0; j < pMesh->mNumBones; ++j)
        {
            aiBone* pBone = pMesh->mBones[j];
            if (pBone->mName.C_Str() == boneName)
            {
                auto offset =  aiMatrix4x4ToXMFLOAT4X4(pBone->mOffsetMatrix);
                // rootnode���� meshNode���� �����ϰ� �ٽ� offset�� ������ �� �����ϴ� �� �ϴ�.
            }
        }
    }

    return DirectX::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

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
    if (Dive::FileSystem::FileExists(filePath))
    {
        Clear();
        return loadExternalExtension(filePath);
    }

    return false;
}

bool AssetImporter::SaveToFile(const std::string& filePath)
{
	return false;
}

void AssetImporter::Clear()
{
    DV_DELETE(m_pModel);
}

// ��ü ��带 ��ȯ�ϸ鼭 aiMesh�� �ش� aiNode�� ����
void AssetImporter::CollectMeshes(OutModel& model, aiNode* pNode, const aiScene* pScene)
{
    for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
    {
        aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
        for (uint32_t j = 0; j < static_cast<uint32_t>(model.meshes.size()); ++j)
        {
            if (pMesh == model.meshes[j])
            {
                DV_WARN("������ �޽�({:s})�� ������ �߰ߵǾ����ϴ�.", pMesh->mName.C_Str());
                break;
            }
        }

        model.meshes.emplace_back(pMesh);
        model.meshNodes.emplace_back(pNode);
        model.numVertices += pMesh->mNumVertices;
        model.numIndices += pMesh->mNumFaces * 3;
    }

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        CollectMeshes(model, pNode->mChildren[i], pScene);
}

void AssetImporter::CollectBones(OutModel& model, const aiScene* pScene)
{
    // �̰� �� ���� set���� �����ϴ��� �𸣰ڴ�.
    // �ϴ� ������ ��Ұ� �ߺ�������� �ʴ� Ư���� ������ �� �ϴ�.
    std::set<aiNode*> necessary;
    std::set<aiNode*> rootNodes;

    bool bHaveSkinnedMesh = false;
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        if (model.meshes[i]->HasBones())
        {
            bHaveSkinnedMesh = true;
            break;
        }
    }

    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        aiMesh* pMesh = model.meshes[i];
        aiNode* pMeshNode = model.meshNodes[i];
        aiNode* pMeshParentNode = pMeshNode->mParent;
        aiNode* pRootNode = nullptr;

        // bone�� ���ٸ� ���൵ �ȵȴ�.
        for (size_t j = 0; j < pMesh->mNumBones; ++j)
        {
            // �ϴ� bone�� boneNode�� ã�´�.
            aiBone* pBone = pMesh->mBones[j];
            std::string boneName = pBone->mName.C_Str();
            aiNode* pBoneNode = GetNode(boneName, pScene->mRootNode);

            if (!pBoneNode)
            {
                DV_ERROR("Bone {:s}�� Node�� ã�����Ͽ����ϴ�.", boneName);
                return; // �����δ� ���α׷��� �����ؾ� �Ѵ�.
            }

            // boneNode�� necessary�� �ִ´�. �̶� ������� ����ȴ�.
            necessary.emplace(pBoneNode);
            pRootNode = pBoneNode;

            // ���� bone�� node�������� rootNode�� �����Ҷ����� �θ� node�� necessary�� �߰��Ѵ�.
            for(;;)
            {
                pBoneNode = pBoneNode->mParent;
                // ��ǻ� rootNode���� �ݺ��ȴٴ� �ǹ�
                if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode))
                {
                    break;
                }
                pRootNode = pBoneNode;
                // boneNode���� �θ�� �� ����
                necessary.emplace(pBoneNode);
            }

            // boneNode�� rootNode�� �� ���� ����Ǵ� �� �ϴ�.
            // �ظ��ϸ� Scene�� rootNode�̰���?
            if (rootNodes.find(pRootNode) == rootNodes.end())
                rootNodes.emplace(pRootNode);
        }

        // �̰� skinned�� rigid�� �����ִ� ����ε�...
        if (bHaveSkinnedMesh && !pMesh->mNumBones)
        {
            aiNode* pBoneNode = pMeshNode;
            necessary.emplace(pBoneNode);
            pRootNode = pBoneNode;

            // ���� �κ��� ���� ����.
            for (;;)
            {
                pBoneNode = pBoneNode->mParent;
                if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode))
                    break;
                pRootNode = pBoneNode;
                necessary.emplace(pBoneNode);
            }

            if (rootNodes.find(pRootNode) == rootNodes.end())
                rootNodes.emplace(pRootNode);
        }
    }

    if (rootNodes.empty())
        return;

    model.pRootBone = *rootNodes.begin();

    // ���� �������� ����� aiBone�� �̿��� aiNode�� �����Ѵ�.
    // �̶� ����� ������ �߿��ϴ�.
    // �̸� ������� vertex�� boneID�� ����ȴ�.
    // ������ �����尡 �ٸ� ���� ���������� �� �ִٴ� ���̴�.
    // �����常���� ���������� �����Ͽ� ����� �� �ְԲ� ���� ��ȯ�� �ʿ�������.
    // �׸��� ���� ���뿡 �����ϴ� ��(offset)�� ��� ������ �������� �𸥴�.
    CollectBonesFinal(model.bones, necessary, model.pRootBone);
}

void AssetImporter::CollectBonesFinal(std::vector<aiNode*>& dest, const std::set<aiNode*>& necessary, aiNode* pNode)
{
    bool bIncludeBone = necessary.find(pNode) != necessary.end();
    std::string boneName = pNode->mName.C_Str();

    if (bIncludeBone)
        dest.emplace_back(pNode);

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        CollectBonesFinal(dest, necessary, pNode->mChildren[i]);
}

void AssetImporter::BuildAndSaveModel(OutModel& model, const aiScene* pScene)
{
    if (!model.pRootNode)
    {
        DV_ERROR("��Ʈ��尡 �������� �ʾ� ������ �����մϴ�.");
        return;
    }

    // �ϴ� ��Ʈ���� ���ӿ�����Ʈ(+Ʈ������)���� �����Ǿ�� �Ѵ�.
    Dive::GameObject* pRoot = Dive::Scene::CreateGameObject(model.name);
    DirectX::XMFLOAT4X4 transform = aiMatrix4x4ToXMFLOAT4X4(model.pRootNode->mTransformation);
    pRoot->GetTransform()->SetLocalMatrix(transform);
    m_pModel->SetRootGameObject(pRoot);

    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        aiNode* pMeshNode = model.meshNodes[i];
        aiMesh* pMesh = model.meshes[i];

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
        if(!pMesh->mNumBones)
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

            // Model�� Mesh�� vertices, indices�� �ִ´�.
            auto pStaticMesh = m_pModel->InsertStaticMesh(new Dive::StaticMesh(pMesh->mName.C_Str(), vertices, indices));

            // ���� GameObject�� MeshRenderer�� ������ �� Mesh�� �����Ѵ�.
            Dive::GameObject* pGameObject = Dive::Scene::CreateGameObject(pMesh->mName.C_Str());
            auto transform = aiMatrix4x4ToXMFLOAT4X4(pMeshNode->mTransformation);
            pGameObject->GetTransform()->SetLocalMatrix(transform);
            pGameObject->GetTransform()->SetParent(pRoot->GetTransform());

            auto pMeshRenderer = pGameObject->AddComponent<Dive::MeshRenderer>();
            pMeshRenderer->SetMesh(pStaticMesh);
            pMeshRenderer->SetMaterial(parseAndCreateMaterials(pScene, pMesh));
        }
        else
        {
            // blend datas
            std::vector<uint32_t> boneMappings;
            std::vector<std::vector<unsigned char>> blendIndices;
            std::vector<std::vector<float>> blendWeights;
            GetBlendData(model, pMesh, model.meshNodes[i], boneMappings, blendIndices, blendWeights);

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

                // boneIDs
                for (size_t j = 0; j < blendIndices[i].size(); ++j)
                    vertex.boneIDs[j] = blendIndices[i][j];
                
                // weights
                for(size_t j = 0; j < blendWeights[i].size(); ++j)
                    vertex.weights[j] = blendWeights[i][j];
            }

            // Model�� Mesh�� vertices, indices�� �ִ´�.
            auto pSkinnedMesh = new Dive::SkinnedMesh(pMesh->mName.C_Str(), vertices, indices);
            m_pModel->InsertSkinnedMesh(pSkinnedMesh);

            // ���� GameObject�� MeshRenderer�� ������ �� Mesh�� �����Ѵ�.
            Dive::GameObject* pGameObject = Dive::Scene::CreateGameObject(pMesh->mName.C_Str());
            auto transform = aiMatrix4x4ToXMFLOAT4X4(pMeshNode->mTransformation);
            pGameObject->GetTransform()->SetLocalMatrix(transform);
            pGameObject->GetTransform()->SetParent(pRoot->GetTransform());

            auto pMeshRenderer = pGameObject->AddComponent<Dive::SkinnedMeshRenderer>();
            pMeshRenderer->SetMesh(pSkinnedMesh);
            pMeshRenderer->SetMaterial(parseAndCreateMaterials(pScene, pMesh));
        }
    }

    m_pModel->BuildMeshBuffers();

    // ������� ����ġ�� ����ȭ�Ѵ�.
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

    m_FilePath = filePath;

    DV_ASSERT(!m_pModel);
    m_pModel = new Dive::Model();
    m_pModel->SetName(Dive::FileSystem::GetFileName(filePath));
    /*
    Dive::GameObject* pRoot = Dive::Scene::CreateGameObject(Dive::FileSystem::GetFileName(filePath));
    parseAndCreateNode(pScene, pScene->mRootNode, pRoot);
    m_pModel->SetRootGameObject(pRoot);
    m_pModel->BuildMeshBuffers();
    */

    // urho3d�� OutModel�̶�� ����ü�� aiMesh, aiBone ���� �̾Ƴ��� �� BuildAndSaveModel�̶�� �Լ����� Model�� �����ϰ� ����ȭ�Ѵ�.
    OutModel model;
    model.name = Dive::FileSystem::GetFileName(filePath);
    model.pRootNode = pScene->mRootNode;

    CollectMeshes(model, model.pRootNode, pScene);
    CollectBones(model, pScene);

    BuildAndSaveModel(model, pScene);

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
        // �ϴ� �ʿ���� ��带 �����ϱ� ���� �̷��� �������.
        // ������ Skinned�� ��쿣 ��� bone node�� �ʿ��ϴ�.
        // ������ node���� bone ���� ���θ� �� �� ���ٴ� ���̴�.
        // mesh���� bone�� ã�� �� �ٽ� bone�� node�� Ȯ���ؾ� �Ѵ�.
        //if (pNode->mChildren[i]->mMeshes)
        {
            auto pChild = Dive::Scene::CreateGameObject();
            parseAndCreateNode(pScene, pNode->mChildren[i], pChild, pNodeGameObject->GetTransform());
        }
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

            // Model�� Mesh�� vertices, indices�� �ִ´�.
            auto pStaticMesh = m_pModel->InsertStaticMesh(new Dive::StaticMesh(pMesh->mName.C_Str(), vertices, indices));

            // ���� GameObject�� MeshRenderer�� ������ �� Mesh�� �����Ѵ�.
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

            // Model�� Mesh�� vertices, indices�� �ִ´�.
            auto pSkinnedMesh = new Dive::SkinnedMesh(pMesh->mName.C_Str(), vertices, indices);
            m_pModel->InsertSkinnedMesh(pSkinnedMesh);

            // ���� GameObject�� MeshRenderer�� ������ �� Mesh�� �����Ѵ�.
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

    // diffuse map
    aiString texturePath;
    if (pAiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
        pAiMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);

    // material
    auto diffuseTexturePath = Dive::FileSystem::GetPath(m_FilePath) + "textures/";
    diffuseTexturePath += Dive::FileSystem::GetFileNameAndExtension(texturePath.C_Str());
    auto pDiffuseTex = Dive::ResourceCache::GetResourceByPath<Dive::Texture2D>(diffuseTexturePath);
    if (pDiffuseTex)
    {
        Dive::Material* pMat = new Dive::Material;
        pMat->SetName(Dive::FileSystem::GetFileName(m_FilePath));
        pMat->SetTexture(Dive::eTextureUnit::Diffuse, pDiffuseTex);

        Dive::ResourceCache::AddManualResource<Dive::Material>(pMat);

        return pMat;
    }

   // ���ٸ� ����Ʈ ����
    return Dive::ResourceCache::GetResourceByPath<Dive::Material>("Assets/Materials/Default.yaml");
}

