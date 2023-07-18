#pragma once
#include <assimp/scene.h>
#include "Dive.h"
#include "Mesh.h"

struct BoneInfo
{
	int id;
	DirectX::XMFLOAT4X4 offsetTransform;
};

// ������ �Ľ��Ͽ� Meshes, Textures, BoneInfoMap�� ����
// BoneInfoMap�� ��� Animation���� ���� �ϼ��ȴ�.
class Model
{
public:
	Model(const std::string& filePath);
	~Model();

	void Draw();

	std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
	uint32_t& GetBoneCount() { return m_BoneCounter; }

private:
	void loadModel(const std::string& filePath);
	void processNode(aiNode* pNode, const aiScene* pScene);
	Mesh processMesh(aiMesh* pMesh, const aiScene* pScene);
	void extractBoneWeightForVertices(std::vector<VertexSkinned>& vertices, aiMesh* pMesh, const aiScene* pScene);
	void setVertexBoneData(VertexSkinned& vertex, int boneID, float weight);

private:
	std::vector<Mesh> m_Meshes;
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
	uint32_t m_BoneCounter;

};