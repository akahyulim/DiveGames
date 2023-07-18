#pragma once
#include <assimp/scene.h>
#include "Dive.h"
#include "Mesh.h"

struct BoneInfo
{
	int id;
	DirectX::XMFLOAT4X4 offsetTransform;
};

// 파일을 파싱하여 Meshes, Textures, BoneInfoMap을 구성
// BoneInfoMap의 경우 Animation에서 최종 완성된다.
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