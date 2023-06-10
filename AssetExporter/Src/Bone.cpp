#include "Bone.h"
#include "AssimpHelpers.h"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* pChannel)
	: m_Name(name),
	m_ID(id)
{
	DirectX::XMStoreFloat4x4(&m_LocalTransform, DirectX::XMMatrixIdentity());

	m_NumPositionKeys = pChannel->mNumPositionKeys;
	for (uint32_t i = 0; i < m_NumPositionKeys; ++i)
	{
		KeyPosition data;
		data.position = AssimpHelpers::VectorConvertFloat3(pChannel->mPositionKeys[i].mValue);
		data.timeStamp = pChannel->mPositionKeys[i].mTime;

		m_Positions.emplace_back(data);
	}

	m_NumRotationKeys = pChannel->mNumRotationKeys;
	for (uint32_t i = 0; i < m_NumRotationKeys; ++i)
	{
		KeyRotation data;
		data.orientation = AssimpHelpers::QuaternionConvertFloat4(pChannel->mRotationKeys[i].mValue);
		data.timeStamp = pChannel->mRotationKeys[i].mTime;

		m_Rotations.emplace_back(data);
	}

	m_NumScaleKeys = pChannel->mNumScalingKeys;
	for (uint32_t i = 0; i < m_NumScaleKeys; ++i)
	{
		KeyScale data;
		data.scale = AssimpHelpers::VectorConvertFloat3(pChannel->mScalingKeys[i].mValue);
		data.timeStamp = pChannel->mScalingKeys[i].mTime;

		m_Scales.emplace_back(data);
	}
}

Bone::~Bone()
{
}

void Bone::Update(float delta)
{

}
