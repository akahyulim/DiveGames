#include "ModelImporter.h"
#include "DiveEngine.h"

static const unsigned int MAX_CHANNELS = 4;

static DirectX::XMFLOAT3 aiVector3DToFloat3(const aiVector3D& vec)
{
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

static DirectX::XMFLOAT4 aiColor4DToFloat4(const aiColor4D& col)
{
    return DirectX::XMFLOAT4(col.r, col.g, col.b, col.a);
}

static DirectX::XMVECTOR ToVector(const aiVector3D& vec)
{
    DirectX::XMFLOAT3 float3 = aiVector3DToFloat3(vec);
    return DirectX::XMLoadFloat3(&float3);
}

static std::vector<Dive::VertexElement> GetVertexType(aiMesh* pMesh)
{
    std::vector<Dive::VertexElement> ret;

    // position
    ret.emplace_back(Dive::eVertexElementType::Float3, Dive::eVertexElementSemantic::Position);

    // normal
    if (pMesh->HasNormals())
    {
        ret.emplace_back(Dive::eVertexElementType::Float3, Dive::eVertexElementSemantic::Normal);
    }

    // color
    for (unsigned int i = 0; i < pMesh->GetNumColorChannels() && i < MAX_CHANNELS; ++i)
    {
        ret.emplace_back(Dive::eVertexElementType::Float4, Dive::eVertexElementSemantic::Color, i);
    }

    // texCoord
    for (unsigned int i = 0; i < pMesh->GetNumUVChannels() && i < MAX_CHANNELS; ++i)
    {
        ret.emplace_back(Dive::eVertexElementType::Float2, Dive::eVertexElementSemantic::TexCoord, i);
    }

    // tangent
    if (pMesh->HasTangentsAndBitangents())
    {
        ret.emplace_back(Dive::eVertexElementType::Float3, Dive::eVertexElementSemantic::Tangent);
    }

    // skinned
    {
        // weight
        ret.emplace_back(Dive::eVertexElementType::Float4, Dive::eVertexElementSemantic::BlendWeight);

        // bone index
        ret.emplace_back(Dive::eVertexElementType::UByte4, Dive::eVertexElementSemantic::MaxBlendIndex);
    }

    return ret;
}

static unsigned int GetBoneIndex(OutModel& model, const std::string& boneName)
{
    for (unsigned int i = 0; i < model.bones.size(); ++i)
    {
        if (boneName == model.bones[i]->mName.C_Str())
            return i;
    }

    return 0xffffffff;
}

static void GetBlendData(OutModel& model, aiMesh* pMesh, aiNode* pMeshNode, std::vector<unsigned int>& boneMappings,
    std::vector<std::vector<unsigned char>>& blendIndices, std::vector<std::vector<float>>& blendWeights)
{
    blendIndices.resize(pMesh->mNumVertices);
    blendWeights.resize(pMesh->mNumVertices);
    boneMappings.clear();

    if (model.bones.size() > 64)
    {
        if (pMesh->mNumBones > 64)
        {
            // error
            return;
        }

        if (pMesh->mNumBones > 0)
        {
            boneMappings.resize(pMesh->mNumBones);
            for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
            {
                aiBone* pBone = pMesh->mBones[i];
                std::string boneName = pBone->mName.C_Str();
                unsigned int globalIndex = GetBoneIndex(model, boneName);
                if (globalIndex == 0XFFFFFFFF)
                    DV_APP_ERROR("뼈대({:s})(을)를 찾지 못하였습니다. ", boneName);

                boneMappings[i] = globalIndex;

                for (unsigned int j = 0; j < pBone->mNumWeights; ++j)
                {
                    unsigned int vertex = pBone->mWeights[j].mVertexId;
                    blendIndices[vertex].emplace_back(globalIndex);
                    blendWeights[vertex].emplace_back(pBone->mWeights[j].mWeight);
                }
            }
        }
        else
        {

        }
    }
    else
    {
        if (pMesh->mNumBones > 0)
        {
            for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
            {
                aiBone* pBone = pMesh->mBones[i];
                std::string boneName = pBone->mName.C_Str();
                unsigned int globalIndex = GetBoneIndex(model, boneName);
                if (globalIndex == 0XFFFFFFFF)
                    DV_APP_ERROR("뼈대({:s})(을)를 찾지 못하였습니다. ", boneName);
                
                for (unsigned int j = 0; j < pBone->mNumWeights; ++j)
                {
                    unsigned int vertex = pBone->mWeights[j].mVertexId;
                    blendIndices[vertex].emplace_back(globalIndex);
                    blendWeights[vertex].emplace_back(pBone->mWeights[j].mWeight);
                }
            }
        }
        else
        {

        }
    }
}

// 일단 bounding box는 제외
static void WriteVertices(float*& dest, aiMesh* pMesh, unsigned int index, bool bSkinned, const DirectX::XMFLOAT4X4& vertexTransform,
    const DirectX::XMFLOAT4X4& normalTransform, std::vector<std::vector<unsigned char>>& blendIndices, std::vector<std::vector<float>>& blendWeights)
{
    // vertex
    // 변환시킨 후 넣어야 한다.
    auto vertex = aiVector3DToFloat3(pMesh->mVertices[index]);
    *dest++ = vertex.x;
    *dest++ = vertex.y;
    *dest++ = vertex.z;

    // normal
    if (pMesh->HasNormals())
    {
        // 역시 변환 후 넣어야 한다.
        auto normal = aiVector3DToFloat3(pMesh->mNormals[index]);
        *dest++ = normal.x;
        *dest++ = normal.y;
        *dest++ = normal.z;
    }

    // color
    // 현재 대부분 값이 없다.
    for(unsigned int i = 0; i < pMesh->GetNumColorChannels() && i < MAX_CHANNELS; ++i)
    {
        auto color = aiColor4DToFloat4(pMesh->mColors[i][index]);
        *dest++ = color.x;
        *dest++ = color.y;
        *dest++ = color.z;
        *dest++ = color.w;
    }

    // texCoord
    for (unsigned int i = 0; i < pMesh->GetNumUVChannels() && i < MAX_CHANNELS; ++i)
    {
        // 예전에 값이 엉뚱해서 직접 수정했다.
        auto texCoord = aiVector3DToFloat3(pMesh->mTextureCoords[i][index]);
        *dest++ = texCoord.x;
        *dest++ = texCoord.y;
    }

    // tangent
    if (pMesh->HasTangentsAndBitangents())
    {
        // 여기도 NormalTransform으로 변환
        auto tangent = aiVector3DToFloat3(pMesh->mTangents[index]);
        *dest++ = tangent.x;
        *dest++ = tangent.y;
        *dest++ = tangent.z;
    }
 
    if (bSkinned)
    {
        // weights
        for (unsigned int i = 0; i < 4; ++i)
        {
            if (i < blendWeights[index].size())
                *dest++ = blendWeights[index][i];
            else
                *dest++ = 0.0f;
        }

        // indice
        auto* pDestBytes = (unsigned char*)dest;
        ++dest; // 이건 뭐지?
        for (unsigned int i = 0; i < 4; ++i)
        {
            if (i < blendIndices[index].size())
                *pDestBytes++ = blendIndices[index][i];
            else
                *pDestBytes++ = 0;
        }
    }
}

static void WriteShortIndices(unsigned short*& dest, aiMesh* pMesh, unsigned int index, unsigned int offset)
{
    if (pMesh->mFaces[index].mNumIndices == 3)
    {
        *dest++ = pMesh->mFaces[index].mIndices[0] + offset;
        *dest++ = pMesh->mFaces[index].mIndices[1] + offset;
        *dest++ = pMesh->mFaces[index].mIndices[2] + offset;
    }
}

static void WriteLargeIndices(unsigned long*& dest, aiMesh* pMesh, unsigned int index, unsigned int offset)
{
    if (pMesh->mFaces[index].mNumIndices == 3)
    {
        *dest++ = pMesh->mFaces[index].mIndices[0] + offset;
        *dest++ = pMesh->mFaces[index].mIndices[1] + offset;
        *dest++ = pMesh->mFaces[index].mIndices[2] + offset;
    }
}

static unsigned int GetNumValidFaces(const aiMesh* pMesh)
{
    if (!pMesh)
        return 0;

    unsigned int ret = 0;

    for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
    {
        if (pMesh->mFaces[i].mNumIndices == 3)
            ++ret;
    }

    return ret;
}

static aiNode* GetNode(aiNode* pRootNode, std::string& name)
{
    if (!pRootNode || name.empty())
        return nullptr;

    if (name.compare(pRootNode->mName.C_Str()))
        return pRootNode;

    for (size_t i = 0; i < pRootNode->mNumChildren; i++)
    {
        auto pFound = GetNode(pRootNode->mChildren[i], name);
        if (pFound)
            return pFound;
    }

    return nullptr;
}

static aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* pNode, aiNode* pRootNode, bool bRootInclusive)
{
    while (pNode && pNode != pRootNode)
    {
        pNode = pNode->mParent;

        if (!bRootInclusive && pNode == pRootNode)
            break;

        if (pNode)
            transform = pNode->mTransformation * transform;
    }

    return transform;
}

