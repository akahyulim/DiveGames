#include "AssetImporter.h"

static std::string s_FilePath;
static std::string s_ModelName;
static const aiScene* s_pScene = nullptr;
static Dive::Model* s_pModel = nullptr;

DirectX::XMFLOAT3 ConvertXMFLOAT3(const aiVector3D& vec)
{
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

DirectX::XMFLOAT4 ConvertXMFLOAT4(const aiQuaternion& vec)
{
    return DirectX::XMFLOAT4(vec.x, vec.y, vec.z, vec.w);
}

DirectX::XMFLOAT4X4 ConvertXMFLOAT4X4(const aiMatrix4x4& matrix)
{
    return DirectX::XMFLOAT4X4(
        matrix.a1, matrix.b1, matrix.c1, matrix.d1,
        matrix.a2, matrix.b2, matrix.c2, matrix.d2,
        matrix.a3, matrix.b3, matrix.c3, matrix.d3,
        matrix.a4, matrix.b4, matrix.c4, matrix.d4
    );
}

aiNode* FindNodeByName(aiNode* pNode, const aiString& name)
{
    if (!pNode)
        return nullptr;

    if (pNode->mName == name)
        return pNode;

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
    {
        auto pFound = FindNodeByName(pNode->mChildren[i], name);
        if (pFound)
            return pFound;
    }

    return nullptr;
}

void GetBlendData(aiMesh* pMesh, std::vector<std::vector<int>>& outBlendIndices, std::vector<std::vector<float>>& outBlendWeights)
{
    if (!pMesh || pMesh->mNumBones == 0)
        return;

    outBlendIndices.resize(pMesh->mNumVertices);
    outBlendWeights.resize(pMesh->mNumVertices);

    for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
    {
        aiBone* pBone = pMesh->mBones[i];
        for (uint32_t j = 0; j < pBone->mNumWeights; ++j)
        {
            uint32_t vertexID = pBone->mWeights[j].mVertexId;
            outBlendIndices[vertexID].emplace_back(i);
            outBlendWeights[vertexID].emplace_back(pBone->mWeights[j].mWeight);
        }
    }

    // outBlendWeights�� ����ȭ
    for (uint32_t i = 0; i < static_cast<uint32_t>(outBlendWeights.size()); ++i)
    {
        // 1. ���밡 4���� �ʰ��� ��� ���� ���� wegiht�� bone�� ����
        if (outBlendWeights[i].size() > 4)
        {
            DV_WARN("{:d}��° ������ ������ �ִ� ���밡 4�� �̻��Դϴ�.", i);

            while (outBlendWeights[i].size() > 4)
            {
                size_t lowestIndex = 0;
                float lowest = std::numeric_limits<float>::infinity();
                for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
                {
                    if (outBlendWeights[i][j] < lowest)
                    {
                        lowest = outBlendWeights[i][j];
                        lowestIndex = j;
                    }
                }

                outBlendWeights[i].erase(outBlendWeights[i].begin() + lowestIndex);
                outBlendIndices[i].erase(outBlendIndices[i].begin() + lowestIndex);
            }
        }

        // 2. �� ���� 1.0 �̻��̰ų� 0.0�� �ƴ� ��� ����ȭ
        float sum = 0.0f;
        for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
            sum += outBlendWeights[i][j];
        if (sum != 1.0f && sum != 0.0f)
        {
            DV_WARN("Weight ������ 1.0�� �ƴϾ ����ȭ�� ����");

            for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
                outBlendWeights[i][j] /= sum;
        }
    }
}

Dive::GameObject* GetNodeObjectByBoneName(Dive::GameObject* pNodeObject, const std::string& boneName)
{
    if (!pNodeObject || boneName.empty())
        return nullptr;

    if (pNodeObject->GetName() == boneName)
        return pNodeObject;

    for (Dive::Transform* pChild : pNodeObject->GetTransform()->GetChildren())
    {
        Dive::GameObject* pNode = GetNodeObjectByBoneName(pChild->GetGameObject(), boneName);
        if (pNode)
            return pNode;
    }

    return nullptr;
}

Dive::Material* ParseAndCreateMaterials(aiMesh* pMesh)
{
    const auto* pAiMaterial = s_pScene->mMaterials[pMesh->mMaterialIndex];
    if (!pAiMaterial)
        return Dive::ResourceCache::GetResourceByPath<Dive::Material>("Assets/Materials/Default.yaml");

    {
        aiString stringVal;
        float floatVal;
        int intVal;
        aiColor3D colorVal;

        std::string diffuseTexName, normalTexName, specularTexName, lightmapTexName, emissiveTexName;

        if (pAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), stringVal) == AI_SUCCESS)
            diffuseTexName = Dive::FileSystem::GetFileNameAndExtension(stringVal.C_Str());
        if (pAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), stringVal) == AI_SUCCESS)
            normalTexName = Dive::FileSystem::GetFileNameAndExtension(stringVal.C_Str());
        if (pAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), stringVal) == AI_SUCCESS)
            specularTexName = Dive::FileSystem::GetFileNameAndExtension(stringVal.C_Str());
        if (pAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), stringVal) == AI_SUCCESS)
            lightmapTexName = Dive::FileSystem::GetFileNameAndExtension(stringVal.C_Str());
        if (pAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), stringVal) == AI_SUCCESS)
            emissiveTexName = Dive::FileSystem::GetFileNameAndExtension(stringVal.C_Str());

        std::cout << "diffuseTexName: " << diffuseTexName.c_str() << std::endl;

        if (auto pInsideTex = s_pScene->GetEmbeddedTexture(diffuseTexName.c_str()))
        {
            auto height = pInsideTex->mHeight;
            auto width = pInsideTex->mWidth;
            
            // �뷫 �̷��� �ϴ� �� �ѵ� height�� 0�̴�... ���� ����ؾ� �ϳ�?
            // https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedDirectx11/SimpleTexturedDirectx11/ModelLoader.cpp
            // ������ �ִ�.
            Dive::Texture2D* pDiffTex = new Dive::Texture2D();
            
        }

    }

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
    auto diffuseTexturePath = Dive::FileSystem::GetPath(s_FilePath) + "textures/";
    diffuseTexturePath += Dive::FileSystem::GetFileNameAndExtension(texturePath.C_Str());
    auto pDiffuseTex = Dive::ResourceCache::GetResourceByPath<Dive::Texture2D>(diffuseTexturePath);
    if (pDiffuseTex)
    {
        Dive::Material* pMat = new Dive::Material;
        pMat->SetName(Dive::FileSystem::GetFileName(s_FilePath));
        pMat->SetTexture(Dive::eTextureUnit::Diffuse, pDiffuseTex);

        Dive::ResourceCache::AddManualResource<Dive::Material>(pMat);

        return pMat;
    }

    // ���ٸ� ����Ʈ ����
    return Dive::ResourceCache::GetResourceByPath<Dive::Material>("Assets/Materials/Default.yaml");
}

