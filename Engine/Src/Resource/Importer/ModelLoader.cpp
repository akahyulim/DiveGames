#include "divepch.h"
#include "ModelLoader.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "IO/FileSystem.h"
#include "Math/BoundingBox.h"
#include "Resource/ResourceManager.h"
#include "World/SceneManager.h"
#include "World/GameObject.h"
#include "World/Components/Renderable.h"

namespace Dive
{
    const aiScene* ModelLoader::s_aiScene = nullptr;

    std::string ModelLoader::s_FileName;
    std::string ModelLoader::s_ModelName;

    Scene* ModelLoader::s_pActiveScene = nullptr;
    Model* ModelLoader::s_pModel = nullptr;

    static DirectX::XMFLOAT3 ConvertXMFLOAT3(const aiVector3D& vec)
    {
        return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
    }

    static DirectX::XMFLOAT4 ConvertXMFLOAT4(const aiQuaternion& vec)
    {
        return DirectX::XMFLOAT4(vec.x, vec.y, vec.z, vec.w);
    }

    static DirectX::XMFLOAT4X4 ConvertXMFLOAT4X4(const aiMatrix4x4& mat)
    {
        return DirectX::XMFLOAT4X4(
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        );
    }

    static void GetBlendData(aiMesh* pMesh, std::vector<std::vector<int>>& outBlendIndices, std::vector<std::vector<float>>& outBlendWeights)
    {
        if (!pMesh || pMesh->mNumBones == 0)
            return;

        outBlendIndices.resize(pMesh->mNumVertices);
        outBlendWeights.resize(pMesh->mNumVertices);

        for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
        {
            aiBone* pBone = pMesh->mBones[i];
            for (uint32_t j = 0; j < pBone->mNumWeights; ++j)
            {
                uint32_t vertexID = pBone->mWeights[j].mVertexId;
                outBlendIndices[vertexID].emplace_back(i);
                outBlendWeights[vertexID].emplace_back(pBone->mWeights[j].mWeight);
            }
        }

        // outBlendWeights를 정규화
        for (uint32_t i = 0; i < static_cast<uint32_t>(outBlendWeights.size()); ++i)
        {
            // 1. 뼈대가 4개를 초과할 경우 가장 작은 wegiht와 bone을 제거
            if (outBlendWeights[i].size() > 4)
            {
                DV_LOG(ModelLoader, warn, "{:d}번째 정점에 영향을 주는 뼈대가 4개 이상입니다.", i);

                while (outBlendWeights[i].size() > 4)
                {
                    size_t lowestIndex = 0;
                    float lowest = std::numeric_limits<float>::infinity();
                    for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
                    {
                        if (outBlendWeights[i][j] < lowest)
                        {
                            lowest = outBlendWeights[i][j];
                            lowestIndex = j;
                        }
                    }

                    outBlendWeights[i].erase(outBlendWeights[i].begin() + lowestIndex);
                    outBlendIndices[i].erase(outBlendIndices[i].begin() + lowestIndex);
                }
            }

            // 2. 총 합이 1.0 이상이거나 0.0이 아닐 경우 정규화
            float sum = 0.0f;
            for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
                sum += outBlendWeights[i][j];
            if (sum != 1.0f && sum != 0.0f)
            {
                DV_LOG(ModelLoader, warn, "Weight 총합이 1.0이 아니어서 정규화를 수행");

                for (uint32_t j = 0; j < static_cast<uint32_t>(outBlendWeights[i].size()); ++j)
                    outBlendWeights[i][j] /= sum;
            }
        }
    }

