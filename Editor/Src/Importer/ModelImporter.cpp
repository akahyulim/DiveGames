#include "ModelImporter.h"
#include "DiveEngine.h"

ModelImporter::ModelImporter()
{
}

ModelImporter::~ModelImporter()
{
}

bool ModelImporter::Load(Dive::Model* pModel, const std::string& filepath)
{
    if (!m_pScene)
    {
        return false;
    }

    m_pModel = pModel;

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

    if (const aiScene* pScene = importer.ReadFile(filepath, flags))
    {
        auto pRoot = m_pScene->CreateGameObject();
        // 확장자가 붙은 파일명이다. 역시 확장자는 제거하는 게 나아 보인다.
        std::wstring name(filepath.begin(), filepath.end());
        name = std::filesystem::_Parse_filename(name);
        std::string filename(name.begin(), name.end());
        pRoot->SetName(filename);
        m_pModel->SetRootGameObject(pRoot);

        parseNode(pScene->mRootNode, pScene, pRoot, nullptr);

        // parse animation

        pModel->UpdateGeometry();
    }

    importer.FreeScene();
    
	return true;
}

// node를 순회하며 mesh 파싱 호출
// 하지만 node는 mesh만 가지는 것이 아니다.
void ModelImporter::parseNode(const aiNode* pAiNode, const aiScene* pAiScene, Dive::GameObject* pGameObject, Dive::GameObject* pParent)
{
    if (pParent)
    {
        pGameObject->SetName(pAiNode->mName.C_Str());

        auto pParentTransform = pParent->GetComponent<Dive::Transform>();
        pGameObject->GetComponent<Dive::Transform>()->SetParent(pParentTransform);
    }

    auto transform = pAiNode->mTransformation;
    DirectX::XMFLOAT4X4 localMatrix = {
        transform.a1, transform.b1, transform.c1, transform.d1,
        transform.a2, transform.b2, transform.c2, transform.d2,
        transform.a3, transform.b3, transform.c3, transform.d3,
        transform.a4, transform.b4, transform.c4, transform.d4
    };
    pGameObject->GetComponent<Dive::Transform>()->SetLocalMatrix(localMatrix);
 
    parseNodeMeshes(pAiNode, pAiScene, pGameObject);

    for (unsigned int i = 0; i < pAiNode->mNumChildren; i++)
    {
        auto pChild = m_pScene->CreateGameObject();
        parseNode(pAiNode->mChildren[i], pAiScene, pChild, pGameObject);
    }
}

void ModelImporter::parseNodeMeshes(const aiNode* pAiNode, const aiScene* pAiScene, Dive::GameObject* pGameObject)
{
    auto nodeObject = pGameObject;

    // 하나의 node에 mesh가 둘 이상일 수 있다.
    // 이때는 game object를 추가로 생성해 component에 data를 넣어야 한다.
    // 스파르탄의 경우 매개변수에서 game object를 전달받는데,
    // 2개 이상인 경우 아래에서 추가로 game object를 생성하고 있다.
    for (unsigned int i = 0; i != pAiNode->mNumMeshes; i++)
    {
        auto pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[i]];

        std::string name = pAiNode->mName.C_Str();

        // 둘 이상이면 이름 수정(동일한 이름일 수 있나보다)
        if (i > 0)
        {
            name += "_" + std::to_string(i + 1);

            // 매개 변수로 받은 game object에 덮어씌우면 안된다.
            // 동적으로 생성한 후 nodeObject에 대입해야 한다.
        }

        auto pMeshRenderable = loadMesh(pAiMesh, nodeObject);
        loadMaterial(pAiScene, pAiMesh, pMeshRenderable);
    }
}

Dive::MeshRenderable* ModelImporter::loadMesh(const aiMesh* pAiMesh, Dive::GameObject* pGameObject)
{
    // vertices
    auto numVertices = pAiMesh->mNumVertices;
    std::vector<Dive::VertexType> vertices(numVertices);
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
        const auto& texCoord = pAiMesh->mTextureCoords[0][i];
        vertices[i].texCoords[0] = texCoord.x > 1.0f ? texCoord.x - 1.0f: texCoord.x;
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
    m_pModel->AppendGeometry(vertices, indices, &vertexOffset, &indexOffset);

    // Renderable을 생성하고 데이터를 저장한다.
    auto pMeshRenderable = pGameObject->AddComponent<Dive::MeshRenderable>();
    pMeshRenderable->SetGeometry(
        pGameObject->GetName(),
        vertexOffset,
        numVertices,
        indexOffset,
        numIndices,
        m_pModel);

    return pMeshRenderable;
}

void ModelImporter::loadMaterial(const aiScene* pAiScene, const aiMesh* pAiMesh, Dive::MeshRenderable* pMeshRenderable)
{
    if (!pAiScene->HasMaterials())
        return;

    // material 생성
    // 스파르탄도 그냥 동적생성한다. 임포트의 특성일 수 있다.
    auto pMaterial = new Dive::Material();

    auto pAiMaterial = pAiScene->mMaterials[pAiMesh->mMaterialIndex];

    // name
    // 이 이름을 추후 Engine format의 material file 이름으로 활용하는 듯 하다.
    aiString name;
    aiGetMaterialString(pAiMaterial, AI_MATKEY_NAME, &name);
    DV_APP_INFO("name(filePath): {:s}", name.C_Str());

    // color + opacity
    aiColor4D color;
    aiGetMaterialColor(pAiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
    aiColor4D opacity;
    aiGetMaterialColor(pAiMaterial, AI_MATKEY_OPACITY, &opacity);
    pMaterial->SetAlbedoColor(DirectX::XMFLOAT4(color.r, color.g, color.b, opacity.r));

    // maps
    const auto loadMaterialTex = [pAiMaterial, pMaterial](const Dive::eMaterialMapType diveType, const aiTextureType assimpPbrType, const aiTextureType assimpLegacyType)
    {
        aiTextureType assimpType = pAiMaterial->GetTextureCount(assimpPbrType) > 0 ? assimpPbrType : aiTextureType_NONE;
        assimpType = pAiMaterial->GetTextureCount(assimpLegacyType) > 0 ? assimpLegacyType : assimpType;

        aiString path;
        if (pAiMaterial->GetTextureCount(assimpType) > 0)
        {
            if (AI_SUCCESS == pAiMaterial->GetTexture(assimpType, 0, &path))
            {
                // 경로가 다를 수 있다. 따라서 추론하는 helper함수가 필요하다.
                
                // 해당 경로의 텍스쳐 생성

                // 테스트용 강제 로드
                // 결과를 보니 texCoord가 이상하다.
                if (diveType == Dive::eMaterialMapType::Albedo)
                {
                    auto pTexture = Dive::Texture2D::Create("Assets/Models/dancing-stormtrooper/textures/Stormtrooper_D.png");
                    pMaterial->SetMap(diveType, pTexture);
                }

                // matrial에 texture 저장. 그런데 model에서 이루어진다.
                // 정확하게 말하자면 Model의 AddTexture()에서 Material에 type에 맞춰 texture를 set한다.
                // texture의 획득(생성) 역시 이 함수 내부에서 이루어진다.

                // 스파르타은 height를 normal로 set 하고 있다.
                // 이는 assimp 특성 때문인 듯 하다.
            }
        }
    };

    loadMaterialTex(Dive::eMaterialMapType::Albedo, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE);
    loadMaterialTex(Dive::eMaterialMapType::Normal, aiTextureType_NORMAL_CAMERA, aiTextureType_NORMALS);

    pMeshRenderable->SetMaterail(pMaterial);
}
