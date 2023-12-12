#include "ModelImporter.h"

// ���� ������ ���ĸ�ź ������ ���̴�.
// �Ľ� �������� ���������� ���� ������Ʈ�� Ʈ������ ������Ʈ�� ����
// �޽ô� ���� ���� �����
// ���� ������Ʈ�� �޽� ������ ������Ʈ�� �޽ø� ���������μ�
// �ϳ��� �� ���� ������Ʈ�� �����Ǵ� ���̴�.
// �̶� ���� �� ���� ������Ʈ�� ��Ʈ ��带 �����Ѵ�.
// ������ �� ������ ��� �� ���� ������Ʈ�� ���� ������ �ʿ��ϰ�
// �ø�������� ���� ����������.

static DirectX::XMFLOAT3 ConvertXMFLOAT3(const aiVector3D& vec)
{
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

static DirectX::XMFLOAT4 ConvertXMFLOAT4(const aiQuaternion& vec)
{
    return DirectX::XMFLOAT4(vec.x, vec.y, vec.z, vec.w);
}

static DirectX::XMFLOAT4X4 ConvertXMFLOAT4X4(const aiMatrix4x4& mat)
{
    return DirectX::XMFLOAT4X4(
        mat.a1, mat.b1, mat.c1, mat.d1,
        mat.a2, mat.b2, mat.c2, mat.d2,
        mat.a3, mat.b3, mat.c3, mat.d3,
        mat.a4, mat.b4, mat.c4, mat.d4
    );
}

static void GetBlendData(aiMesh* pMesh, std::vector<std::vector<int>>& outBlendIndices, std::vector<std::vector<float>>& outBlendWeights)
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

static Dive::GameObject* GetNodeObjectByBoneName(Dive::GameObject* pNodeObject, const std::string& boneName)
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

static Dive::GameObject* FindRootBone(Dive::GameObject* pNodeObject, const std::vector<Dive::BoneInfo>& boneInfos)
{
    for (const Dive::BoneInfo& boneInfo : boneInfos)
    {
        if (pNodeObject->GetName() == boneInfo.name)
            return pNodeObject;
    }

    auto pTransform = pNodeObject->GetTransform();
    for (auto pChild : pTransform->GetChildren())
    {
        auto pRootBone = FindRootBone(pChild->GetGameObject(), boneInfos);
        if (pRootBone)
            return pRootBone;
    }

    return nullptr;
}

ModelImporter::ModelImporter()
    : m_FileDirectory(),
    m_FileName(),
    m_ModelName(),
    m_pScene(nullptr),
    m_pImportedModel(nullptr)
{
}

bool ModelImporter::Load(const std::string& fileName)
{
    if (Dive::FileSystem::FileExists(fileName))
    {
        m_FileDirectory = fileName.substr(0, fileName.find_last_of("/\\"));
        m_FileName = fileName;
        m_ModelName = Dive::FileSystem::GetFileName(fileName);

        return LoadExternalFile(fileName);
    }

    return false;
}

bool ModelImporter::LoadExternalFile(const std::string& fileName)
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

    m_pScene = importer.ReadFile(fileName, flags);
    if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
    {
        DV_ERROR("{:s}", importer.GetErrorString());
        return false;
    }

    m_pImportedModel = new Dive::Model;
    m_pImportedModel->SetName(m_ModelName);

    processNode(m_pScene->mRootNode, nullptr);

    processAnimation();
    // skeleton���κ��� bones�� ������ �� BoneRenderer�� ����� SkinnedMeshRenderer�� bones�� �߰��ϴ� ����
    {
        auto pRootObject = m_pImportedModel->GetRootGameObject();
        auto skeleton = m_pImportedModel->GetSkeleton();
        const auto& boneInfos = skeleton.GetBones();

        // rootBone �˻� �� ����
        auto pRootBone = FindRootBone(pRootObject, boneInfos);
        m_pImportedModel->SetRootBone(pRootBone);

        // BoneRenderer�� ��� �߰�
        for (uint32_t i = 0; i < static_cast<uint32_t>(boneInfos.size()); ++i)
        {
            Dive::GameObject* pNodeObject = GetNodeObjectByBoneName(pRootObject, boneInfos[i].name);
            if (!pNodeObject)
            {
                DV_ERROR("{:s} GameObject�� �������� �ʽ��ϴ�.", boneInfos[i].name.c_str());
                continue;
            }
            pNodeObject->AddComponent<Dive::BoneRenderer>();
        }

        // SkinnedMeshRenderer�� rootBone ����
        // Animaition�� TestAnim ����
        std::function<void(Dive::GameObject*)> func = [&func, &pRootBone](Dive::GameObject* pNode)
            {
                if (!pNode)
                    return;

                auto pSkinnedMeshRenderer = pNode->GetComponent<Dive::SkinnedMeshRenderer>();
                if (pSkinnedMeshRenderer)
                    pSkinnedMeshRenderer->SetRootBone(pRootBone);

                auto pAnimator = pNode->GetComponent<Dive::Animator>();
                if (pAnimator)
                    pAnimator->SetAnimation(Dive::ResourceCache::GetResourceByName<Dive::Animation>("TestAnim"));

                auto pTransform = pNode->GetTransform();
                for (auto pChild : pTransform->GetChildren())
                    func(pChild->GetGameObject());
            };

        func(pRootObject);
    }
    m_pImportedModel->BuildMeshBuffers();

    return true;
}

