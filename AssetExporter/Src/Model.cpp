#include "Model.h"
#include "AssimpHelpers.h"

Model::Model(const std::string& filePath)
	: m_BoneCounter(0)
{
	loadModel(filePath);
}

Model::~Model()
{

}

void Model::loadModel(const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(pScene->mRootNode, pScene);
}

void Model::processNode(aiNode* pNode, const aiScene* pScene) 
{
	for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		m_Meshes.emplace_back(processMesh(pMesh, pScene));
	}

	for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
	{
		processNode(pNode->mChildren[i], pScene);
	}
}

// vertices, indices, texture를 구성한 후 Mesh를 생성하여 리턴
// 그리고 이 과정에서 vertex의 boneID, weight가 저장된다.
Mesh Model::processMesh(aiMesh* pMesh, const aiScene* pScene)
{
	std::vector<VertexSkinned> vertices;
	std::vector<uint32_t> indices;

	// vertex
	{
		for (uint32_t i = 0; i < pMesh->mNumVertices; i++)
		{
			VertexSkinned vertex;

			vertex.position = AssimpHelpers::VectorConvertFloat3(pMesh->mVertices[i]);
			vertex.normal = AssimpHelpers::VectorConvertFloat3(pMesh->mNormals[i]);
			if (pMesh->mTextureCoords[0])
			{
				vertex.texCoords.x = pMesh->mTextureCoords[0][i].x;
				vertex.texCoords.y = pMesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
			}

			vertices.emplace_back(vertex);
		}
	}

	// index
	{
		for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
		{
			aiFace face = pMesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
	}

	extractBoneWeightForVertices(vertices, pMesh, pScene);

	return Mesh(vertices, indices);
}

void Model::extractBoneWeightForVertices(std::vector<VertexSkinned>& vertices, aiMesh* pMesh, const aiScene* pScene)
{
	for (uint32_t i = 0; i < pMesh->mNumBones; ++i)
	{
		int boneID = -1;
		aiBone* pBone = pMesh->mBones[i];
		std::string boneName = pBone->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offsetTransform = AssimpHelpers::ConvertMatrix(pBone->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}

		auto weights = pBone->mWeights;
		int numWeights = pBone->mNumWeights;

		for (int j = 0; j < numWeights; ++j)
		{
			uint32_t vertexId = weights[j].mVertexId;
			float weight = weights[j].mWeight;

			setVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

void Model::setVertexBoneData(VertexSkinned& vertex, int boneID, float weight)
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