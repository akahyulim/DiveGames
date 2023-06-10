#pragma once
#include <assimp/scene.h>
#include "Dive.h"
#include "Model.h"
#include "Bone.h"

struct AssimpNodeData
{
	std::string name;
	DirectX::XMFLOAT4X4 transformation;
	uint32_t numChildren;
	std::vector<AssimpNodeData> children;
};

// 0. �⺻ �ִϸ��̼� ������ �����Ѵ�.
// 1. ����� ���������� �����Ѵ�.
// 2. Model���� ������ BoneInfoMap�� �ϼ��Ѵ�.
// 3. Bone�� ���� �� �����Ѵ�.
class Animation
{
public:
	Animation(const std::string& filePath, Model* pModel);
	~Animation();

	Bone* FindBone(const std::string& name);

	float GetDuration() const { return m_Duration; }
	float GetTickPerSecond() const { return m_TickPerSecond; }
	const AssimpNodeData& GetRootNodeData() const { return m_RootNodeData; }
	const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }


	// �׽�Ʈ��
	auto GetBones() const { return m_Bones; }

private:
	void readHierarchyData(AssimpNodeData& dest, const aiNode* pSrc);
	void readMissingBones(const aiAnimation* pAnimation, Model* pModel);

private:
	float m_Duration;
	float m_TickPerSecond;

	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNodeData;

	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
};