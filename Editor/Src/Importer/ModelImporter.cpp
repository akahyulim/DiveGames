#include "ModelImporter.h"
#include "DiveEngine.h"

namespace Editor
{
    uint32_t GetNumValidFaces(aiMesh* pMesh)
    {
        uint32_t ret = 0;

        for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            if (pMesh->mFaces[i].mNumIndices == 3)
                ++ret;
        }

        return ret;
    }

	ModelImporter::ModelImporter()
		: m_pAiScene(nullptr),
        m_bAnimations(false),
        m_bMaterials(false)
	{
	}

	ModelImporter::~ModelImporter()
	{
	}
	
    // 궁극적으로
    // 1. 파일 설정 후 정보 출력
    // 2. 익스포트 설정
    // 3. 익스포트 세 가지 기능으로 구분해야한다.
	bool ModelImporter::LoadAndExport(const std::string& filepath)
	{
        // 일단 기본 설정 사용.
        const auto flags =
            aiProcess_MakeLeftHanded |              // directx style.
            aiProcess_FlipUVs |                     // directx style.
            aiProcess_FlipWindingOrder |            // directx style.
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_OptimizeMeshes |              // reduce the number of Meshes         
            aiProcess_ImproveCacheLocality |        // re-order triangles for better vertex cache locality.
            aiProcess_RemoveRedundantMaterials |    // remove redundant/unreferenced materials.
            aiProcess_LimitBoneWeights |
            aiProcess_SplitLargeMeshes |
            aiProcess_Triangulate |
            aiProcess_GenUVCoords |
            aiProcess_SortByPType |                 // splits Meshes with more than one primitive type in homogeneous sub-Meshes.
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
        m_pAiScene = (aiScene*)importer.ReadFile(filepath, flags);
        if (!m_pAiScene)
        {
            DV_LOG_CLIENT_ERROR("전달받은 파일(:s)을 열 수 없습니다.", filepath);
            return false;
        }

        m_bAnimations = m_pAiScene->HasAnimations();
        m_bMaterials = m_pAiScene->HasMaterials();
        aiNode* pRootNode = m_pAiScene->mRootNode;

        // parsing start
        OutModel outModel;
        //outModel.OutName;
        outModel.pRootNode = pRootNode;

        collectMeshes(outModel, outModel.pRootNode);
        //printMeshData(outModel);
        //collectBones(outModel);

        // 만약 파싱 데이터가 사라지면 이걸 마지막으로 하던가
        // 다른 로드 함수를 사용해야 한다.
        importer.FreeScene();

        saveModel(outModel);

        return m_pAiScene != nullptr;
	}

    // 루트노드부터 시작해 모든 메시 정보를 취합.
    void ModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
    {
        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            aiMesh* pMesh = m_pAiScene->mMeshes[pNode->mMeshes[i]];

            for (uint32_t j = 0; j < model.Meshes.size(); ++j)
            {
                // sprtan의 경우 뒤에 +1을 붙였던 것 같다.
                if (pMesh == model.Meshes[j])
                {
                    DV_LOG_CLIENT_WARN("ModelImporter::collectMeshes - 동일한 메시가 중복 발견 되었습니다.");
                    break;
                }
            }

            model.MeshIndices.insert(pNode->mMeshes[i]);
            model.Meshes.emplace_back(pMesh);
            model.MeshNodes.emplace_back(pNode);
            model.TotalVertexCount += pMesh->mNumVertices;
            model.TotalIndexCount += GetNumValidFaces(pMesh) * 3;

            // aiNode, aiMesh에서 부모를 알아낼 수 없다.
            // 오직 aiNode를 통해 자식들만 확인할 수 있다.
            DV_LOG_CLIENT_DEBUG("mesh name: {:s}", pMesh->mName.C_Str());
        }

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
            collectMeshes(model, pNode->mChildren[i]);
    }

    // 루트뼈대와 계층구조화된 뼈대들을 저장한다.
    void ModelImporter::collectBones(OutModel& model)
    {
        // 여기에서 계층구조화 되는 듯 한데 정렬 기준을 모르겠다.
        // => 기본적으로 부모의 왼쪽은 작고 오른쪽은 크도록 정렬된다고 한다.
        // 그렇다면 aiNode에 대소 비교자가 오버로딩 되어 있어야 한다.
        // 그런데 오버로드 되어 있지 않다고 나온다...
        std::set<aiNode*> necessary;
        std::set<aiNode*> rootNodes;

        bool bHaveSkinnedMeshes = false;
        for (uint32_t i = 0; i < (uint32_t)model.Meshes.size(); ++i)
        {
            if (model.Meshes[i]->HasBones())
            {
                bHaveSkinnedMeshes = true;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)model.Meshes.size(); ++i)
        {
            aiMesh* pMesh = model.Meshes[i];
            aiNode* pMeshNode = model.MeshNodes[i];
            aiNode* pMeshParentNode = pMeshNode->mParent;
            aiNode* pRootNode = nullptr;

            // 메시 하나에도 여러개의 뼈대가 존재할 수 있다.
            // 뼈대가 있다는 건 스키닝 메시라는 말이다.
            for (uint32_t j = 0; j < (uint32_t)pMesh->mNumBones; ++j)
            {
                auto* pBone = pMesh->mBones[j];
                std::string boneName = pBone->mName.C_Str();
                DV_LOG_CLIENT_DEBUG("bone name: {:s}", boneName);

                auto* pBoneNode = getNode(boneName, model.pRootNode);
                if (!pBoneNode)
                {
                    // 실행을 멈춰야 한다. urho는 errorExit를 호출했다.
                    DV_LOG_CLIENT_ERROR("ModelImporter::collectBones - 뼈대{:s}의 노드를 찾지 못하였습니다.", boneName);
                    return;
                }
                necessary.insert(pBoneNode);
                pRootNode = pBoneNode;

                // 현재 boneNode의 부모 Node들을 전부 저장한다.
                for (;;)
                {
                    pBoneNode = pBoneNode->mParent;

                    if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode)) // 마지막에 !animationOnly
                        break;

                    pRootNode = pBoneNode;
                    necessary.insert(pBoneNode);
                }

                // 이름 그대로 루트노드들을 저장한다.
                if (rootNodes.find(pRootNode) == rootNodes.end())
                    rootNodes.insert(pRootNode);
            }

            // 이건 리지드 메시의 경우다.
            // 그렇다면 리지드 메시 단독만 존재할 경우엔 처리가 안된다는 말인데...
            // 실제로 mong.ase의 경우 뼈대 노드를 못찾는다.
            // 다만 스키닝 메시와 리지드 메시가 함께 존재하는 경우엔 처리될 것 같다.
            if (bHaveSkinnedMeshes && !pMesh->mNumBones)
            {
                auto* pBoneNode = pMeshNode;
                necessary.insert(pBoneNode);
                pRootNode = pBoneNode;

                // 바로 위 for문 이하와 동일.
                for (;;)
                {
                    pBoneNode = pBoneNode->mParent;

                    if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode)) // 마지막에 !animationOnly
                        break;

                    pRootNode = pBoneNode;
                    necessary.insert(pBoneNode);
                }

                if (rootNodes.find(pRootNode) == rootNodes.end())
                    rootNodes.insert(pRootNode);
            }
        }

        // 루트 노드가 둘 이상을 경우를 multiple root nodes라 한다.
        // 그리고 이들의 공통 부모를 찾아야 한다.
        if (rootNodes.size() > 1)
        {
            DV_LOG_CLIENT_DEBUG("ModelImporter::collectBones - 둘 이상의 루트노드를 가집니다.");
        }

        if (rootNodes.empty())
            return;

        model.pRootBone = *(rootNodes.begin());

    }

    // Model, Animation, Material, Scene Export가 구분될 수 있어야 한다.
    // 기본적인 개요는 Model, Animation 등을 Collect한 후 export하는 과정으로 이루어져 있다.
    bool ModelImporter::saveModel(OutModel& model)
    {
        // 버퍼를 만들지 않는다. Model을 생성한 후 파일화한다.

        // Mesh
        // vertices, indices, offset, size가 기본 => 현재 shaodw data를 buffer들이 가지고 있다.
        // parent, transform이 필요
        // center pos, aabb

        // Material
        // static model에도 필요하다.

        // Model
        // Mesh들을 관리

        // Animation
        // dynamic model에 필요하다.
        // 아직 어떤 데이터를 어떻게 구분해야 할 지 모르겠다.
        // 다만 이 역시 component 형태로 구성될 것이다.
        
        return false;
    }

    aiNode* ModelImporter::getNode(const std::string& name, aiNode* pRootNode)
    {
        if (!pRootNode)
            return nullptr;

        if (name == pRootNode->mName.C_Str())
            return pRootNode;

        for (uint32_t i = 0; i < pRootNode->mNumChildren; ++i)
        {
            auto* pFound = getNode(name, pRootNode->mChildren[i]);
            if (pFound)
                return pFound;
        }

        return nullptr;
    }

    void ModelImporter::printMeshData(OutModel& model)
    {
        DV_LOG_CLIENT_DEBUG("num mesh: {0:d}, num vertices: {1:d}, num indices: {2:d}",
            (uint32_t)model.Meshes.size(), model.TotalVertexCount, model.TotalIndexCount);
    }
}