bool ModelImporter::LoadEngineFile(const std::string& fileName)
{
    return false;
}

void ModelImporter::Clear()
{
    DV_DELETE(m_pImportedModel);
}

void ModelImporter::processNode(aiNode* pNode, Dive::Transform* pParent)
{
    auto pNodeObject = Dive::Scene::CreateGameObject();
    pNodeObject->SetName(pParent ? pNode->mName.data : m_ModelName);
    auto pNodeTransform = pNodeObject->GetTransform();
    pNodeTransform->SetParent(pParent);
    pNodeTransform->SetLocalMatrix(ConvertXMFLOAT4X4(pNode->mTransformation));

    if (!pParent)
        m_pImportedModel->SetRootGameObject(pNodeObject);

    for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
    {
        auto pMeshNodeObject = pNodeObject;
        auto pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];
        std::string meshName = pMesh->mName.C_Str();

        if (i > 0)
        {
            pMeshNodeObject = Dive::Scene::CreateGameObject();
            pMeshNodeObject->GetTransform()->SetParent(pNodeObject->GetTransform()->GetParent());
            pMeshNodeObject->GetTransform()->SetLocalMatrix(ConvertXMFLOAT4X4(pNode->mTransformation));

            // ������ �̸��� ������ ��찡 �־� �ε����� �߰�
            // ���� �̸���� ���ϴ� ����� �õ��غ���.
            meshName += "_" + std::to_string(i + 1);
        }
        pMeshNodeObject->SetName(meshName);

        processMesh(pMesh, pMeshNodeObject);
    }

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        processNode(pNode->mChildren[i], pNodeTransform);
}

void ModelImporter::processMesh(aiMesh* pMesh, Dive::GameObject* pMeshNodeObject)
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

        // �ֽŹ��� ���ĸ�ź�� ��� �޽ø� �ϳ��� �ٷ��.
        // ���������� �ڽ��� �׸� �޽��� �����°� ������ �����ϴ� ����̴�.
        // �̷��� ������ ��� ���� �ε��ϸ� ��� ������Ʈ�� �ϳ��� �޽÷� ������� ������.
        // �׸��� ����ƽ�� ��Ų�� �޽ø� �����Ͽ� �����ϱ⵵ ���������.
        // ������ �������Ϳ��� ���� �ε��ϴ°� ���� ���� ����ϱ�?
        auto pStaticMesh = m_pImportedModel->InsertStaticMesh(new Dive::StaticMesh(pMesh->mName.C_Str(), vertices, indices));
        auto pMeshRenderer = pMeshNodeObject->AddComponent<Dive::MeshRenderer>();
        pMeshRenderer->SetMesh(pStaticMesh);
        pMeshRenderer->SetMaterial(loadMaterial(pMesh));
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

        // ���� �ϳ��� ��忡 �� �̻��� �޽ð� ������ ���
        // �޽ø� �߰��� �����ϰ� �ִ�.
        // �̶� �Ʒ��� ������ �Բ� ����Ǵµ�
        // ���� Model�� �ϳ��� Skeleton�� ���� �� �ִ�.
        Dive::Skeleton skeleton;
        auto& bones = skeleton.GetModifiableBones();
        bones.resize(pMesh->mNumBones);
        // ���� ���̷����� �� �� �̻��� �� ������ model���� �ϳ��� �����Ѵ�.
        // ���� �ƿ� ������ó�� model���� �ϳ��� ������� �� �����̴�.
        auto& boneInfoMap = m_pImportedModel->GetBoneInfoMap();

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
        m_pImportedModel->SetSkeleton(skeleton);

        auto pSkinnedMesh = new Dive::SkinnedMesh(pMeshNodeObject->GetName(), vertices, indices);
        m_pImportedModel->InsertSkinnedMesh(pSkinnedMesh);

        // �̺κп��� ������ �� ���� �ִ�. ������ Ư���� ��ġ���� ��������. �����ϴ�.
        auto pMeshRenderer = pMeshNodeObject->AddComponent<Dive::SkinnedMeshRenderer>();
        pMeshRenderer->SetMesh(pSkinnedMesh);
        pMeshRenderer->SetMaterial(loadMaterial(pMesh));

        // temp: �� ������ SkinnedMeshRenderer�� BoneInfo�� ������ �� �ִ�.
        pMeshRenderer->SetBones(bones);

        // temp
        auto pAnimator = pMeshNodeObject->AddComponent<Dive::Animator>();
        // ���� �ִϸ��̼��� �������� �ʾҴ�.
        //pAnimator->SetAnimation(Dive::ResourceCache::GetResourceByName<Dive::Animation>("TestAnim"));
    }
}

