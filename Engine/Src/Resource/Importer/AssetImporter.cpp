#include "DivePch.h"
#include "AssetImporter.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceCache.h"
#include "Graphics/GraphicsDefs.h"
#include "Renderer/Bone.h"
#include "Renderer/Mesh.h"
#include "Renderer/SkinnedMesh.h"
#include "Renderer/Model.h"
#include "Renderer/Animation.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"
#include "IO/FileStream.h"
#include "IO/Log.h"


namespace Dive
{
    static DirectX::XMFLOAT4X4 ConvertMatrix(const aiMatrix4x4& transform)
    {
        return DirectX::XMFLOAT4X4
        (
            transform.a1, transform.b1, transform.c1, transform.d1,
            transform.a2, transform.b2, transform.c2, transform.d2,
            transform.a3, transform.b3, transform.c3, transform.d3,
            transform.a4, transform.b4, transform.c4, transform.d4
        );
    }

    static DirectX::XMFLOAT3 ConvertVector3(const aiVector3D& vector)
    {
        return DirectX::XMFLOAT3{
            vector.x,
            vector.y,
            vector.z
        };
    }

    static DirectX::XMFLOAT4 ConvertQuaternion(const aiQuaternion& quaternion)
    {
        return DirectX::XMFLOAT4{
            quaternion.x,
            quaternion.y,
            quaternion.z,
            quaternion.w
        };
    }

    static aiNode* GetNodeByName(aiNode* pNode, std::string name)
    {
        if (pNode->mName.C_Str() == name)
        {
            return pNode;
        }
        else
        {
            for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
            {
                aiNode* pTarget = GetNodeByName(pNode->mChildren[i], name);
                if (pTarget != nullptr)
                    return pTarget;
            }
        }

        return nullptr;
    }

    static uint32_t GetNumValidFaces(aiMesh* pMesh)
    {
        uint32_t ret = 0;

        for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            if (pMesh->mFaces[i].mNumIndices == 3)
                ++ret;
        }

