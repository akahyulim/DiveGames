#include "ModelImporter.h"
#include "DiveEngine.h"

// vertices offset�������� �����ϴ� �� �´�.
// ������
// 1. �ε��� ���� dest�� ������ �� ���� ũ���̰ų�
// 2. 
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

static void GetVertexElements(aiMesh* pMesh, bool bSkinned)
{
    if (!pMesh)
        return;

    std::vector<Dive::VertexElement> elements;

    // position: XMFLOAT3
    

    if (pMesh->HasNormals())
    {
        // XMFLOAT3
    }

    for (size_t i = 0; i < pMesh->GetNumColorChannels(); ++i)
    {
        // XMFLOAT4
    }

    for (size_t i = 0; i < pMesh->GetNumUVChannels(); ++i)
    {
        // XMFLOAT2
    }

    if (pMesh->HasTangentsAndBitangents())
    {
        // XMFLOAT4
    }

    if (bSkinned)
    {
        // weight: XMFLOAT4
        // index: XMUINT4??
    }
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

static DirectX::XMFLOAT3 ToFloat3(const aiVector3D& vec)
{
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

static DirectX::XMVECTOR ToVector(const aiVector3D& vec)
{
    DirectX::XMFLOAT3 float3 = ToFloat3(vec);
    return DirectX::XMLoadFloat3(&float3);
}

// ���� �ҿ����� �Լ���.
static std::string GetMeshMaterialName(const aiScene* pScene, const aiMesh* pMesh)
{
    if (!pScene || !pMesh)
        return std::string();

    auto pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];
    aiString matNameStr;
    // �̰� ���� �𸣰ڴ�.
    pMaterial->Get(AI_MATKEY_NAME, matNameStr);

    // �����ڵ�� .xml�� �ٿ���.
    return matNameStr.C_Str();
}

// ���� Engine���� include�� dll ������ ����ϰ� �ִ�.
// ���� �̸� �ű��, �ظ��ϸ� static lib �������� �����ϰ� �ʹ�. 
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

        // �̸��� ���� �� ȣ��, urho�� �ܼ� Ŀ�ǵ忡 �������� �̸��� ���Եȴ�.
        // �����Ϳ��� �̸��� ���޹޴� ���� �´� �� �ϴ�.
        exportModel(pRootNode, "");
        // export animation
        // export material

        // 2��° �̻���� �ε尡 ����Ǵ� ������ �߻��Ѵ�.
        // �׷��� ����� ���� ����ÿ� ������...
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

    // �̰� ���� �� ���� �����ϴ� �� �³��ϴ� ������ ���.
    // Materialó�� Export �Լ��� ���� ����� ���� ����.
    if (m_bAnimations)
    {
        // collection animations
        // build and save anmations
    }
}

