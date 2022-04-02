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

    if (const aiScene* pScene = importer.ReadFile(filepath, flags))
    {
        auto pRoot = m_pScene->CreateGameObject();
        // Ȯ���ڰ� ���� ���ϸ��̴�. ���� Ȯ���ڴ� �����ϴ� �� ���� ���δ�.
        std::wstring name(filepath.begin(), filepath.end());
        name = std::filesystem::_Parse_filename(name);
        std::string filename(name.begin(), name.end());
        pRoot->SetName(filename);
        m_pModel->SetRootGameObject(pRoot);

        // parse node
        parseNode(pScene->mRootNode, pScene, pRoot, nullptr);

        // parse animation

        // update model(create buffers)
        pModel->UpdateGeometry();
    }

    importer.FreeScene();
    
	return true;
}

// node�� ��ȸ�ϸ� mesh �Ľ� ȣ��
// ������ node�� mesh�� ������ ���� �ƴϴ�.
void ModelImporter::parseNode(const aiNode* pNode, const aiScene* pScene, Dive::GameObject* pGameObject, Dive::GameObject* pParent)
{
    if (pParent)
    {
        pGameObject->SetName(pNode->mName.C_Str());

        auto pParentTransform = pParent->GetComponent<Dive::Transform>();
        pGameObject->GetComponent<Dive::Transform>()->SetParent(pParentTransform);
    }

    // node�κ��� transform�� ��� ����
    auto transform = pNode->mTransformation;
    DirectX::XMFLOAT4X4 localMatrix = {
        transform.a1, transform.b1, transform.c1, transform.d1,
        transform.a2, transform.b2, transform.c2, transform.d2,
        transform.a3, transform.b3, transform.c3, transform.d3,
        transform.a4, transform.b4, transform.c4, transform.d4
    };
    pGameObject->GetComponent<Dive::Transform>()->SetLocalMatrix(localMatrix);
 
    // parse node meshes
    parseNodeMeshes(pNode, pScene, pGameObject);

    // parse node children(recusive)
    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        auto pChild = m_pScene->CreateGameObject();
        parseNode(pNode->mChildren[i], pScene, pChild, pGameObject);
    }
}

// mesh�� �Ľ�
void ModelImporter::parseNodeMeshes(const aiNode* pNode, const aiScene* pScene, Dive::GameObject* pGameObject)
{
    auto nodeObject = pGameObject;

    // �ϳ��� node�� mesh�� �� �̻��� �� �ִ�.
    // �̶��� game object�� �߰��� ������ component�� data�� �־�� �Ѵ�.
    // ���ĸ�ź�� ��� �Ű��������� game object�� ���޹޴µ�,
    // 2�� �̻��� ��� �Ʒ����� �߰��� game object�� �����ϰ� �ִ�.
    for (unsigned int i = 0; i != pNode->mNumMeshes; i++)
    {
        auto pMesh = pScene->mMeshes[pNode->mMeshes[i]];
       
        std::string name = pNode->mName.C_Str();
        DV_APP_INFO("Mesh Name: {:s}", name);
        
        // �� �̻��̸� �̸� ����(������ �̸��� �� �ֳ�����)
        if (i > 0)
        {
            name += "_" + std::to_string(i + 1);
            DV_APP_INFO("Mesh Name: {:s}", name);

            // �Ű� ������ ���� game object�� ������ �ȵȴ�.
            // �������� ������ �� nodeObject�� �����ؾ� �Ѵ�.
        }

        // load mesh
        // �̸��� aiMesh���� ������ mesh�� �� �̻��� �ܿ� �̸��� �ߺ��� �� �ִ�.
        // ���� �ռ� ������ �̸��� �� ������ game object�� set���ش�.
        loadMesh(pMesh, nodeObject);
    }
}

void ModelImporter::loadMesh(const aiMesh* pMesh, Dive::GameObject* pGameObject)
{
    // vertices
    auto numVertices = pMesh->mNumVertices;
    std::vector<Dive::VertexType> vertices(numVertices);
    for (unsigned int i = 0; i < numVertices; i++)
    {
        // position
        const auto& pos = pMesh->mVertices[i];
        vertices[i].position[0] = pos.x;
        vertices[i].position[1] = pos.y;
        vertices[i].position[2] = pos.z;

        // texcoords
        // 8������ �����Ѵ�...
        const auto& texCoord = pMesh->mTextureCoords[0][i];
        vertices[i].texCoords[0] = texCoord.x;
        vertices[i].texCoords[1] = texCoord.y;

        // normal
        const auto& normal = pMesh->mNormals[i];
        vertices[i].normal[0] = normal.x;
        vertices[i].normal[1] = normal.y;
        vertices[i].normal[2] = normal.z;

        // tangent
        const auto& tangent = pMesh->mTangents[i];
        vertices[i].tangent[0] = tangent.x;
        vertices[i].tangent[1] = tangent.y;
        vertices[i].tangent[2] = tangent.z;

        // color ���� material���� �ϴ� �� �´�.
    }

    // indices
    auto numIndices = pMesh->mNumFaces * 3;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
    {
        auto& face = pMesh->mFaces[i];
        indices.emplace_back(face.mIndices[0]);
        indices.emplace_back(face.mIndices[1]);
        indices.emplace_back(face.mIndices[2]);
    }

    // �� �̾Ƴ� �� 
    
    // Model�� geometry data�� �����Ѵ�.
    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0;
    m_pModel->AppendGeometry(vertices, indices, &vertexOffset, &indexOffset);

    // Renderable�� �����ϰ� �����͸� �����Ѵ�.
    auto pMeshRenderable = pGameObject->AddComponent<Dive::MeshRenderable>();
    pMeshRenderable->SetGeometry(
        pGameObject->GetName(),
        vertexOffset,
        numVertices,
        indexOffset,
        numIndices,
        m_pModel);

    // material

    // bone
}
