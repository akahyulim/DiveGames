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

    // node를 순회하며 mesh 파싱 호출
    // 하지만 node는 mesh만 가지는 것이 아니다.
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

        // 하나의 node에 mesh가 둘 이상일 수 있다.
        // 이때는 game object를 추가로 생성해 component에 data를 넣어야 한다.
        // 스파르탄의 경우 매개변수에서 game object를 전달받는데,
        // 2개 이상인 경우 아래에서 추가로 game object를 생성하고 있다.
        for (unsigned int i = 0; i != pAiNode->mNumMeshes; i++)
        {
            auto pAiMesh = params.pAiScene->mMeshes[pAiNode->mMeshes[i]];

            std::string name = pAiNode->mName.C_Str();

            // 둘 이상이면 이름 수정(동일한 이름일 수 있나보다)
            if (i > 0)
            {
                name += "_" + std::to_string(i + 1);

                // 매개 변수로 받은 game object에 덮어씌우면 안된다.
                // 동적으로 생성한 후 nodeObject에 대입해야 한다.
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
            // ase 로드가 안되는 건 이것 때문인듯..? 근데 참조로 받아서 좀 에바다.
            // 일단 빈 값을 넣거나 건너 뛴다고 해도 추후 직접 계산이 필요하다.
            const auto& tangent = pAiMesh->mTangents[i];
            vertices[i].tangent[0] = tangent.x;
            vertices[i].tangent[1] = tangent.y;
            vertices[i].tangent[2] = tangent.z;

            // texcoords
            // 결과가 좀 이상하다. 1.0f 이상인 값들이 들어간다.
            // 일단 해결했지만 다른 방법이 있을 것 같다.
            // 이 값도 없는 파일들이 있다. 직접 계산도 어렵다.
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

        // 다 뽑아낸 후 
        // 직접 계산해야 하는 부분이 존재할 수 있다.

        // Model에 geometry data를 전달한다.
        unsigned int vertexOffset = 0;
        unsigned int indexOffset = 0;
        params.pModel->AppendGeometry(vertices, indices, &vertexOffset, &indexOffset);

        // Renderable을 생성하고 데이터를 저장한다.
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

    // 현재 Mesh 단위로 Material을 탐색하고 있다.
    // 해당 Mesh 단위로 MeshRenderable을 생성하고 mtrl을 연결하기 위한 듯 하나
    // 동일한 이름의 mtrl이 검색되는 것으로 보아 mtrl이 중복 생성됨을 생각해볼 수 있다.
    // 따라서 여기에선 Renderable에 연결만 하고
    // Material의 생성은 다른 방법을 찾아야 한다.
    void ModelImporter::loadMaterial(const ModelParams& params, const aiMesh* pAiMesh, MeshRenderable* pMeshRenderable)
    {
        if (!params.pAiScene->HasMaterials())
            return;

        // material 생성
        auto pMaterial = new Material();
        auto pAiMaterial = params.pAiScene->mMaterials[pAiMesh->mMaterialIndex];
        // 일단 이름을 만들고 매니져에 넣는다.
        // 스파르탄은 Model에 Set하면 그 곳에서 함께 전달된 Renderable에서 Cache한다.
        pMaterial->SetName(pAiMaterial->GetName().C_Str());
        ResourceManager::GetInstance().Cache<Material>(pMaterial);
        DV_CORE_TRACE("mtrl: {:s}", pAiMaterial->GetName().C_Str());
        // 동일한 이름의 mtrl이 존재한다.
        // 문제는 값 또한 같은 것인지 아직 확인이 불가능하다는 것이다.
        // 현재 Mesh 단위로 탐색 중이니 아마도 같은 것 같다는 생각은 든다.

        // name
        // 이 이름을 추후 Engine format의 material file 이름으로 활용하는 듯 하다.
        aiString name;
        aiGetMaterialString(pAiMaterial, AI_MATKEY_NAME, &name);
        // 둘의 차이를 모르겠다.
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
                        // 임시
                    //    delete pTex;
                        
                     //   if(diveType == eMaterialMapType::Albedo)
                      //      pTex = ResourceManager::GetInstance().Load<Texture2D>("Assets/Textures/no_texture.png");
                    }
                   // if(pTex)
                        pMaterial->SetMap(diveType, pTex);

                    DV_CORE_INFO("mtrl tex: {:s}", path.C_Str());

                    if (diveType == eMaterialMapType::Albedo)
                    {
                        // color를 0, 0, 0, 0으로 바꿨다. 이유는...   
                    }

                    // 몇몇 모델은 normal을 hight로, hight를 normal로 다룬다고 한다.
                    // 따라서 이를 고치는 부분이 있다...
                }
            }
        };

        loadMaterialTex(eMaterialMapType::Albedo, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE);
        loadMaterialTex(eMaterialMapType::Normal, aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS);

        pMeshRenderable->SetMaterial(pMaterial);
    }
}