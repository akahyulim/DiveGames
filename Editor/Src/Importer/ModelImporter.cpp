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
        // �ϴ� �⺻ ���� ���.
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
        // urho3d�� aiImportFileExWithProperties()�� aiImportFile()�� ����Ѵ�.
        m_pScene = (aiScene*)importer.ReadFile(filepath, flags);
        if (!m_pScene)
        {
            DV_LOG_CLIENT_ERROR("���޹��� ����(:s)�� �� �� �����ϴ�.", filepath);
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
        //printMeshData(outModel);
        //collectBones(outModel);

        // ���� �Ľ� �����Ͱ� ������� �̰� ���������� �ϴ���
        // �ٸ� �ε� �Լ��� ����ؾ� �Ѵ�.
        importer.FreeScene();

        saveModel(outModel);

        return m_pScene != nullptr;
	}

    // ��Ʈ������ ������ ��� �޽� ������ ����.
    void ModelImporter::collectMeshes(OutModel& model, aiNode* pNode)
    {
        for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
        {
            aiMesh* pMesh = m_pScene->mMeshes[pNode->mMeshes[i]];

            for (unsigned int j = 0; j < model.meshes.size(); ++j)
            {
                // sprtan�� ��� �ڿ� +1�� �ٿ��� �� ����.
                if (pMesh == model.meshes[j])
                {
                    DV_LOG_CLIENT_WARN("������ �޽ð� �ߺ� �߰� �Ǿ����ϴ�.");
                    break;
                }
            }

            model.meshIndices.insert(pNode->mMeshes[i]);
            model.meshes.emplace_back(pMesh);
            model.meshNodes.emplace_back(pNode);
            model.totalNumVertices += pMesh->mNumVertices;
            model.totalNumIndices += GetNumValidFaces(pMesh) * 3;

            // aiNode, aiMesh���� �θ� �˾Ƴ� �� ����.
            // ���� aiNode�� ���� �ڽĵ鸸 Ȯ���� �� �ִ�.
            DV_LOG_CLIENT_DEBUG("mesh name: {:s}", pMesh->mName.C_Str());
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
            collectMeshes(model, pNode->mChildren[i]);
    }

    // ��Ʈ����� ��������ȭ�� ������� �����Ѵ�.
    void ModelImporter::collectBones(OutModel& model)
    {
        // ���⿡�� ��������ȭ �Ǵ� �� �ѵ� ���� ������ �𸣰ڴ�.
        // => �⺻������ �θ��� ������ �۰� �������� ũ���� ���ĵȴٰ� �Ѵ�.
        // �׷��ٸ� aiNode�� ��� ���ڰ� �����ε� �Ǿ� �־�� �Ѵ�.
        // �׷��� �����ε� �Ǿ� ���� �ʴٰ� ���´�...
        std::set<aiNode*> necessary;
        std::set<aiNode*> rootNodes;

        bool bHaveSkinnedMeshes = false;
        for (unsigned int i = 0; i < (unsigned int)model.meshes.size(); ++i)
        {
            if (model.meshes[i]->HasBones())
            {
                bHaveSkinnedMeshes = true;
                break;
            }
        }

        for (unsigned int i = 0; i < (unsigned int)model.meshes.size(); ++i)
        {
            aiMesh* pMesh = model.meshes[i];
            aiNode* pMeshNode = model.meshNodes[i];
            aiNode* pMeshParentNode = pMeshNode->mParent;
            aiNode* pRootNode = nullptr;

            // �޽� �ϳ����� �������� ���밡 ������ �� �ִ�.
            // ���밡 �ִٴ� �� ��Ű�� �޽ö�� ���̴�.
            for (unsigned int j = 0; j < (unsigned int)pMesh->mNumBones; ++j)
            {
                auto* pBone = pMesh->mBones[j];
                std::string boneName = pBone->mName.C_Str();
                DV_LOG_CLIENT_DEBUG("bone name: {:s}", boneName);

                auto* pBoneNode = getNode(boneName, model.pRootNode);
                if (!pBoneNode)
                {
                    // ������ ����� �Ѵ�. urho�� errorExit�� ȣ���ߴ�.
                    DV_LOG_CLIENT_ERROR("����{:s}�� ��带 ã�� ���Ͽ����ϴ�.", boneName);
                    return;
                }
                necessary.insert(pBoneNode);
                pRootNode = pBoneNode;

                // ���� boneNode�� �θ� Node���� ���� �����Ѵ�.
                for (;;)
                {
                    pBoneNode = pBoneNode->mParent;

                    if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode)) // �������� !animationOnly
                        break;

                    pRootNode = pBoneNode;
                    necessary.insert(pBoneNode);
                }

                // �̸� �״�� ��Ʈ������ �����Ѵ�.
                if (rootNodes.find(pRootNode) == rootNodes.end())
                    rootNodes.insert(pRootNode);
            }

            // �̰� ������ �޽��� ����.
            // �׷��ٸ� ������ �޽� �ܵ��� ������ ��쿣 ó���� �ȵȴٴ� ���ε�...
            // ������ mong.ase�� ��� ���� ��带 ��ã�´�.
            // �ٸ� ��Ű�� �޽ÿ� ������ �޽ð� �Բ� �����ϴ� ��쿣 ó���� �� ����.
            if (bHaveSkinnedMeshes && !pMesh->mNumBones)
            {
                auto* pBoneNode = pMeshNode;
                necessary.insert(pBoneNode);
                pRootNode = pBoneNode;

                // �ٷ� �� for�� ���Ͽ� ����.
                for (;;)
                {
                    pBoneNode = pBoneNode->mParent;

                    if (!pBoneNode || (pBoneNode == pMeshNode || pBoneNode == pMeshParentNode)) // �������� !animationOnly
                        break;

                    pRootNode = pBoneNode;
                    necessary.insert(pBoneNode);
                }

                if (rootNodes.find(pRootNode) == rootNodes.end())
                    rootNodes.insert(pRootNode);
            }
        }

        // ��Ʈ ��尡 �� �̻��� ��츦 multiple root nodes�� �Ѵ�.
        // �׸��� �̵��� ���� �θ� ã�ƾ� �Ѵ�.
        if (rootNodes.size() > 1)
        {
            DV_LOG_CLIENT_DEBUG("find multiple root node!");
        }

        if (rootNodes.empty())
            return;

        model.pRootBone = *(rootNodes.begin());

    }

    // Model, Animation, Material, Scene Export�� ���е� �� �־�� �Ѵ�.
    // �⺻���� ����� Model, Animation ���� Collect�� �� export�ϴ� �������� �̷���� �ִ�.
    bool ModelImporter::saveModel(OutModel& model)
    {
        // ���۸� ������ �ʴ´�. Model�� ������ �� ����ȭ�Ѵ�.

        // Mesh
        // vertices, indices, offset, size�� �⺻ => ���� shaodw data�� buffer���� ������ �ִ�.
        // parent, transform�� �ʿ�
        // center pos, aabb

        // Material
        // static model���� �ʿ��ϴ�.

        // Model
        // Mesh���� ����

        // Animation
        // dynamic model�� �ʿ��ϴ�.
        // ���� � �����͸� ��� �����ؾ� �� �� �𸣰ڴ�.
        // �ٸ� �� ���� component ���·� ������ ���̴�.
        
        return false;
    }

    aiNode* ModelImporter::getNode(const std::string& name, aiNode* pRootNode)
    {
        if (!pRootNode)
            return nullptr;

        if (name == pRootNode->mName.C_Str())
            return pRootNode;

        for (unsigned int i = 0; i < pRootNode->mNumChildren; ++i)
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
            (unsigned int)model.meshes.size(), model.totalNumVertices, model.totalNumIndices);
    }
}