// ��Ʈ������ ��������� ��� �޽ø� aiMesh�� �����Ѵ�.
void DvModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
    {
        aiMesh* pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

        // mong�� ���⿡�� 2���� �ɸ���.
        for (unsigned int j = 0; j < model.meshes.size(); ++j)
        {
            if (pMesh == model.meshes[j])
            {
                DV_APP_WARN("������ �޽ð� �߰ߵǾ����ϴ�.");
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

        // 1. Mesh�� BoneNode�� ã�� necessary�� ����
        // 2. BoneNode�� RootNode�� ã�� rootNodes�� ����
        for (size_t j = 0; j < pMesh->mNumBones; j++)
        {
            aiBone* pBone = pMesh->mBones[j];
            std::string boneName = pBone->mName.C_Str();
            // aiBone�� mNode�� �ִ�?
            aiNode* pBoneNode = GetNode(m_pScene->mRootNode, boneName);
            // ����� �ٸ��ٰ� ���´�...?
            //DV_APP_INFO("bone node comparison ? {:s}", pBoneNode == pBone->mNode ? "true" : " false");
            
            if (!pBoneNode)
            {
                DV_APP_ERROR("BondeNode({:s})�� ã�� ���Ͽ����ϴ�.", boneName);
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

        // ���� �޽ÿ� ���밡 ���� ���
        // �Ϻκ��� static mesh�� ��� ����.
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

    // �ϳ����߸� �ϳ� ����.
    // ���� 2�� �̻��� ��� ���� �θ� ã�� �����ϴ� ��
    if (rootNodes.size() > 1)
    {
        // �ϴ� ������ ���� �����´�.
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
        DV_APP_ERROR("���� ��Ʈ ��尡 �������� �ʾ� ���� ������ �����մϴ�.");
        return;
    }

    if (model.meshes.empty())
    {
        DV_APP_ERROR("���� �޽ð� �������� �ʾ� ���� ������ �����մϴ�.");
        return;
    }

    Dive::DvModel* pModel = new Dive::DvModel;
    
    unsigned int meshCount = 0;
    
    bool bCombineBuffers = true;
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        if (GetNumValidFaces(model.meshes[i]))
        {
            ++meshCount;

            // ���⿡�� 0��° vertex�� element�� ���Ͽ� combine buffer ������ �����Ѵ�.
        }
    }

    if (bCombineBuffers && model.totalNumVertices > 65535)
    {
        bool bLargeIndices = true;

        for (size_t i = 0; i < model.meshes.size(); ++i)
        {
            if (GetNumValidFaces(model.meshes[i]))
            {
                // ���� ������ 2byte�� �Ѵµ��� �����ٰ�?
                if (model.meshes[i]->mNumVertices > 65535)
                    bLargeIndices = false;
            }
        }

        if (bLargeIndices == true)
            bCombineBuffers = false;
    }

    pModel->SetMeshCount(meshCount);

    Dive::VertexBuffer* pVertexBuffer = nullptr;
    Dive::DvIndexBuffer* pIndexBuffer = nullptr;
    std::vector<Dive::VertexBuffer*> vertexBuffers;
    std::vector<Dive::DvIndexBuffer*> indexBuffers;
    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0;
    unsigned int meshIndex = 0;
    bool bSkinned = model.bones.size() > 0;

    // �޽ú��� ó��
    for (size_t i = 0; i < model.meshes.size(); ++i)
    {
        aiMesh* pMesh = model.meshes[i];
        unsigned int validFaces = GetNumValidFaces(pMesh);
        if (!validFaces)
            continue;

        bool bLargeIndices;
        if (bCombineBuffers)
            bLargeIndices = model.totalNumIndices > 65535;
        else
            bLargeIndices = pMesh->mNumVertices > 65535;

        // 1. !bCombineBuffers�� ��� �� �޽ø��� ����
        // 2. bCombineBuffers�� ��� ���� �� ���� ����?
        if (!bCombineBuffers || vertexBuffers.empty())
        {
            pVertexBuffer = new Dive::VertexBuffer;
            pIndexBuffer = new Dive::DvIndexBuffer;

            // ������ ���� ũ�� ����
            // �� �������� ���� �������� �Ѵ�.
            if (bCombineBuffers)
            {
                // vertex size = total vertices, element type
                // index size = total indices, largeIndices(4 or 2 byte)
                DV_APP_INFO("vertex buffer size: {0:d}", model.totalNumVertices);
                pIndexBuffer->SetSize(model.totalNumIndices, bLargeIndices);
            }
            else
            {
                // vertex size = �޽� ���� ����, element type
                // index size = validFace * 3, lageIndices(4 or 2byte)
                DV_APP_INFO("vertex buffer size: {0:d}", pMesh->mNumVertices);
                pIndexBuffer->SetSize(validFaces * 3, bLargeIndices);
            }

            vertexBuffers.emplace_back(pVertexBuffer);
            indexBuffers.emplace_back(pIndexBuffer);
        }

        // �������� ����: �θ� �����ϴ� ���� �ƴ϶� ��ȯ ����� �����Ѵ�.
        // VertexTransform, NormalTransform�� pos, rot, scale�� ���
        DirectX::XMFLOAT3 pos, scale;
        DirectX::XMFLOAT4 rot;
        GetPosRotScale(GetMeshBakingTransform(model.meshNodes[i], model.pRootNode), pos, rot, scale);

        // vertex, normal transform�� ����Ѵ�. �ϴ� normal transform�� ����
        DirectX::XMFLOAT4X4 vertexTransform;
        DirectX::XMStoreFloat4x4(&vertexTransform, 
            (DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale))
                * DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rot))
                * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos))));

        // �ռ� ������ �������� ����ߴ�...
        // urho�� ��� Geomenty��.
        Dive::DvMesh* pModelMesh = new Dive::DvMesh;

        // ���� �ռ� ������ ���۷κ��� ���� ������ ����Ų��.
        // ���� ��ü�� ũ�⸸ �Ҵ�Ǿ� �ִ�. �׷��� �����͸� ��������ϱ�
        unsigned char* pVertexData = nullptr;
        auto pIndexData = pIndexBuffer->GetSubResource();

        // 2. build vertex ����
        unsigned char* pDest = nullptr; // vertexData + offset * vertexSize
        for (size_t j = 0; j < pMesh->mNumVertices; ++j)
        {
            // vertex, normal transform�� ����.
            // bSkinned, box, blendIndex, blendWeight � ����.
        }
        // blendData�� �� �� �� ����.

        // 3. build index ����
        // ���� �޽ø� �����ϴ� �ε����� ó���Ѵ�.
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
                    //center += ToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[0]]);
                    //center += ToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[1]]);
                    //center += ToFloat3(pMesh->mVertices[pMesh->mFaces[j].mIndices[2]]);
                }
            }
        }


        // SetGeometry
        pModelMesh->SetVertexBuffer(0, pVertexBuffer); // 0���� �ϸ� �ᱹ ���۰� �ϳ���� �Ҹ���... �׷��ٸ� 2�� �̻����� ���� �ʿ䵵 ����...
        pModelMesh->SetIndexBuffer(pIndexBuffer);
        // DrawRange: draw�� �ʿ��� offset, count�� ����
        // SetGeometryCenter
        pModel->SetMesh(meshIndex, pModelMesh);

        vertexOffset += pMesh->mNumVertices;
        indexOffset += validFaces * 3;
        ++meshIndex;
    }

    // model�� ��ü ���� ���͸� �����Ѵ�?
    // SetVertexBuffers 
    pModel->SetIndexBuffers(indexBuffers);  // SetIndexBuffers
    // SetSkeleton

    // Save: ���� ���� �� �Ű������� ����
    pModel->Save();

    // Materials
    //=> Material�� ����ȭ�ϴ� ���� �ƴ϶�
    // Material ������ ����Ʈȭ�� txt�� ����� �� ����.
    // ������ ExportMaterial�� BuildAndSaveMaterial �Լ��� ���� �ִ�.
    // �̰� �־�� �� ���ϸ����� Material�� ������ �� �ִٴ� �ǰ�...?
    if (m_bMaterials)
    {
        for (size_t i = 0; i < model.meshes.size(); i++)
        {
            auto name = GetMeshMaterialName(m_pScene, model.meshes[i]);
        }
    }
}