static aiMatrix4x4 GetDerivedTransform(aiNode* pNode, aiNode* pRootNode, bool bRootInclusive = true)
{
    return GetDerivedTransform(pNode->mTransformation, pNode, pRootNode, bRootInclusive);
}

static aiMatrix4x4 GetMeshBakingTransform(aiNode* pMeshNode, aiNode* pRootNode)
{
    if (pMeshNode == pRootNode)
        return {};
    else
        return GetDerivedTransform(pMeshNode, pRootNode);
}

static void GetPosRotScale(const aiMatrix4x4& transform, DirectX::XMFLOAT3& outPos, DirectX::XMFLOAT4& outRot, DirectX::XMFLOAT3& outScale)
{
    aiVector3D aiPos;
    aiQuaternion aiRot;
    aiVector3D aiScale;

    transform.Decompose(aiScale, aiRot, aiPos);

    outPos = DirectX::XMFLOAT3(aiPos.x, aiPos.y, aiPos.z);
    outRot = DirectX::XMFLOAT4(aiRot.x, aiRot.y, aiRot.z, aiRot.w);
    outScale = DirectX::XMFLOAT3(aiScale.x, aiScale.y, aiScale.z);
}

// 아직 불완전한 함수다.
static std::string GetMeshMaterialName(const aiScene* pScene, const aiMesh* pMesh)
{
    if (!pScene || !pMesh)
        return std::string();

    auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
    aiString matNameStr;
    // 이게 뭔지 모르겠다.
    pMaterial->Get(AI_MATKEY_NAME, matNameStr);

    // 참고코드는 .xml을 붙였다.
    return matNameStr.C_Str();
}

