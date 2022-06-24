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
     
        DV_APP_INFO("start - {:s}", filepath);

        // 이름을 만든 후 호출
        exportModel(pRootNode, "");
        // export animation
        // export material
    }
    else
    {
        DV_APP_ERROR("모델 임포트에 실패하였습니다.");
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

    // 이걸 굳이 이 곳에 포함하는 게 맞나하는 생각이 든다.
    // Material처럼 Export 함수를 따로 만드는 편이 낫다.
    if (m_bAnimations)
    {
        // collection animations
        // build and save anmations
    }
}

// 역시나 이 곳에서도 계층구조는 안중에 없다.
// BuidAndSaveModel()에서 비슷한 구문이 있다.
void DvModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        auto pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

        for (unsigned int j = 0; j < model.meshes.size(); j++)
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
        model.vertexTotalCount += pMesh->mNumVertices;
        model.indexTotalCount += GetNumValidFaces(pMesh) * 3;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        collectMeshes(model, pNode->mChildren[i]);
    }
}

// OutModel로 DvModel을 구성한 후 Model을 파일화한다.
// vertices, indices, bone으로 구성된다.
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

    // 왜인지 모르겠지만 버퍼까지 직접 생성한다.
    
    for (size_t i = 0; i < model.meshes.size(); i++)
    {


        // 계층구조의 경우 부모 노드와 자신의 Transform을 저장하는 방식이 아니라
        // 아에 곱한 후 vertices까지 변환하여 저장하는 듯 하다.
        // 분석이 좀 더 필요하다.

    }


    // DvModel 파일화 Save

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
            DV_APP_INFO("mat name: {:s}", name);
        }
    }
}
