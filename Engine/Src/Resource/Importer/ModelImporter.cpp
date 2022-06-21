#include "divepch.h"
#include "ModelImporter.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Renderer/Graphics/Texture2D.h"
#include "Renderer/Graphics/Texture2D.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/MeshRenderable.h"
#include "Helper/FileSystem.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
    ModelImporter::ModelImporter(Scene* pScene)
        : m_pScene(pScene)
    {
        DV_ASSERT(m_pScene != nullptr);
    }

    ModelImporter::~ModelImporter()
    {
    }

    bool ModelImporter::Load(Model* pModel, const std::string& filepath)
    {
        if (!m_pScene)
        {
            return false;
        }

        ModelParams params;
        params.filepath = filepath;
        params.name = Helper::FileSystem::GetFileNameWithoutExtension(filepath);
        params.pModel = pModel;

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

        if (const aiScene* pAiScene = importer.ReadFile(filepath, flags))
        {
            params.pAiScene = pAiScene;

            auto pRoot = m_pScene->CreateGameObject();
            pRoot->AddComponent<Transform>();
            pRoot->SetName(Helper::FileSystem::GetFileNameWithoutExtension(filepath));
            params.pModel->SetRootGameObject(pRoot);

            parseNode(pAiScene->mRootNode, params, pRoot);

            // parse animation

            pModel->UpdateGeometry();
        }

        importer.FreeScene();

        return true;
    }

    // node�� ��ȸ�ϸ� mesh �Ľ� ȣ��
    // ������ node�� mesh�� ������ ���� �ƴϴ�.
    void ModelImporter::parseNode(const aiNode* pAiNode, const ModelParams& params, GameObject* pGameObject, GameObject* pParent)
    {
        if (pParent)
        {
            pGameObject->SetName(pAiNode->mName.C_Str());

            auto pParentTransform = pParent->GetComponent<Transform>();
            pGameObject->GetComponent<Transform>()->SetParent(pParentTransform);
        }

        auto transform = pAiNode->mTransformation;
        DirectX::XMFLOAT4X4 localMatrix = {
            transform.a1, transform.b1, transform.c1, transform.d1,
            transform.a2, transform.b2, transform.c2, transform.d2,
            transform.a3, transform.b3, transform.c3, transform.d3,
            transform.a4, transform.b4, transform.c4, transform.d4
        };
        pGameObject->GetComponent<Transform>()->SetLocalMatrix(localMatrix);

        parseNodeMeshes(pAiNode, params, pGameObject);

        for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
        {
            auto pChild = m_pScene->CreateGameObject();
            pChild->AddComponent<Transform>();
            parseNode(pAiNode->mChildren[i], params, pChild, pGameObject);
        }
    }

    void ModelImporter::parseNodeMeshes(const aiNode* pAiNode, const ModelParams& params, GameObject* pGameObject)
    {
        auto nodeObject = pGameObject;

        // �ϳ��� node�� mesh�� �� �̻��� �� �ִ�.
        // �̶��� game object�� �߰��� ������ component�� data�� �־�� �Ѵ�.
        // ���ĸ�ź�� ��� �Ű��������� game object�� ���޹޴µ�,
        // 2�� �̻��� ��� �Ʒ����� �߰��� game object�� �����ϰ� �ִ�.
        for (unsigned int i = 0; i != pAiNode->mNumMeshes; i++)
        {
            auto pAiMesh = params.pAiScene->mMeshes[pAiNode->mMeshes[i]];

            std::string name = pAiNode->mName.C_Str();

            // �� �̻��̸� �̸� ����(������ �̸��� �� �ֳ�����)
            if (i > 0)
            {
                name += "_" + std::to_string(i + 1);

                // �Ű� ������ ���� game object�� ������ �ȵȴ�.
                // �������� ������ �� nodeObject�� �����ؾ� �Ѵ�.
            }

            auto pMeshRenderable = loadMesh(params, pAiMesh, nodeObject);
            loadMaterial(params, pAiMesh, pMeshRenderable);
        }
    }

    MeshRenderable* ModelImporter::loadMesh(const ModelParams& params, const aiMesh* pAiMesh, GameObject* pGameObject)
    {
        // vertices
        auto numVertices = pAiMesh->mNumVertices;
        std::vector<VertexType> vertices(numVertices);
        for (unsigned int i = 0; i < numVertices; i++)
        {
            // position
            const auto& pos = pAiMesh->mVertices[i];
            vertices[i].position[0] = pos.x;
            vertices[i].position[1] = pos.y;
            vertices[i].position[2] = pos.z;

            // normal
            const auto& normal = pAiMesh->mNormals[i];
            vertices[i].normal[0] = normal.x;
            vertices[i].normal[1] = normal.y;
            vertices[i].normal[2] = normal.z;

            // tangent
            // ase �ε尡 �ȵǴ� �� �̰� �����ε�..? �ٵ� ������ �޾Ƽ� �� ���ٴ�.
            // �ϴ� �� ���� �ְų� �ǳ� �ڴٰ� �ص� ���� ���� ����� �ʿ��ϴ�.
            const auto& tangent = pAiMesh->mTangents[i];
            vertices[i].tangent[0] = tangent.x;
            vertices[i].tangent[1] = tangent.y;
            vertices[i].tangent[2] = tangent.z;

            // texcoords
            // ����� �� �̻��ϴ�. 1.0f �̻��� ������ ����.
            // �ϴ� �ذ������� �ٸ� ����� ���� �� ����.
            // �� ���� ���� ���ϵ��� �ִ�. ���� ��굵 ��ƴ�.
            const auto& texCoord = pAiMesh->mTextureCoords[0][i];
            vertices[i].texCoords[0] = texCoord.x > 1.0f ? texCoord.x - 1.0f : texCoord.x;
            vertices[i].texCoords[1] = texCoord.y > 1.0f ? texCoord.y - 1.0f : texCoord.y;

        }

        // indices
        auto numIndices = pAiMesh->mNumFaces * 3;
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < pAiMesh->mNumFaces; i++)
        {
            auto& aiFace = pAiMesh->mFaces[i];
            indices.emplace_back(aiFace.mIndices[0]);
            indices.emplace_back(aiFace.mIndices[1]);
            indices.emplace_back(aiFace.mIndices[2]);
        }

        // �� �̾Ƴ� �� 
        // ���� ����ؾ� �ϴ� �κ��� ������ �� �ִ�.

        // Model�� geometry data�� �����Ѵ�.
        unsigned int vertexOffset = 0;
        unsigned int indexOffset = 0;
        params.pModel->AppendGeometry(vertices, indices, &vertexOffset, &indexOffset);

        // Renderable�� �����ϰ� �����͸� �����Ѵ�.
        auto pMeshRenderable = pGameObject->AddComponent<MeshRenderable>();
        pMeshRenderable->SetGeometry(
            pGameObject->GetName(),
            vertexOffset,
            numVertices,
            indexOffset,
            numIndices,
            params.pModel);

        return pMeshRenderable;
    }

    // ���� Mesh ������ Material�� Ž���ϰ� �ִ�.
    // �ش� Mesh ������ MeshRenderable�� �����ϰ� mtrl�� �����ϱ� ���� �� �ϳ�
    // ������ �̸��� mtrl�� �˻��Ǵ� ������ ���� mtrl�� �ߺ� �������� �����غ� �� �ִ�.
    // ���� ���⿡�� Renderable�� ���Ḹ �ϰ�
    // Material�� ������ �ٸ� ����� ã�ƾ� �Ѵ�.
    void ModelImporter::loadMaterial(const ModelParams& params, const aiMesh* pAiMesh, MeshRenderable* pMeshRenderable)
    {
        if (!params.pAiScene->HasMaterials())
            return;

        // material ����
        auto pMaterial = new Material();
        auto pAiMaterial = params.pAiScene->mMaterials[pAiMesh->mMaterialIndex];
        // �ϴ� �̸��� ����� �Ŵ����� �ִ´�.
        // ���ĸ�ź�� Model�� Set�ϸ� �� ������ �Բ� ���޵� Renderable���� Cache�Ѵ�.
        pMaterial->SetName(pAiMaterial->GetName().C_Str());
        ResourceManager::GetInstance().Cache<Material>(pMaterial);
        DV_CORE_TRACE("mtrl: {:s}", pAiMaterial->GetName().C_Str());
        // ������ �̸��� mtrl�� �����Ѵ�.
        // ������ �� ���� ���� ������ ���� Ȯ���� �Ұ����ϴٴ� ���̴�.
        // ���� Mesh ������ Ž�� ���̴� �Ƹ��� ���� �� ���ٴ� ������ ���.

        // name
        // �� �̸��� ���� Engine format�� material file �̸����� Ȱ���ϴ� �� �ϴ�.
        aiString name;
        aiGetMaterialString(pAiMaterial, AI_MATKEY_NAME, &name);
        // ���� ���̸� �𸣰ڴ�.
        //DV_CORE_INFO("material name: {0:s} / {1:s}", pAiMaterial->GetName().C_Str(), name.C_Str());

        // color + opacity
        aiColor4D color;
        aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
        aiColor4D opacity;
        aiGetMaterialColor(pAiMaterial, AI_MATKEY_OPACITY, &opacity);
        pMaterial->SetAlbedoColor(DirectX::XMFLOAT4(color.r, color.g, color.b, opacity.r));

        // textures
        const auto loadMaterialTex = [&params, pAiMaterial, pMeshRenderable, pMaterial](const eMaterialMapType diveType, const aiTextureType assimpPbrType, const aiTextureType assimpLegacyType)
        {
            aiTextureType assimpType = pAiMaterial->GetTextureCount(assimpPbrType) > 0 ? assimpPbrType : aiTextureType_NONE;
            assimpType = pAiMaterial->GetTextureCount(assimpLegacyType) > 0 ? assimpLegacyType : assimpType;

            aiString path;
            if (pAiMaterial->GetTextureCount(assimpType) > 0)
            {
                if (AI_SUCCESS == pAiMaterial->GetTexture(assimpType, 0, &path))
                {
                    const auto& texPath = Helper::FileSystem::GetDirectory(params.filepath) + path.C_Str();
                    auto pTex = new Texture2D;
                    //if (!pTex->LoadFromFile(texPath))
                    pTex->LoadFromFile(texPath);
                    {
                        // �ӽ�
                    //    delete pTex;
                        
                     //   if(diveType == eMaterialMapType::Albedo)
                      //      pTex = ResourceManager::GetInstance().Load<Texture2D>("Assets/Textures/no_texture.png");
                    }
                   // if(pTex)
                        pMaterial->SetMap(diveType, pTex);

                    DV_CORE_INFO("mtrl tex: {:s}", path.C_Str());

                    if (diveType == eMaterialMapType::Albedo)
                    {
                        // color�� 0, 0, 0, 0���� �ٲ��. ������...   
                    }

                    // ��� ���� normal�� hight��, hight�� normal�� �ٷ�ٰ� �Ѵ�.
                    // ���� �̸� ��ġ�� �κ��� �ִ�...
                }
            }
        };

        loadMaterialTex(eMaterialMapType::Albedo, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE);
        loadMaterialTex(eMaterialMapType::Normal, aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS);

        pMeshRenderable->SetMaterial(pMaterial);
    }
}