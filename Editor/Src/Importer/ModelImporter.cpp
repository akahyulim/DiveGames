#include "ModelImporter.h"
#include "DiveEngine.h"

namespace Editor
{
    unsigned int GetNumValidFaces(aiMesh* pMesh)
    {
        unsigned int ret = 0;

        for (unsigned int i = 0; i < pMesh->mNumFaces; ++i)
        {
            if (pMesh->mFaces[i].mNumIndices == 3)
                ++ret;
        }

        return ret;
    }

	ModelImporter::ModelImporter()
		: m_pScene(nullptr),
        m_bAnimations(false),
        m_bMaterials(false)
	{
	}

	ModelImporter::~ModelImporter()
	{
	}
	
	bool ModelImporter::LoadAndExportModel(const std::string& filepath)
	{
        // 일단 기본 설정 사용.
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

        // Load
        // urho3d는 aiImportFileExWithProperties()와 aiImportFile()을 사용한다.
        m_pScene = (aiScene*)importer.ReadFile(filepath, flags);
        if (!m_pScene)
        {
            DV_LOG_CLIENT_ERROR("전달받은 파일(:s)을 열 수 없습니다.", filepath);
            return false;
        }

        m_bAnimations = m_pScene->HasAnimations();
        m_bMaterials = m_pScene->HasMaterials();
        aiNode* pRootNode = m_pScene->mRootNode;

        // parsing start
        OutModel outModel;
        //outModel.outName;
        outModel.pRootNode = pRootNode;

        collectMeshes(outModel, outModel.pRootNode);

        // 만약 파싱 데이터가 사라지면 이걸 마지막으로 하던가
        // 다른 로드 함수를 사용해야 한다.
        importer.FreeScene();

        return saveModel();
	}

    void ModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
    {
        for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
        {
            aiMesh* pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

            for (unsigned int j = 0; j < model.meshes.size(); ++j)
            {
                if (pMesh == model.meshes[j])
                {
                    DV_LOG_CLIENT_WARN("동일한 메시가 중복 발견 되었습니다.");
                    break;
                }
            }

            model.meshIndices.emplace_back(pNode->mMeshes[i]);
            model.meshes.emplace_back(pMesh);
            model.meshNodes.emplace_back(pNode);
            model.totalNumVertices += pMesh->mNumVertices;
            model.totalNumIndices += GetNumValidFaces(pMesh) * 3;
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
            collectMeshes(model, pNode->mChildren[i]);
    }

    // 이때 생성된 Model을 Scene에서 사용할 지
    // 엔진 파일로 만들지도 생각해야 한다.
    bool ModelImporter::saveModel()
    {
        // 파싱한 데이터를 이용해 버퍼를 만들고
        // 이를 Model, Geometry에 전달한 후
        // Model을 파일로 save한다.
        
        return false;
    }
}