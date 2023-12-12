#include "ModelImporter.h"

// 현재 구현은 스파르탄 구버전 모델이다.
// 파싱 과정에서 계층구조는 게임 오브젝트의 트랜스폼 컴포넌트를 통해
// 메시는 모델을 통해 만들고
// 게임 오브젝트의 메시 렌더러 컴포넌트에 메시를 저장함으로서
// 하나의 모델 게임 오브젝트가 구성되는 것이다.
// 이때 모델이 모델 게임 오브젝트의 루트 노드를 관리한다.
// 문제는 이 구현의 경우 모델 게임 오브젝트의 복사 구현이 필요하고
// 시리얼라이즈 역시 복잡해진다.

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

    // outBlendWeights를 정규화
    for (uint32_t i = 0; i < static_cast<uint32_t>(outBlendWeights.size()); ++i)
    {
        // 1. 뼈대가 4개를 초과할 경우 가장 작은 wegiht와 bone을 제거
        if (outBlendWeights[i].size() > 4)
        {
            DV_WARN("{:d}번째 정점에 영향을 주는 뼈대가 4개 이상입니다.", i);

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

        // 2. 총 합이 1.0 이상이거나 0.0이 아닐 경우 정규화
        float sum = 0.0f;
        for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
            sum += outBlendWeights[i][j];
        if (sum != 1.0f && sum != 0.0f)
        {
            DV_WARN("Weight 총합이 1.0이 아니어서 정규화를 수행");

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

    // 이게 빈 노드들을 제거해주었다.
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
    // skeleton으로부터 bones를 가져온 후 BoneRenderer를 만들고 SkinnedMeshRenderer에 bones를 추가하는 과정
    {
        auto pRootObject = m_pImportedModel->GetRootGameObject();
        auto skeleton = m_pImportedModel->GetSkeleton();
        const auto& boneInfos = skeleton.GetBones();

        // rootBone 검색 후 저장
        auto pRootBone = FindRootBone(pRootObject, boneInfos);
        m_pImportedModel->SetRootBone(pRootBone);

        // BoneRenderer를 모두 추가
        for (uint32_t i = 0; i < static_cast<uint32_t>(boneInfos.size()); ++i)
        {
            Dive::GameObject* pNodeObject = GetNodeObjectByBoneName(pRootObject, boneInfos[i].name);
            if (!pNodeObject)
            {
                DV_ERROR("{:s} GameObject가 존재하지 않습니다.", boneInfos[i].name.c_str());
                continue;
            }
            pNodeObject->AddComponent<Dive::BoneRenderer>();
        }

        // SkinnedMeshRenderer에 rootBone 저장
        // Animaition에 TestAnim 저장
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

            // 동일한 이름을 가지는 경우가 있어 인덱스를 추가
            // 이전 이름들과 비교하는 방법도 시도해보자.
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

        // 최신버전 스파르탄의 경우 메시를 하나만 다룬다.
        // 렌더러블은 자신이 그릴 메시의 오프셋과 개수를 저장하는 방식이다.
        // 이렇게 구현할 경우 씬을 로드하면 모든 오브젝트가 하나의 메시로 만들어져 버린다.
        // 그리고 스태틱과 스킨드 메시를 구분하여 관리하기도 힘들어진다.
        // 하지만 모델임포터에서 씬을 로드하는게 과연 옳은 사용일까?
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

        // 현재 하나의 노드에 둘 이상의 메시가 존재할 경우
        // 메시를 추가로 생성하고 있다.
        // 이때 아래의 과정도 함께 수행되는데
        // 정작 Model은 하나의 Skeleton만 가질 수 있다.
        Dive::Skeleton skeleton;
        auto& bones = skeleton.GetModifiableBones();
        bones.resize(pMesh->mNumBones);
        // 현재 스켈레톤이 두 개 이상일 수 있지만 model에서 하나만 관리한다.
        // 따라서 아에 참고문서처럼 model에서 하나만 관리토록 할 생각이다.
        auto& boneInfoMap = m_pImportedModel->GetBoneInfoMap();

        // 현재 메시가 사용하는 뼈대의 정보를 모아서 관리하는 것이다.
        for (uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
        {
            aiBone* pBone = pMesh->mBones[boneIndex];
            bones[boneIndex].name = pBone->mName.C_Str();
            bones[boneIndex].index = boneIndex;
            bones[boneIndex].offsetMatrix = ConvertXMFLOAT4X4(pBone->mOffsetMatrix);

            // id는... 문서를 좀 더 보자.
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

        // 이부분에서 오류가 난 적이 있다. 하지만 특별한 조치없이 고쳐졌다. 찝찝하다.
        auto pMeshRenderer = pMeshNodeObject->AddComponent<Dive::SkinnedMeshRenderer>();
        pMeshRenderer->SetMesh(pSkinnedMesh);
        pMeshRenderer->SetMaterial(loadMaterial(pMesh));

        // temp: 이 곳에서 SkinnedMeshRenderer에 BoneInfo를 전달할 수 있다.
        pMeshRenderer->SetBones(bones);

        // temp
        auto pAnimator = pMeshNodeObject->AddComponent<Dive::Animator>();
        // 아직 애니메이션이 구성되지 않았다.
        //pAnimator->SetAnimation(Dive::ResourceCache::GetResourceByName<Dive::Animation>("TestAnim"));
    }
}

Dive::Material* ModelImporter::loadMaterial(aiMesh* pMesh)
{
    auto pMaterial = m_pScene->mMaterials[pMesh->mMaterialIndex];
    if (!pMaterial)
    {
        // 그래픽 혹은 렌더러에서 생성한 디폴트 머티리얼을 가져와야 한다.
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
        // 파일 내부에서 데이터를 얻어와 직접 만들기
        pDiffuseTex = loadEmbeddedTexture(pEmbeddedTexture);
    }
    else
    {
        // 이건 외부에 존재하는 파일을 로드
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

// 예제에서는 ID3D11ShaderResourceView를 생성한 후 리턴한다.
// https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedDirectx11/SimpleTexturedDirectx11/ModelLoader.cpp
Dive::Texture2D* ModelImporter::loadEmbeddedTexture(const aiTexture* pEmbeddedTexture)
{
    auto pDvTexture = new Dive::Texture2D();

    if (pEmbeddedTexture->mHeight != 0)
    {
        pDvTexture->SetSize(pEmbeddedTexture->mWidth, pEmbeddedTexture->mHeight);
        pDvTexture->SetRawTextureData((const void*)pEmbeddedTexture->pcData);
        // 될 것 같긴한데 두 번에 걸쳐서 생성하는게 마음에 걸린다.
        // => urho3d를 참고한 인터페이스다. 
    }
    const size_t size = pEmbeddedTexture->mWidth;
    const void* pSrcData = (const void*)pEmbeddedTexture->pcData;
    Dive::Image img;
    if (!img.LoadFromMemory(Dive::FileSystem::GetExtension(pEmbeddedTexture->mFilename.C_Str()), size, pSrcData))
        return nullptr;
    pDvTexture->SetImage(&img);

    // 매뉴얼 등록
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