// 현재 Engine에서 include한 dll 버전을 사용하고 있다.
// 추후 이를 옮기고, 왠만하면 static lib 버전으로 수정하고 싶다. 
bool DvModelImporter::LoadFromFile(const std::string& filepath)
{
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

    if (m_pScene = (aiScene*)importer.ReadFile(filepath, flags))
    {
        m_bAnimations = m_pScene->HasAnimations();
        m_bMaterials = m_pScene->HasMaterials();

        aiNode* pRootNode = m_pScene->mRootNode;

        // 이름을 만든 후 호출, urho는 콘솔 커맨드에 추출파일 이름이 포함된다.
        // 에디터에서 이름을 전달받는 것이 맞는 듯 하다.
        exportModel(pRootNode, "");
        // export animation
        // export material

        // 2번째 이상부터 로드가 먹통되는 현상이 발생한다.
        // 그런데 디버그 모드로 실행시엔 괜찮다...
        importer.FreeScene();
    }

	return true;
}

void DvModelImporter::exportModel(aiNode* pRootNode, const std::string& outName)
{
    OutModel model;
    model.pRootNode = pRootNode;
    model.outName = outName;

    collectMeshes(model, model.pRootNode);
    //collectBones(model);
    // build bone collision info
    buildAndSaveModel(model);
}