Dive::Material* ModelImporter::loadMaterial(aiMesh* pMesh)
{
    auto pMaterial = m_pScene->mMaterials[pMesh->mMaterialIndex];
    if (!pMaterial)
    {
        // �׷��� Ȥ�� ���������� ������ ����Ʈ ��Ƽ������ �����;� �Ѵ�.
        return nullptr;
    }

    aiString name;
    aiGetMaterialString(pMaterial, AI_MATKEY_NAME, &name);

    aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
    aiGetMaterialColor(pMaterial, AI_MATKEY_OPACITY, &opacity);

    // textures
    aiString texturePath;
    if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
        pMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);

    Dive::Texture2D* pDiffuseTex = nullptr;
    const aiTexture* pEmbeddedTexture = m_pScene->GetEmbeddedTexture(texturePath.C_Str());
    if (pEmbeddedTexture)
    {
        // ���� ���ο��� �����͸� ���� ���� �����
        pDiffuseTex = loadEmbeddedTexture(pEmbeddedTexture);
    }
    else
    {
        // �̰� �ܺο� �����ϴ� ������ �ε�
        auto diffuseTexturePath = Dive::FileSystem::GetPath(m_FileName) + "textures/";
        diffuseTexturePath += Dive::FileSystem::GetFileNameAndExtension(texturePath.C_Str());
        pDiffuseTex = Dive::ResourceCache::GetResourceByPath<Dive::Texture2D>(diffuseTexturePath);
    }

    auto pDvMat = new Dive::Material();
    pDvMat->SetName(name.data);
    pDvMat->SetDiffuseColor(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
    pDvMat->SetTexture(Dive::eTextureUnit::Diffuse, pDiffuseTex);
    Dive::ResourceCache::AddManualResource<Dive::Material>(pDvMat);

    return pDvMat;
}

// ���������� ID3D11ShaderResourceView�� ������ �� �����Ѵ�.
// https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedDirectx11/SimpleTexturedDirectx11/ModelLoader.cpp
Dive::Texture2D* ModelImporter::loadEmbeddedTexture(const aiTexture* pEmbeddedTexture)
{
    auto pDvTexture = new Dive::Texture2D();

    if (pEmbeddedTexture->mHeight != 0)
    {
        pDvTexture->SetSize(pEmbeddedTexture->mWidth, pEmbeddedTexture->mHeight);
        pDvTexture->SetRawTextureData((const void*)pEmbeddedTexture->pcData);
        // �� �� �����ѵ� �� ���� ���ļ� �����ϴ°� ������ �ɸ���.
        // => urho3d�� ������ �������̽���. 
    }
    const size_t size = pEmbeddedTexture->mWidth;
    const void* pSrcData = (const void*)pEmbeddedTexture->pcData;
    Dive::Image img;
    if (!img.LoadFromMemory(Dive::FileSystem::GetExtension(pEmbeddedTexture->mFilename.C_Str()), size, pSrcData))
        return nullptr;
    pDvTexture->SetImage(&img);

    // �Ŵ��� ���
    pDvTexture->SetName(Dive::FileSystem::GetFileNameAndExtension(pEmbeddedTexture->mFilename.C_Str()));
    Dive::ResourceCache::AddManualResource<Dive::Texture2D>(pDvTexture);

    return pDvTexture;
}

void ModelImporter::processAnimation()
{
    auto numAnims = m_pScene->mNumAnimations;
    for (uint32_t i = 0; i < numAnims; ++i)
    {
        auto pAnim = m_pScene->mAnimations[i];
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
