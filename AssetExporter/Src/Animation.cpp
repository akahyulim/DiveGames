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
	// root node 행렬의 역행렬을 계산했는데 사용하는 곳이 없다.
	readHierarchyData(m_RootNodeData, pScene->mRootNode);
	// 인자로 받은 model에서 bondmap을 가져와 빠진 bone이 있는지 확인한 후 id를 채운다.
	// 이때 bone의 확인은 animation channel의 node name으로 한다.
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

		// 이 부분은 결국 BondInfo를 구성하지 않았다는 것인데...
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		// Bone 객체를 생성한다.
		// 이 함수의 메인은 이게 아닌가 싶은데 이름이 왜...?
		m_Bones.emplace_back(Bone(boneName, boneInfoMap[boneName].id, pChannel));
	}

	m_BoneInfoMap = boneInfoMap;
}