    bool ModelLoader::Load(Model* pModel, const std::string& fileName)
    {
        s_FileName = fileName;
        s_ModelName = FileSystem::GetFileName(fileName);
        s_pModel = pModel;
        s_pModel->SetName(s_ModelName);

        Assimp::Importer importer;

        uint32_t flags =
            aiProcess_LimitBoneWeights |
            aiProcess_ConvertToLeftHanded |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace;

        // 이게 빈 노드들을 제거해주었다.
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        s_aiScene = importer.ReadFile(fileName, flags);
        if (!s_aiScene || s_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !s_aiScene->mRootNode)
        {
            DV_LOG(ModelLoader, err, "{:s}", importer.GetErrorString());
            return false;
        }

        s_pActiveScene = SceneManager::GetInstance()->GetActiveScene();
        if (!s_pActiveScene)
        {
            DV_LOG(ModelLoader, err, "모델 파일을 저장할 활성화된 Scene이 존재하지 않습니다.");
            return false;
        }

        // 루트노드부터 파싱
        parseNode(s_aiScene->mRootNode);

        importer.FreeScene();

        return true;
    }

    void ModelLoader::parseNode(const aiNode* pNode, GameObject* pParent)
    {
        auto pNodeObject = s_pActiveScene->CreateGameObject();
        pNodeObject->SetName(pParent ? pNode->mName.data : s_ModelName);
        pNodeObject->SetParent(pParent);
        pNodeObject->SetLocalTransform(ConvertXMFLOAT4X4(pNode->mTransformation));

        if (!pParent)
            s_pModel->SetRootObject(pNodeObject);

        // 이건 노드에 둘 이상의 메시가 존재할 경우
        // 게임오브젝트를 추가하고 이름을 +i로 설정하는 것이다.
        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            auto pMeshNodeObject = pNodeObject;
            auto pMesh = s_aiScene->mMeshes[pNode->mMeshes[i]];
            std::string meshName = pMesh->mName.C_Str();

            if (i > 0)
            {
                pMeshNodeObject = s_pActiveScene->CreateGameObject();
                pMeshNodeObject->SetParent(pNodeObject);
                pMeshNodeObject->SetLocalTransform(ConvertXMFLOAT4X4(pNode->mTransformation));

                // 동일한 이름을 가지는 경우가 있어 인덱스를 추가
                // 이전 이름들과 비교하는 방법도 시도해보자.
                meshName += "_" + std::to_string(i + 1);
            }

            // 메시의 이름은 게임오브젝트가 관리하게 된다.
            pMeshNodeObject->SetName(meshName);

            parseMesh(pMesh, pMeshNodeObject);
        }

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
            parseNode(pNode->mChildren[i], pNodeObject);
    }

    void ModelLoader::parseMesh(const aiMesh* pMesh, GameObject* pMeshNodeObject)
    {
        // indices
        uint32_t numIndices = pMesh->mNumFaces * 3;
        std::vector<uint32_t> indices;
        indices.resize(numIndices);

        for (uint32_t faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex)
        {
            const auto& face = pMesh->mFaces[faceIndex];
            const uint32_t index = (faceIndex * 3);
            indices[index] = face.mIndices[0];
            indices[(size_t)index + 1] = face.mIndices[1];
            indices[(size_t)index + 2] = face.mIndices[2];
        }

        // Mesh가 StaticVertices와 SkinnedVertices를 구분해서 관리한다면
        // 무리없이 적용가능하다.

        // vertices
        uint32_t numVertices = pMesh->mNumVertices;
        //if (!pMesh->mNumBones)
        {
            std::vector<VertexStatic> vertices;
            vertices.resize(numVertices);

            for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
            {
                VertexStatic& vertex = vertices[vertexIndex];

                // position
                const auto& position = pMesh->mVertices[vertexIndex];
                vertex.position[0] = position.x;
                vertex.position[1] = position.y;
                vertex.position[2] = position.z;

                // texCoords
                const uint32_t uvChannel = 0;
                if (pMesh->HasTextureCoords(uvChannel))
                {
                    const auto& texCoords = pMesh->mTextureCoords[uvChannel][vertexIndex];
                    vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                    vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                }

                // normal
                if (pMesh->mNormals)
                {
                    const auto& normal = pMesh->mNormals[vertexIndex];
                    vertex.normal[0] = normal.x;
                    vertex.normal[1] = normal.y;
                    vertex.normal[2] = normal.z;
                }

                // tangent
                if (pMesh->mTangents)
                {
                    const auto& tangent = pMesh->mTangents[vertexIndex];
                    vertex.tangent[0] = tangent.x;
                    vertex.tangent[1] = tangent.y;
                    vertex.tangent[2] = tangent.z;
                }

                // bitangent
                if (pMesh->mBitangents)
                {
                    const auto& bitangent = pMesh->mBitangents[vertexIndex];
                    vertex.bitangent[0] = bitangent.x;
                    vertex.bitangent[1] = bitangent.y;
                    vertex.bitangent[2] = bitangent.z;
                }
            }

            Mesh* pAddedMesh = new Mesh;
            pAddedMesh->SetName(pMesh->mName.C_Str());
           
            uint32_t vertexOffset = 0;
            pAddedMesh->AddVertices(vertices, &vertexOffset);
            // 오프셋을 전달하는 이유는 하나의 버퍼에 메시를 누적하는 과정인 듯 하다.
            // 허나 현재 메시당 버퍼가 하나이므로 오프셋을 저장할 필요가 없다. 실제로 전부 0이다.
            uint32_t indexOffset = 0;
            pAddedMesh->AddIndices(indices, &indexOffset);

            pAddedMesh->ComputeBouingBox();
            pAddedMesh->CreateBuffers();

            auto pMeshRenderer = pMeshNodeObject->AddComponent<Renderable>();
            pMeshRenderer->SetGeometry(pAddedMesh, vertexOffset, (uint32_t)vertices.size(), indexOffset, (uint32_t)indices.size());
            // 일단 aiMaterial이 존재하는지 부터 확인해야 한다.
            // 없다면 디폴트 머티리얼을 전달하는 것이 맞다.
            pMeshRenderer->SetMaterial(loadMaterial(pMesh));

            s_pModel->AddMesh(pAddedMesh);
        }
        /*
        else
        {
            std::vector<std::vector<int>> blendIndices;
            std::vector<std::vector<float>> blendWeights;
            GetBlendData(pMesh, blendIndices, blendWeights);

            std::vector<VertexSkinned> vertices;
            vertices.resize(numVertices);

            for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
            {
                VertexSkinned& vertex = vertices[vertexIndex];

                // position
                const auto& position = pMesh->mVertices[vertexIndex];
                vertex.position[0] = position.x;
                vertex.position[1] = position.y;
                vertex.position[2] = position.z;

                // texCoords
                const uint32_t uvChannel = 0;
                if (pMesh->HasTextureCoords(uvChannel))
                {
                    const auto& texCoords = pMesh->mTextureCoords[uvChannel][vertexIndex];
                    vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                    vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                }

                // normal
                if (pMesh->mNormals)
                {
                    const auto& normal = pMesh->mNormals[vertexIndex];
                    vertex.normal[0] = normal.x;
                    vertex.normal[1] = normal.y;
                    vertex.normal[2] = normal.z;
                }

                // tangent
                if (pMesh->mTangents)
                {
                    const auto& tangent = pMesh->mTangents[vertexIndex];
                    vertex.tangent[0] = tangent.x;
                    vertex.tangent[1] = tangent.y;
                    vertex.tangent[2] = tangent.z;
                }

                // boneIDs
                for (uint32_t j = 0; j < static_cast<uint32_t>(blendIndices[vertexIndex].size()); ++j)
                    vertex.boneIDs[j] = blendIndices[vertexIndex][j];

                // weights
                for (uint32_t j = 0; j < static_cast<uint32_t>(blendWeights[vertexIndex].size()); ++j)
                    vertex.weights[j] = blendWeights[vertexIndex][j];
            }

            // 현재 하나의 노드에 둘 이상의 메시가 존재할 경우
            // 메시를 추가로 생성하고 있다.
            // 이때 아래의 과정도 함께 수행되는데
            // 정작 Model은 하나의 Skeleton만 가질 수 있다.
            Skeleton skeleton;
            auto& bones = skeleton.GetModifiableBones();
            bones.resize(pMesh->mNumBones);
            // 현재 스켈레톤이 두 개 이상일 수 있지만 model에서 하나만 관리한다.
            // 따라서 아에 참고문서처럼 model에서 하나만 관리토록 할 생각이다.
            auto& boneInfoMap = m_pImportedModel->GetBoneInfoMap();

            // 현재 메시가 사용하는 뼈대의 정보를 모아서 관리하는 것이다.
            for (uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
            {
                aiBone* pBone = pMesh->mBones[boneIndex];
                bones[boneIndex].name = pBone->mName.C_Str();
                bones[boneIndex].index = boneIndex;
                bones[boneIndex].offsetMatrix = ConvertXMFLOAT4X4(pBone->mOffsetMatrix);

                // id는... 문서를 좀 더 보자.
                std::string boneName = pBone->mName.C_Str();
                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    BoneInfo newBoneInfo;
                    newBoneInfo.index = boneIndex;
                    newBoneInfo.offsetMatrix = ConvertXMFLOAT4X4(pBone->mOffsetMatrix);
                    boneInfoMap[boneName] = newBoneInfo;
                }
            }
            m_pImportedModel->SetSkeleton(skeleton);

            auto pSkinnedMesh = new SkinnedMesh(pMeshNodeObject->GetName(), vertices, indices);
            m_pImportedModel->InsertSkinnedMesh(pSkinnedMesh);

            // 이부분에서 오류가 난 적이 있다. 하지만 특별한 조치없이 고쳐졌다. 찝찝하다.
            auto pMeshRenderer = pMeshNodeObject->AddComponent<SkinnedMeshRenderer>();
            pMeshRenderer->SetMesh(pSkinnedMesh);
            pMeshRenderer->SetMaterial(loadMaterial(pMesh));

            // temp: 이 곳에서 SkinnedMeshRenderer에 BoneInfo를 전달할 수 있다.
            pMeshRenderer->SetBones(bones);

            // temp
            auto pAnimator = pMeshNodeObject->AddComponent<Animator>();
            // 아직 애니메이션이 구성되지 않았다.
            //pAnimator->SetAnimation(ResourceCache::GetResourceByName<Animation>("TestAnim"));
        }
        */
    }

    Material* ModelLoader::loadMaterial(const aiMesh* pMesh)
    {
        auto pMaterial = s_aiScene->mMaterials[pMesh->mMaterialIndex];
        if (!pMaterial)
        {
            // 그래픽 혹은 렌더러에서 생성한 디폴트 머티리얼을 가져와야 한다.
            return nullptr;
        }

        aiString name;
        aiGetMaterialString(pMaterial, AI_MATKEY_NAME, &name);

        // 일단 동일한 이름의 머티리얼이 존재한다면 리턴하도록 했지만
        // 추후 동일한 이름을 허용해야할 수도 있다.
        // 이건 현재 적용이 불안정한 듯 하다. => 역시 안먹힌다.
        //auto pExisted = ResourceManager::GetInstance()->GetResource<Material>(name.C_Str());
        //if (pExisted)
        //    return pExisted;

        aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
        aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
        aiGetMaterialColor(pMaterial, AI_MATKEY_OPACITY, &opacity);

        auto pMat = new Material();
        pMat->SetName(name.data);
        pMat->SetDiffuseColor(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
        //ResourceManager::GetInstance()->AddManualResource(pMat);
        // 이걸 어디에서 해야하나... 이렇게 직접 전달하기 싫다면
        // Material::GetShader()에서 리소스매니저로부터 직접 디폴트 셰이더를 전달하는 방법도 있다.
        //auto pShader = ResourceManager::GetInstance()->GetResource<Shader>("../../Assets/Shaders/ForwardLight.hlsl");
        //pMat->SetShader(pShader);

        aiString texturePath;

        // 맵핑 텍스쳐 로드 및 등록
        // static 함수로 뽑아내려 했으나 일단 실패했다.
        // diff map
        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            Texture2D* pDiffuseTex = nullptr;
            const aiTexture* pEmbeddedTex = s_aiScene->GetEmbeddedTexture(texturePath.C_Str());
            if (pEmbeddedTex)
            {
                // 파일 내부에서 데이터를 얻어와 직접 만들기
                pDiffuseTex = loadEmbeddedTexture(pEmbeddedTex);
            }
            else
            {
                // 이건 외부에 존재하는 파일을 로드
                auto diffuseTexturePath = FileSystem::GetPath(s_FileName) + "textures/";
                diffuseTexturePath += FileSystem::GetFileNameAndExtension(texturePath.C_Str());
                //pDiffuseTex = ResourceManager::GetInstance()->GetResource<Texture2D>(diffuseTexturePath);
                pDiffuseTex = ResourceManager::GetInstance()->GetResource<Texture2D>(diffuseTexturePath);
            }
            pMat->SetDvTexture(eTextureUnitType::Diffuse, pDiffuseTex);
            DV_LOG(ModelLoader, info, "Load DiffuseMap");
        }

        // normal map
        if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            Texture2D* pNormalTex = nullptr;
            const aiTexture* pEmbeddedTex = s_aiScene->GetEmbeddedTexture(texturePath.C_Str());
            if (pEmbeddedTex)
            {
                // 파일 내부에서 데이터를 얻어와 직접 만들기
                pNormalTex = loadEmbeddedTexture(pEmbeddedTex);
            }
            else
            {
                // 이건 외부에 존재하는 파일을 로드
                auto normalTexturePath = FileSystem::GetPath(s_FileName) + "textures/";
                normalTexturePath += FileSystem::GetFileNameAndExtension(texturePath.C_Str());
                //pNormalTex = ResourceManager::GetInstance()->GetResource<Texture2D>(normalTexturePath);
                pNormalTex = ResourceManager::GetInstance()->GetResource<Texture2D>(normalTexturePath);
            }
            pMat->SetDvTexture(eTextureUnitType::Normal, pNormalTex);
            DV_LOG(ModelLoader, info, "Load NormalMap");
        }
        
        return pMat;
    }

    // https://github.com/assimp/assimp/blob/master/samples/SimpleTexturedDirectx11/SimpleTexturedDirectx11/ModelLoader.cpp
    Texture2D* ModelLoader::loadEmbeddedTexture(const aiTexture* pEmbeddedTex)
    {
        Texture2D* pLoadedTex = nullptr;

        // 압축되지 않은 이미지
        if (pEmbeddedTex->mHeight != 0)
        {
            pLoadedTex = new Texture2D(pEmbeddedTex->mWidth, pEmbeddedTex->mHeight);
            pLoadedTex->UpdateSubresource(
                (const void*)pEmbeddedTex->pcData, 
                Texture::CalcuRowPitchSize(pEmbeddedTex->mWidth, 
                    DXGI_FORMAT_R8G8B8A8_UNORM));
        }
        // 압축된 이미지
        else
        {
            const size_t bufferSize = static_cast<size_t>(pEmbeddedTex->mWidth);
            const void* pSrcData = (const void*)pEmbeddedTex->pcData;

            pLoadedTex = new Texture2D;
            pLoadedTex->LoadFromMemory(
                pEmbeddedTex->mFilename.C_Str(), 
                static_cast<size_t>(pEmbeddedTex->mWidth), 
                (const void*)pEmbeddedTex->pcData);

        }

        return pLoadedTex;
    }
}