// 루트노드부터 재귀적으로 모든 메시를 aiMesh에 저장한다.
void DvModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
    {
        aiMesh* pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

        // mong은 여기에서 2개가 걸린다.
        for (unsigned int j = 0; j < model.meshes.size(); ++j)
        {
            if (pMesh == model.meshes[j])
            {
                DV_APP_WARN("동일한 메시가 발견되었습니다.");
                break;
            }
        }

        model.meshIndices.emplace_back(pNode->mMeshes[i]);
        model.meshes.emplace_back(pMesh);
        model.meshNodes.emplace_back(pNode);
        model.totalNumVertices += pMesh->mNumVertices;
        model.totalNumIndices += pMesh->mNumFaces * 3;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
    {
        collectMeshes(model, pNode->mChildren[i]);
    }
}

void DvModelImporter::collectBones(OutModel& model)
{
    std::unordered_set<aiNode*> necessary;
    std::unordered_set<aiNode*> rootNodes;

    bool skinnedMesh = false;
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        if (model.meshes[i]->HasBones())
        {
            skinnedMesh = true;
            break;
        }
    }

    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        aiMesh* pMesh = model.meshes[i];
        aiNode* pMeshNode = model.meshNodes[i];
        aiNode* pMeshNodeParent = pMeshNode->mParent;
        aiNode* pRootNode = nullptr;

        // 1. Mesh의 BoneNode를 찾아 necessary에 저장
        // 2. BoneNode의 RootNode를 찾아 rootNodes에 저장
        for (size_t j = 0; j < pMesh->mNumBones; j++)
        {
            aiBone* pBone = pMesh->mBones[j];
            std::string boneName = pBone->mName.C_Str();
            // aiBone에 mNode가 있다?
            aiNode* pBoneNode = GetNode(m_pScene->mRootNode, boneName);
            // 결과는 다르다고 나온다...?
            //DV_APP_INFO("bone node comparison ? {:s}", pBoneNode == pBone->mNode ? "true" : " false");
            
            if (!pBoneNode)
            {
                DV_APP_ERROR("BondeNode({:s})를 찾지 못하였습니다.", boneName);
                return;
            }
            necessary.insert(pBoneNode);
            
            pRootNode = pBoneNode;
            while (1)
            {
                pBoneNode = pBoneNode->mParent;
                if (!pBoneNode || (pBoneNode == pMeshNode) || (pBoneNode == pMeshNodeParent))
                    break;
                
                pRootNode = pBoneNode;
                necessary.insert(pBoneNode);
            }
            if (rootNodes.find(pRootNode) == rootNodes.end())
            {
                rootNodes.insert(pRootNode);
            }
        }

        // 현재 메시에 뼈대가 없는 경우
        // 일부분이 static mesh인 경우 같다.
        if (skinnedMesh && !pMesh->mNumBones)
        {
            aiNode* pBoneNode = pMeshNode;
            necessary.insert(pBoneNode);
            pRootNode = pBoneNode;

            while (1)
            {
                pBoneNode = pBoneNode->mParent;
                if (!pBoneNode || (pBoneNode == pMeshNode) || (pBoneNode == pMeshNodeParent))
                    break;

                pRootNode = pBoneNode;
                necessary.insert(pBoneNode);
            }
            if (rootNodes.find(pRootNode) == rootNodes.end())
            {
                rootNodes.insert(pRootNode);
            }
        }
    }

    // 하나여야만 하나 보다.
    // 따라서 2개 이상일 경우 공통 부모를 찾아 조율하는 듯
    if (rootNodes.size() > 1)
    {
        // 일단 진행을 위해 비어놓는다.
        DV_APP_INFO("root node count: {:d}", rootNodes.size());
    }

    if (rootNodes.empty())
        return;

    model.pRootNode = *rootNodes.begin();

    DV_APP_INFO("IsSkinned? {:s}", skinnedMesh ? "true" : "false");
}

