#include "Animation.h"
#include "AssimpHelpers.h"

Animation::Animation(const std::string& filePath, Model* pModel)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate);
	DV_ASSERT(pScene && pScene->mRootNode);

	auto pAnimation = pScene->mAnimations[0];
	m_Duration = pAnimation->mDuration;
	m_TickPerSecond = pAnimation->mTicksPerSecond;
	// root node ����� ������� ����ߴµ� ����ϴ� ���� ����.
	readHierarchyData(m_RootNodeData, pScene->mRootNode);
	// ���ڷ� ���� model���� bondmap�� ������ ���� bone�� �ִ��� Ȯ���� �� id�� ä���.
	// �̶� bone�� Ȯ���� animation channel�� node name���� �Ѵ�.
	readMissingBones(pAnimation, pModel);
}

Animation::~Animation()
{
}

Bone* Animation::FindBone(const std::string& name)
{
	auto it = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& bone)
		{
			return bone.GetBoneName() == name;
		}
	);
	
	if (it == m_Bones.end())
		return nullptr;
	else
		return &(*it);
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* pSrc)
{
	dest.name = pSrc->mName.C_Str();
	dest.transformation = AssimpHelpers::ConvertMatrix(pSrc->mTransformation);
	dest.numChildren = pSrc->mNumChildren;

	for (int i = 0; i < pSrc->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, pSrc->mChildren[i]);
		dest.children.emplace_back(newData);
	}
}

void Animation::readMissingBones(const aiAnimation* pAnimation, Model* pModel)
{
	int size = pAnimation->mNumChannels;
	auto& boneInfoMap = pModel->GetBoneInfoMap();
	uint32_t& boneCount = pModel->GetBoneCount();

	for (int i = 0; i < size; ++i)
	{
		auto pChannel = pAnimation->mChannels[i];
		std::string boneName = pChannel->mNodeName.C_Str();

		// �� �κ��� �ᱹ BondInfo�� �������� �ʾҴٴ� ���ε�...
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		// Bone ��ü�� �����Ѵ�.
		// �� �Լ��� ������ �̰� �ƴѰ� ������ �̸��� ��...?
		m_Bones.emplace_back(Bone(boneName, boneInfoMap[boneName].id, pChannel));
	}

	m_BoneInfoMap = boneInfoMap;
}