        return ret;
    }

    AssetImporter::AssetImporter()
        : m_BoneCounter(0),
        m_pModel(nullptr)
    {
    }
    
    AssetImporter::~AssetImporter()
    {
    }

    bool AssetImporter::LoadFromFile(const std::string& filePath)
    {
        Assimp::Importer importer;
        //importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
        //importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
        //importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1000000);
        //importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1000000);
        //importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
        //importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
        //importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

        uint32_t spartan_flags =
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

        uint32_t urho3d_flags = 
            aiProcess_ConvertToLeftHanded |
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_LimitBoneWeights |
            aiProcess_ImproveCacheLocality |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_FixInfacingNormals |
            aiProcess_FindInvalidData |
            aiProcess_GenUVCoords |
            aiProcess_FindInstances |
            aiProcess_OptimizeMeshes;

        uint32_t doc_flags =
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace;

        const aiScene* pScene = importer.ReadFile(filePath,
            //spartan_flags);
            //urho3d_flags);
            //doc_flags);
            aiProcess_ConvertToLeftHanded |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace);
        if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
        {
            DV_CORE_ERROR("{:s}", importer.GetErrorString());
            return false;
        }

        // CreateAndBuildModel
        m_pModel = new Model;
        m_pModel->SetName(FileSystem::GetFileName(filePath));
        ResourceCache::Cache<Model>(m_pModel);
        parseNodes(pScene, pScene->mRootNode, &m_pModel->GetRootNodeInfo());
        m_pModel->Build();

        // CreateAndBuildAnimation
        aiAnimation* anim = pScene->mAnimations[0];
        m_pAnimation = new Animation(
            anim->mName.C_Str(),
            static_cast<float>(anim->mDuration),
            static_cast<float>(anim->mTicksPerSecond),
            anim->mNumChannels);
        ResourceCache::Cache<Animation>(m_pAnimation);
        processAnimation(anim);
        m_pAnimation->SetRootGameObject(m_pModel->GetRootGameObject());     // 다시 살펴보기

        /*
        // 계층구조와 mesh를 파싱하면서 model을 구성
        processNode(pScene, pScene->mRootNode);
        // 구성된 model의 mesh를 build
        m_pModel->BuildMeshBuffers();

        // animaiton 생성 및 cache
        aiAnimation* anim = pScene->mAnimations[0];
        m_pAnimation = new Animation(
            anim->mName.C_Str(), 
            static_cast<float>(anim->mDuration),
            static_cast<float>(anim->mTicksPerSecond),
            anim->mNumChannels);
        ResourceCache::Cache<Animation>(m_pAnimation);

        processAnimation(pScene->mAnimations[0]);
        */

        importer.FreeScene();

        return true;
    }

    void AssetImporter::extractBoneWeightForVertices(std::vector<VertexSkinned>& vertices, aiMesh* pMesh, const aiScene* pScene)
    {
        // bone name이 key, bone id와 offset이 value
        auto& boneInfoMap = m_pModel->GetBoneInfoMap();

        // 현재 mesh에 영향을 주는 bones
        for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
        {
            int boneID = -1;
            aiBone* pBone = pMesh->mBones[i];
            std::string boneName = pBone->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneID = m_BoneCounter++;
                newBoneInfo.offsetTransform = ConvertMatrix(pBone->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }

            DV_ASSERT(boneID != -1);

            // 현재 bone이 영향을 주는 wights들
            // bone은 mesh를 구성하는 다수의 vertex들에게 영향을 주므로
            // mnumWeights는 영향을 주는 vertices 개수,
            // weights에는 영향을 주는 vertex의 index가 들어있다.
            auto weights = pBone->mWeights;
            int numWeights = pBone->mNumWeights;

            for (int j = 0; j < numWeights; ++j)
            {
                uint32_t vertexID = weights[j].mVertexId;
                float weight = weights[j].mWeight;
                DV_ASSERT(vertexID <= static_cast<uint32_t>(vertices.size()));
                setVertexBoneData(vertices[vertexID], boneID, weight);

                //DV_CORE_DEBUG("{0:d}번째 bone, {1:d}번째 weight vertexID: {2:d}, weight: {3:f}", i, j, vertexID, weight);
            }
        }
    }

    void AssetImporter::setVertexBoneData(VertexSkinned& vertex, int boneID, float weight)
    {
        for (uint32_t i = 0; i < 4; ++i)
        {
            if (vertex.boneIDs[i] < 0)
            {
                vertex.boneIDs[i] = boneID;
                vertex.weights[i] = weight;
                break;
            }
        }
    }

    void AssetImporter::processNode(const aiScene* pScene, aiNode* pNode, GameObject* pParentGameObject)
    {
        GameObject* pNewNode = Scene::CreateGameObject();//new GameObject;
        pNewNode->SetName(pNode->mName.C_Str());
        pNewNode->GetTransform()->SetLocalMatrix(ConvertMatrix(pNode->mTransformation));

        bool bRootNode = pParentGameObject == nullptr;
        if (bRootNode)
        {
            m_pModel->SetRootGameObject(pNewNode);
            pNewNode->GetTransform()->SetParent(nullptr);
        }
        else
        {
            pNewNode->GetTransform()->SetParent(pParentGameObject->GetTransform());
        }

        if (pNode->mMeshes)
            processMeshes(pScene, pNode, pNewNode);

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        {
            processNode(pScene, pNode->mChildren[i], pNewNode);
        }
    }

    void AssetImporter::processMeshes(const aiScene* pScene, aiNode* pNode, GameObject* pNodeGameObject)
    {
        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
            const std::string& name = pMesh->mName.C_Str();
            
            pNodeGameObject->SetName(name);

            // indices
            uint32_t numIndices = pMesh->mNumFaces * 3;
            std::vector<uint32_t> indices;
            indices.resize(numIndices);

            for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
            {
                const auto& face = pMesh->mFaces[i];
                const uint32_t index = (i * 3);
                indices[index] = face.mIndices[0];
                indices[index + 1] = face.mIndices[1];
                indices[index + 2] = face.mIndices[2];
            }

            // vertices
            uint32_t numVertices = pMesh->mNumVertices;
            if (!pMesh->mNumBones)
            {
                std::vector<VertexStatic> vertices;
                vertices.resize(numVertices);

                for (uint32_t i = 0; i < numVertices; ++i)
                {
                    VertexStatic& vertex = vertices[i];

                    // position
                    const auto& position = pMesh->mVertices[i];
                    vertex.position[0] = position.x;
                    vertex.position[1] = position.y;
                    vertex.position[2] = position.z;

                    // texCoords
                    const uint32_t uvChannel = 0;
                    if (pMesh->HasTextureCoords(uvChannel))
                    {
                        const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                        vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                        vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                    }

                    // normal
                    if (pMesh->mNormals)
                    {
                        const auto& normal = pMesh->mNormals[i];
                        vertex.normal[0] = normal.x;
                        vertex.normal[1] = normal.y;
                        vertex.normal[2] = normal.z;
                    }

                    // tangent
                    if (pMesh->mTangents)
                    {
                        const auto& tangent = pMesh->mTangents[i];
                        vertex.tangent[0] = tangent.x;
                        vertex.tangent[1] = tangent.y;
                        vertex.tangent[2] = tangent.z;
                    }
                }

                // model에 넣어서 mesh를 형성하고
                //DvStaticMesh* pStaticMesh = m_pModel->InsertStaticMesh(DvStaticMesh(pNode->mName.C_Str(), name, vertices, indices));

                // game object에 meshRenderer를 추가하고 mesh를 연결
                //MeshRenderer* pMeshRenderer = pNodeGameObject->AddComponent<MeshRenderer>();
                //pMeshRenderer->SetMesh(pStaticMesh);
            }
            else
            {
                std::vector<VertexSkinned> vertices;
                vertices.resize(numVertices);

                for (uint32_t i = 0; i < numVertices; ++i)
                {
                    VertexSkinned& vertex = vertices[i];

                    // position
                    const auto& position = pMesh->mVertices[i];
                    vertex.position[0] = position.x;
                    vertex.position[1] = position.y;
                    vertex.position[2] = position.z;

                    // texCoords
                    const uint32_t uvChannel = 0;
                    if (pMesh->HasTextureCoords(uvChannel))
                    {
                        const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                        vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                        vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                    }

                    // normal
                    if (pMesh->mNormals)
                    {
                        const auto& normal = pMesh->mNormals[i];
                        vertex.normal[0] = normal.x;
                        vertex.normal[1] = normal.y;
                        vertex.normal[2] = normal.z;
                    }

                    // tangent
                    if (pMesh->mTangents)
                    {
                        const auto& tangent = pMesh->mTangents[i];
                        vertex.tangent[0] = tangent.x;
                        vertex.tangent[1] = tangent.y;
                        vertex.tangent[2] = tangent.z;
                    }
                }

                extractBoneWeightForVertices(vertices, pMesh, pScene);

                // model에 넣어서 mesh를 형성하고
                //SkinnedMesh* pSkinnedMesh = m_pModel->InsertSkinnedMesh(SkinnedMesh(pNode->mName.C_Str(), name, vertices, indices));

                // game object에 meshRenderer를 추가하고 mesh를 연결
                //SkinnedMeshRenderer* pSkinnedMeshRenderer = pNodeGameObject->AddComponent<SkinnedMeshRenderer>();
                //pSkinnedMeshRenderer->SetMesh(pSkinnedMesh);
            }
        }
    }

    void AssetImporter::processAnimation(aiAnimation* pAnimation)
    {
        uint32_t numChannels = pAnimation->mNumChannels;
        auto& boneInfoMap = m_pModel->GetBoneInfoMap();
        uint32_t boneCount = m_pModel->GetBoneCount();

        // 채널단위 접근
        // 결국 채널이란게 뼈대별 key 정보라는 의미인듯....
        for (uint32_t i = 0; i < numChannels; ++i)
        {
            aiNodeAnim* pChannel = pAnimation->mChannels[i];
            std::string boneName = pChannel->mNodeName.C_Str();

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;

                DV_CORE_DEBUG("read missing bone: {:s}", boneName);
            }

            // create bone
            {
                uint32_t numPosKeys = pChannel->mNumPositionKeys;
                std::vector<KeyPosition> positionKeys;
                positionKeys.reserve(numPosKeys);
                for (uint32_t j = 0; j < numPosKeys; ++j)
                {
                    double time = pChannel->mPositionKeys[j].mTime;
                    DirectX::XMFLOAT3 value = ConvertVector3(pChannel->mPositionKeys[j].mValue);

                    positionKeys.emplace_back(time, value);
                }
                
                uint32_t numRotKeys = pChannel->mNumRotationKeys;
                std::vector<KeyRotation> rotationKeys;
                rotationKeys.reserve(numRotKeys);
                for (uint32_t j = 0; j < numRotKeys; ++j)
                {
                    double time = pChannel->mRotationKeys[j].mTime;
                    DirectX::XMFLOAT4 value = ConvertQuaternion(pChannel->mRotationKeys[j].mValue);

                    rotationKeys.emplace_back(time, value);
                }
                
                uint32_t numSclKeys = pChannel->mNumScalingKeys;
                std::vector<KeyScale> scaleKeys;
                scaleKeys.reserve(numSclKeys);
                for (uint32_t j = 0; j < numSclKeys; ++j)
                {
                    double time = pChannel->mScalingKeys[j].mTime;
                    DirectX::XMFLOAT3 value = ConvertVector3(pChannel->mScalingKeys[j].mValue);

                    scaleKeys.emplace_back(time, value);
                }

                // 채널에 매칭된 뼈대를 생성한 후 애니메이션에 추가
                Bone bone(boneName, boneInfoMap[boneName].id);
                bone.SetPositions(positionKeys);
                bone.SetRotations(rotationKeys);
                bone.SetScales(scaleKeys);

                m_pAnimation->InsertBone(bone);
            }
        }

        m_pAnimation->SetBoneIDMap(boneInfoMap);
    }

    void AssetImporter::parseNodes(const aiScene* pScene, aiNode* pNode, NodeInfo* pNodeInfo)
    {
        // nodeInfo는 Model의 RootNodeInfo부터 시작해 재귀적으로 저장된다.
        pNodeInfo->name = pNode->mName.C_Str();
        pNodeInfo->transform = ConvertMatrix(pNode->mTransformation);
        pNodeInfo->numChildren = pNode->mNumChildren;
        pNodeInfo->children.resize(pNode->mNumChildren);

        if (pNode->mMeshes) 
        {
            parseMeshes(pScene, pNode);
        }

        for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
        {
            parseNodes(pScene, pNode->mChildren[i], &pNodeInfo->children[i]);
        }
    }

    void AssetImporter::parseMeshes(const aiScene* pScene, aiNode* pNode)
    {
        for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
        {
            aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

            // indices
            uint32_t numIndices = pMesh->mNumFaces * 3;//GetNumValidFaces(pMesh) * 3;
            std::vector<uint32_t> indices;
            indices.resize(numIndices);

            for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
            {
                const auto& face = pMesh->mFaces[i];
                const uint32_t index = (i * 3);
                indices[index] = face.mIndices[0];
                indices[index + 1] = face.mIndices[1];
                indices[index + 2] = face.mIndices[2];
            }

            // vertices
            uint32_t numVertices = pMesh->mNumVertices;
            if (!pMesh->mNumBones)
            {
                std::vector<VertexStatic> vertices;
                vertices.resize(numVertices);

                for (uint32_t i = 0; i < numVertices; ++i)
                {
                    VertexStatic& vertex = vertices[i];

                    // position
                    const auto& position = pMesh->mVertices[i];
                    vertex.position[0] = position.x;
                    vertex.position[1] = position.y;
                    vertex.position[2] = position.z;

                    // texCoords
                    const uint32_t uvChannel = 0;
                    if (pMesh->HasTextureCoords(uvChannel))
                    {
                        const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                        vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                        vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                    }

                    // normal
                    if (pMesh->mNormals)
                    {
                        const auto& normal = pMesh->mNormals[i];
                        vertex.normal[0] = normal.x;
                        vertex.normal[1] = normal.y;
                        vertex.normal[2] = normal.z;
                    }

                    // tangent
                    if (pMesh->mTangents)
                    {
                        const auto& tangent = pMesh->mTangents[i];
                        vertex.tangent[0] = tangent.x;
                        vertex.tangent[1] = tangent.y;
                        vertex.tangent[2] = tangent.z;
                    }
                }

                // model에 넣어서 mesh를 형성하고
                //m_pModel->InsertStaticMesh(DvStaticMesh(pNode->mName.C_Str(), pMesh->mName.C_Str(), vertices, indices));
            }
            else
            {
                std::vector<VertexSkinned> vertices;
                vertices.resize(numVertices);

                for (uint32_t i = 0; i < numVertices; ++i)
                {
                    VertexSkinned& vertex = vertices[i];

                    // position
                    const auto& position = pMesh->mVertices[i];
                    vertex.position[0] = position.x;
                    vertex.position[1] = position.y;
                    vertex.position[2] = position.z;

                    // texCoords
                    const uint32_t uvChannel = 0;
                    if (pMesh->HasTextureCoords(uvChannel))
                    {
                        const auto& texCoords = pMesh->mTextureCoords[uvChannel][i];
                        vertex.texCoords[0] = texCoords.x > 1.0f ? texCoords.x - 1.0f : texCoords.x;
                        vertex.texCoords[1] = texCoords.y > 1.0f ? texCoords.y - 1.0f : texCoords.y;
                    }

                    // normal
                    if (pMesh->mNormals)
                    {
                        const auto& normal = pMesh->mNormals[i];
                        vertex.normal[0] = normal.x;
                        vertex.normal[1] = normal.y;
                        vertex.normal[2] = normal.z;
                    }

                    // tangent
                    if (pMesh->mTangents)
                    {
                        const auto& tangent = pMesh->mTangents[i];
                        vertex.tangent[0] = tangent.x;
                        vertex.tangent[1] = tangent.y;
                        vertex.tangent[2] = tangent.z;
                    }
                }

                //extractBoneWeightForVertices(vertices, pMesh, pScene);

                //m_pModel->InsertSkinnedMesh(SkinnedMesh(pNode->mName.C_Str(), pMesh->mName.C_Str(), vertices, indices));

                /*
                for (size_t i = 0; i < vertices.size(); ++i)
                {
                    DV_CORE_DEBUG("{0:n}번째 vertex bondID: {1:d}, {2:d}, {3:d}, {4:d}", 
                        i, vertices[i].boneIDs[0], vertices[i].boneIDs[1], vertices[i].boneIDs[2], vertices[i].boneIDs[3]);

                    float totalWeight = vertices[i].weights[0] + vertices[i].weights[1] + vertices[i].weights[2] + vertices[i].weights[3];
                    if (totalWeight > 1.0f)
                    {
                        DV_CORE_ERROR("{0:n}번째 vertex weight: {1:f}, {2:f}, {3:f}, {4:f}, total weights: {5:f}",
                            i, vertices[i].weights[0], vertices[i].weights[1], vertices[i].weights[2], vertices[i].weights[3], totalWeight);
                    }
                    else
                    {
                        DV_CORE_DEBUG("{0:n}번째 vertex weight: {1:f}, {2:f}, {3:f}, {4:f}, total weights: {5:f}",
                            i, vertices[i].weights[0], vertices[i].weights[1], vertices[i].weights[2], vertices[i].weights[3], totalWeight);
                    }
                }
                */
            }
        }
    }

}