void ParseCreateMeshes(aiMesh* pMesh, Dive::GameObject* pGameObject)
{
    // indices
    uint32_t numIndices = pMesh->mNumFaces * 3;
    std::vector<uint32_t> indices;
    indices.resize(numIndices);

    for (uint32_t faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex)
    {
        const auto& face = pMesh->mFaces[faceIndex];
        const uint32_t index = (faceIndex * 3);
        indices[index] = face.mIndices[0];
        indices[(size_t)index + 1] = face.mIndices[1];
        indices[(size_t)index + 2] = face.mIndices[2];
    }

    // vertices
    uint32_t numVertices = pMesh->mNumVertices;
    if (!pMesh->mNumBones)
    {
        std::vector<Dive::VertexStatic> vertices;
        vertices.resize(numVertices);

        for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
        {
            Dive::VertexStatic& vertex = vertices[vertexIndex];

            // position
            const auto& position = pMesh->mVertices[vertexIndex];
            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;

            // texCoords
            const uint32_t uvChannel = 0;
            if (pMesh->HasTextureCoords(uvChannel))
            {
                const auto& texCoords = pMesh->mTextureCoords[uvChannel][vertexIndex];
                vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
            }

            // normal
            if (pMesh->mNormals)
            {
                const auto& normal = pMesh->mNormals[vertexIndex];
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
            }

            // tangent
            if (pMesh->mTangents)
            {
                const auto& tangent = pMesh->mTangents[vertexIndex];
                vertex.tangent[0] = tangent.x;
                vertex.tangent[1] = tangent.y;
                vertex.tangent[2] = tangent.z;
            }
        }

        auto pStaticMesh = s_pModel->InsertStaticMesh(new Dive::StaticMesh(pMesh->mName.C_Str(), vertices, indices));
        auto pMeshRenderer = pGameObject->AddComponent<Dive::MeshRenderer>();
        pMeshRenderer->SetMesh(pStaticMesh);
        pMeshRenderer->SetMaterial(ParseAndCreateMaterials(pMesh));
    }
    else
    {
        std::vector<std::vector<int>> blendIndices;
        std::vector<std::vector<float>> blendWeights;
        GetBlendData(pMesh, blendIndices, blendWeights);

        std::vector<Dive::VertexSkinned> vertices;
        vertices.resize(numVertices);

        for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
        {
            Dive::VertexSkinned& vertex = vertices[vertexIndex];

            // position
            const auto& position = pMesh->mVertices[vertexIndex];
            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;

            // texCoords
            const uint32_t uvChannel = 0;
            if (pMesh->HasTextureCoords(uvChannel))
            {
                const auto& texCoords = pMesh->mTextureCoords[uvChannel][vertexIndex];
                vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
            }

            // normal
            if (pMesh->mNormals)
            {
                const auto& normal = pMesh->mNormals[vertexIndex];
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
            }

            // tangent
            if (pMesh->mTangents)
            {
                const auto& tangent = pMesh->mTangents[vertexIndex];
                vertex.tangent[0] = tangent.x;
                vertex.tangent[1] = tangent.y;
                vertex.tangent[2] = tangent.z;
            }

            // boneIDs
            for (uint32_t j = 0; j < static_cast<uint32_t>(blendIndices[vertexIndex].size()); ++j)
                vertex.boneIDs[j] = blendIndices[vertexIndex][j];

            // weights
            for (uint32_t j = 0; j < static_cast<uint32_t>(blendWeights[vertexIndex].size()); ++j)
                vertex.weights[j] = blendWeights[vertexIndex][j];
        }

        // �޽ð� 2�� �̻��̶� ����ϴ� ������ ������ ���ٴ� ���� Ȯ��
        DV_INFO("Mesh({0:s})�� bone ����: {1:d}", pMesh->mName.C_Str(), pMesh->mNumBones);

        // ���� �ϳ��� ��忡 �� �̻��� �޽ð� ������ ���
        // �޽ø� �߰��� �����ϰ� �ִ�.
        // �̶� �Ʒ��� ������ �Բ� ����Ǵµ�
        // ���� Model�� �ϳ��� Skeleton�� ���� �� �ִ�.
        Dive::Skeleton skeleton;
        auto& bones = skeleton.GetModifiableBones();
        bones.resize(pMesh->mNumBones);
        // ���� ���̷����� �� �� �̻��� �� ������ model���� �ϳ��� �����Ѵ�.
        // ���� �ƿ� ������ó�� model���� �ϳ��� ������� �� �����̴�.
        auto& boneInfoMap = s_pModel->GetBoneInfoMap();

        // ���� �޽ð� ����ϴ� ������ ������ ��Ƽ� �����ϴ� ���̴�.
        for (uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
        {
            aiBone* pBone = pMesh->mBones[boneIndex];
            bones[boneIndex].name = pBone->mName.C_Str();
            bones[boneIndex].index = boneIndex;
            bones[boneIndex].offsetMatrix = ConvertXMFLOAT4X4(pBone->mOffsetMatrix);

            // id��... ������ �� �� ����.
            std::string boneName = pBone->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                Dive::BoneInfo newBoneInfo;
                newBoneInfo.index = boneIndex;
                newBoneInfo.offsetMatrix = ConvertXMFLOAT4X4(pBone->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
            }
        }
        s_pModel->SetSkeleton(skeleton);

        auto pSkinnedMesh = new Dive::SkinnedMesh(pGameObject->GetName(), vertices, indices);
        s_pModel->InsertSkinnedMesh(pSkinnedMesh);

        auto pMeshRenderer = pGameObject->AddComponent<Dive::SkinnedMeshRenderer>();
        pMeshRenderer->SetMesh(pSkinnedMesh);
        pMeshRenderer->SetMaterial(ParseAndCreateMaterials(pMesh));

        // temp: �� ������ SkinnedMeshRenderer�� BoneInfo�� ������ �� �ִ�.
        pMeshRenderer->SetBones(bones);

        // temp
        auto pAnimator = pGameObject->AddComponent<Dive::Animator>();
        // ���� �ִϸ��̼��� �������� �ʾҴ�.
        //pAnimator->SetAnimation(Dive::ResourceCache::GetResourceByName<Dive::Animation>("TestAnim"));
    }
}

void ParseAndCreateMeshes(aiNode* pNode, Dive::GameObject* pGameObject)
{
    for (uint32_t meshIndex = 0; meshIndex < pNode->mNumMeshes; ++meshIndex)
    {
        auto pNodeObject = pGameObject;
        auto pMesh = s_pScene->mMeshes[pNode->mMeshes[meshIndex]];
        std::string name = pNode->mName.C_Str();
        
        if (meshIndex > 0)
        {
            pNodeObject = Dive::Scene::CreateGameObject();
            auto pTransform = pNodeObject->GetTransform();
            pTransform->SetParent(pGameObject->GetTransform()->GetParent());
            pTransform->SetLocalMatrix(ConvertXMFLOAT4X4(pNode->mTransformation));
            // ���� �̸���� ���ϴ� ����� �õ��غ���.
            name += "_" + std::to_string(meshIndex);
        }
        pNodeObject->SetName(name);

        ParseCreateMeshes(pMesh, pNodeObject);
    }
}

// static variable�� ����Ѵ�. ��, ��� ��� �Լ��� ����� ���� ��︰��.
void ParseAndCreateNodes(aiNode* pNode, Dive::Transform* pParent)
{
    auto pNodeObject = Dive::Scene::CreateGameObject();
    auto pTransform = pNodeObject->GetTransform();
    if (pParent)
        pTransform->SetParent(pParent);
    else
        s_pModel->SetRootGameObject(pNodeObject);
    pTransform->SetLocalMatrix(ConvertXMFLOAT4X4(pNode->mTransformation));
    
    pNodeObject->SetName(pParent ? pNode->mName.data : s_ModelName);

    if (pNode->mNumMeshes > 0)
        ParseAndCreateMeshes(pNode, pNodeObject);

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        ParseAndCreateNodes(pNode->mChildren[i], pTransform);
}

AssetImporter::~AssetImporter()
{
	Clear();
}

bool AssetImporter::LoadFromFile(const std::string& filePath)
{
    Clear();

    if (Dive::FileSystem::FileExists(filePath))
        return LoadExternalFile(filePath);

    return false;
}

bool AssetImporter::LoadExternalFile(const std::string& filePath)
{
    Assimp::Importer importer;

    uint32_t flags =
        aiProcess_LimitBoneWeights |
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace;

    // �̰� �� ������ �������־���.
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    
    s_pScene = importer.ReadFile(filePath, flags);
    if (!s_pScene || s_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !s_pScene->mRootNode)
    {
        DV_ERROR("{:s}", importer.GetErrorString());
        return false;
    }

    s_FilePath = filePath;
    s_ModelName = Dive::FileSystem::GetFileName(filePath);

    DV_ASSERT(!s_pModel);
    s_pModel = new Dive::Model();
    s_pModel->SetName(s_ModelName);
    
    ParseAndCreateNodes(s_pScene->mRootNode, nullptr);

    CollectAnimations();

    auto pRootObject = s_pModel->GetRootGameObject();

    // skeleton���κ��� bones�� ������ �� BoneRenderer�� ����� SkinnedMeshRenderer�� bones�� �߰��ϴ� ����
    {
        auto skeleton = s_pModel->GetSkeleton();
        const auto& bones = skeleton.GetBones();
        for (uint32_t i = 0; i < static_cast<uint32_t>(bones.size()); ++i)
        {
            Dive::GameObject* pNodeObject = GetNodeObjectByBoneName(pRootObject, bones[i].name);
            if (!pNodeObject)
            {
                DV_ERROR("{:s} GameObject�� �������� �ʽ��ϴ�.", bones[i].name.c_str());
                continue;
            }
            pNodeObject->AddComponent<Dive::BoneRenderer>();
        }

        std::function<void (Dive::GameObject*)> func = [&func](Dive::GameObject* pNode)
        {
            if (!pNode)
                return;

            auto pAnimator = pNode->GetComponent<Dive::Animator>();
            if (pAnimator)
                pAnimator->SetAnimation(Dive::ResourceCache::GetResourceByName<Dive::Animation>("TestAnim"));

            auto pTransform = pNode->GetTransform();
            for (auto pChild : pTransform->GetChildren())
                func(pChild->GetGameObject());
        };

        func(pRootObject);
    }
    s_pModel->BuildMeshBuffers();
 
    importer.FreeScene();

    DV_INFO("Model({:s}) successfully loaded.", filePath);

    return true;
}

bool AssetImporter::LoadEngineFile(const std::string& filePath)
{
    return false;
}

bool AssetImporter::SaveToEngineFile(const std::string& filePath)
{
    return false;
}

void AssetImporter::Clear()
{
    DV_DELETE(s_pModel);
}

std::string AssetImporter::GetFilePath()
{
    return s_FilePath;
}

Dive::Model* AssetImporter::GetModel()
{
    return s_pModel;
}

// BoneInfoMap���� ��ϵǾ�����
// �Ʒ��� �������� �������� ���� Bone�� �����Ѵ�.
// �̴� �������� ReadMissingBones�� �ݴ� ��Ȳ�̴�.
void AssetImporter::CollectAnimations()
{
    auto numAnims = s_pScene->mNumAnimations;
    for(uint32_t i = 0; i < numAnims; ++i)
    {
        auto pAnim = s_pScene->mAnimations[i];
        auto pAnimation = new Dive::Animation();
        pAnimation->SetName("TestAnim");//pAnim->mName.data);
        pAnimation->SetTickPerSecond(static_cast<float>(pAnim->mTicksPerSecond));
        pAnimation->SetDuration(static_cast<float>(pAnim->mDuration));
        Dive::ResourceCache::AddManualResource<Dive::Animation>(pAnimation);

        auto numChannels = pAnim->mNumChannels; 
        for (uint32_t j = 0; j < numChannels; ++j)
        {
            auto pChannel = pAnim->mChannels[j];
            Dive::Bone bone(pChannel->mNodeName.C_Str());

            for (uint32_t index = 0; index < pChannel->mNumPositionKeys; ++index)
            {
                auto key = pChannel->mPositionKeys[index];               
                bone.InsertPositionKey(static_cast<float>(key.mTime), ConvertXMFLOAT3(key.mValue));
            }

            for (uint32_t index = 0; index < pChannel->mNumRotationKeys; ++index)
            {
                auto key = pChannel->mRotationKeys[index];
                bone.InsertRotationKey(static_cast<float>(key.mTime), ConvertXMFLOAT4(key.mValue));
            }

            for (uint32_t index = 0; index < pChannel->mNumScalingKeys; ++index)
            {
                auto key = pChannel->mScalingKeys[index];
                bone.InsertScaleKey(static_cast<float>(key.mTime), ConvertXMFLOAT3(key.mValue));
            }

            pAnimation->InsertBone(bone);
        }
    }
}

void AssetImporter::loadMaterialTextures(aiMaterial* pMat, aiTextureType type, std::string typeName)
{
    for (UINT i = 0; i < pMat->GetTextureCount(type); ++i)
    {
        aiString texPath;
        pMat->GetTexture(type, i, &texPath);
    }
}