void DvModelImporter::buildAndSaveModel(OutModel& model)
{
    if (!model.pRootNode)
    {
        DV_APP_ERROR("모델의 루트 노드가 존재하지 않아 파일 저장을 중지합니다.");
        return;
    }

    if (model.meshes.empty())
    {
        DV_APP_ERROR("모델의 메시가 존재하지 않아 파일 저장을 중지합니다.");
        return;
    }

    Dive::DvModel* pModel = new Dive::DvModel;
    DV_ASSERT(pModel);
    
    unsigned int meshCount = 0;
    bool bCombineBuffers = true;
    auto vertexType = GetVertexType(model.meshes[0]);
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        if (GetNumValidFaces(model.meshes[i]))
        {
            ++meshCount;

            // static과 skinned가 함께 있을 경우 개별 버퍼 구성 대상
            if (i > 0 && vertexType != GetVertexType(model.meshes[i]))
                bCombineBuffers = false;
        }
    }

    if (bCombineBuffers && model.totalNumVertices > 65535)
    {
        bool bAllUnder65k = true;

        for (size_t i = 0; i < model.meshes.size(); ++i)
        {
            if (GetNumValidFaces(model.meshes[i]))
            {
                if (model.meshes[i]->mNumVertices > 65535)
                {
                    bAllUnder65k = false;
                    break;
                }
            }
        }

        if (bAllUnder65k == true)
            bCombineBuffers = false;
    }

    pModel->SetMeshCount(meshCount);

    Dive::DvVertexBuffer* pVertexBuffer = nullptr;
    Dive::DvIndexBuffer* pIndexBuffer = nullptr;
    std::vector<Dive::DvVertexBuffer*> vertexBuffers;
    std::vector<Dive::DvIndexBuffer*> indexBuffers;
    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0;
    unsigned int meshIndex = 0;
    bool bSkinned = model.bones.size() > 0;

    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        aiMesh* pMesh = model.meshes[i];
        auto vertexType = GetVertexType(model.meshes[i]);
        unsigned int validFaces = GetNumValidFaces(pMesh);
        if (!validFaces)
            continue;

        bool bLargeIndices;
        if (bCombineBuffers)
            bLargeIndices = model.totalNumIndices > 65535;
        else
            bLargeIndices = pMesh->mNumVertices > 65535;

        // 통짜 버퍼일 경우 최초 한 번만 생성
        // 아닐 경우 매 메시마다 버퍼 생성
        if (!bCombineBuffers || vertexBuffers.empty())
        {
            pVertexBuffer = new Dive::DvVertexBuffer;
            pIndexBuffer = new Dive::DvIndexBuffer;

            // 각각의 버퍼 크기 설정
            // 이 과정에서 버퍼 구성까지 한다. 그런데 리소스 설정이 에바다.
            // 만약 로드 후 바로 사용할 생각이라면 버퍼를 완벽하게 구성해야 한다.
            // 하지만 urho 역시 Model을 Cache로 생성하진 않았다.
            if (bCombineBuffers)
            {
                pVertexBuffer->SetSize(model.totalNumVertices, vertexType);
                pIndexBuffer->SetSize(model.totalNumIndices, bLargeIndices);
            }
            else
            {
                pVertexBuffer->SetSize(pMesh->mNumVertices, vertexType);
                pIndexBuffer->SetSize(validFaces * 3, bLargeIndices);
            }

            vertexBuffers.emplace_back(pVertexBuffer);
            indexBuffers.emplace_back(pIndexBuffer);
            vertexOffset = 0;
            indexOffset = 0;
        }

        // 계층구조 정보: 부모를 저장하는 것이 아니라 변환 행렬을 누적한다.
        // VertexTransform, NormalTransform과 pos, rot, scale이 대상
        // => 저장 대상은 Transform이다. 굳이 pos, rot, scale을 따로 계산할 필요가 있나...?
        DirectX::XMFLOAT3 pos, scale;
        DirectX::XMFLOAT4 rot;
        GetPosRotScale(GetMeshBakingTransform(model.meshNodes[i], model.pRootNode), pos, rot, scale);

        // vertex, normal transform을 계산한다. 일단 normal transform은 제외
        DirectX::XMFLOAT4X4 vertexTransform;
        DirectX::XMStoreFloat4x4(&vertexTransform, 
            (DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale))
                * DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rot))
                * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos))));

        // 앞서 동일한 변수명을 사용했다...
        // urho의 경우 Geomenty다.
        auto pModelMesh = new Dive::DvMesh;
        DV_ASSERT(pModelMesh);

        // 각각 앞서 생성한 버퍼로부터 시작 지점을 가리킨다.
        // 버퍼 자체는 크기만 할당되어 있다. 그래야 데이터를 집어넣으니깐
        unsigned char* pVertexData = pVertexBuffer->GetData();
        auto pIndexData = pIndexBuffer->GetData();

        // 2. build vertex 저장
        std::vector<std::vector<unsigned char>> blendIndices;
        std::vector<std::vector<float>> blendWeights;
        std::vector<unsigned int> boneMappings;
        if (!model.bones.empty())
            GetBlendData(model, pMesh, model.meshNodes[i], boneMappings, blendIndices, blendWeights);

        auto* pDest = (float*)((unsigned char*)pVertexData + vertexOffset * pVertexBuffer->GetVertexSize());
        for (size_t j = 0; j < pMesh->mNumVertices; ++j)
            WriteVertices(pDest, pMesh, (unsigned int)j, bSkinned, vertexTransform, DirectX::XMFLOAT4X4(), blendIndices, blendWeights);

        // 3. build index 저장
        // map / unmap이 아니다.
        if (bLargeIndices)
        {
            auto pDest = (unsigned long*)pIndexData + indexOffset;
            for (unsigned int j = 0; j < pMesh->mNumFaces; ++j)
                WriteLargeIndices(pDest, pMesh, j, vertexOffset);
        }
        else
        {
            auto pDest = (unsigned short*)pIndexData + indexOffset;
            for (unsigned int j = 0; j < pMesh->mNumFaces; ++j)
                WriteShortIndices(pDest, pMesh, j, vertexOffset);
        }

        // calculate center
        DirectX::XMFLOAT3 center{ 0.0f, 0.0f, 0.0f };
        if (validFaces)
        {
            for (unsigned int j = 0; j < pMesh->mNumFaces; ++j)
            {
                if (pMesh->mFaces[j].mNumIndices == 3)
                {
                    //center += aiVector3DToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[0]]);
                    //center += aiVector3DToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[1]]);
                    //center += aiVector3DToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[2]]);
                }
            }
        }


        // SetMesh(Geometry)
        // 이렇게 하면 통합버퍼의 경우 계속 동일한 버퍼를 저장하게 된다. 문제는 안될듯...?
        pModelMesh->SetVertexBuffer(0, pVertexBuffer); // 0으로 하면 결국 버퍼가 하나라는 소린데... 그렇다면 2개 이상으로 만들 필요도 없고...
        pModelMesh->SetIndexBuffer(pIndexBuffer);
        // DrawRange: draw에 필요한 offset, count를 저장
        // SetGeometryCenter
        pModel->SetMesh(meshIndex, pModelMesh);

        vertexOffset += pMesh->mNumVertices;
        indexOffset += validFaces * 3;
        ++meshIndex;
    }

    pModel->SetVertexBuffers(vertexBuffers); // 추후 다른 버퍼도 추가해야 할 듯?
    pModel->SetIndexBuffers(indexBuffers);
    // SetSkeleton

    // Save: 파일 생성 후 매개변수로 전달
    pModel->Save();

    // Materials
    //=> Material을 파일화하는 것이 아니라
    // Material 파일을 리스트화한 txt를 만드는 것 같다.
    // 실제로 ExportMaterial과 BuildAndSaveMaterial 함수가 따로 있다.
    // 이게 있어야 모델 파일만으로 Material에 접근할 수 있다는 건가...?
    if (m_bMaterials)
    {
        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            auto name = GetMeshMaterialName(m_pScene, model.meshes[i]);
        }
    }
}
