#include "ModelImporter.h"
#include "DiveEngine.h"

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
     
        DV_APP_INFO("start - {:s}", filepath);

        // �̸��� ���� �� ȣ��
        exportModel(pRootNode, "");
        // export animation
        // export material
    }
    else
    {
        DV_APP_ERROR("�� ����Ʈ�� �����Ͽ����ϴ�.");
        return false;
    }

    importer.FreeScene();

    DV_APP_INFO("end - {:s}", filepath);

	return true;
}

void DvModelImporter::exportModel(aiNode* pRootNode, const std::string& outName)
{
    OutModel model;
    model.pRootNode = pRootNode;
    model.outName = outName;

    collectMeshes(model, model.pRootNode);
    // collect bones
    // build bone collision info
    buildAndSaveModel(model);

    DV_APP_INFO("vertex size: {0:d}, index size: {1:d}", model.vertexTotalCount, model.indexTotalCount);

    // �̰� ���� �� ���� �����ϴ� �� �³��ϴ� ������ ���.
    // Materialó�� Export �Լ��� ���� ����� ���� ����.
    if (m_bAnimations)
    {
        // collection animations
        // build and save anmations
    }
}

// ���ó� �� �������� ���������� ���߿� ����.
// BuidAndSaveModel()���� ����� ������ �ִ�.
void DvModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        auto pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

        for (unsigned int j = 0; j < model.meshes.size(); j++)
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
        model.vertexTotalCount += pMesh->mNumVertices;
        model.indexTotalCount += GetNumValidFaces(pMesh) * 3;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        collectMeshes(model, pNode->mChildren[i]);
    }
}

// OutModel�� DvModel�� ������ �� Model�� ����ȭ�Ѵ�.
// vertices, indices, bone���� �����ȴ�.
void DvModelImporter::buildAndSaveModel(OutModel& model)
{
    if (!model.pRootNode)
    {
        return;
    }

    if (model.meshes.empty())
    {
        return;
    }

   // auto pDvModel = new Dive::DvModel;

    // ������ �𸣰����� ���۱��� ���� �����Ѵ�.
    
    for (size_t i = 0; i < model.meshes.size(); i++)
    {


        // ���������� ��� �θ� ���� �ڽ��� Transform�� �����ϴ� ����� �ƴ϶�
        // �ƿ� ���� �� vertices���� ��ȯ�Ͽ� �����ϴ� �� �ϴ�.
        // �м��� �� �� �ʿ��ϴ�.

    }


    // DvModel ����ȭ Save

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
            DV_APP_INFO("mat name: {:s}", name);
        }
    }
}
