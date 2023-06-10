#include "Animator.h"

Animator::Animator(Animation* pAnimation)
	: m_pCurrentAnimation(pAnimation),
	m_CurrentTime(0.0f),
	m_DeltaTime(0.0f)
{
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
	// 왜 100개로 고정되어 있을까?
	m_FinalBoneMatrices.reserve(100);
	for (int i = 0; i < 100; ++i)
		m_FinalBoneMatrices.emplace_back(matrix);
}

void Animator::UpdateAnimation(float delta)
{
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());

	m_DeltaTime = delta;
	if (m_pCurrentAnimation)
	{
		m_CurrentTime += m_pCurrentAnimation->GetTickPerSecond() * delta;
		m_CurrentTime = fmod(m_CurrentTime, m_pCurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_pCurrentAnimation->GetRootNodeData(), matrix);
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_pCurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* pNode, DirectX::XMFLOAT4X4 parentTransform)
{
	auto nodeName = pNode->name;
	auto nodeTransform = pNode->transformation;

	Bone* pBone = m_pCurrentAnimation->FindBone(nodeName);

	if (pBone) 
	{
		pBone->Update(m_CurrentTime);
		nodeTransform = pBone->GetLocalTransform();
	}

	DirectX::XMFLOAT4X4 globalTransformation;
	DirectX::XMStoreFloat4x4(&globalTransformation, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&globalTransformation, 
		DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&parentTransform), DirectX::XMLoadFloat4x4(&nodeTransform)));

	auto boneInfoMap = m_pCurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		auto offset = boneInfoMap[nodeName].offsetTransform;
		DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[index], 
			DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&globalTransformation), DirectX::XMLoadFloat4x4(&offset)));
	}

	for (uint32_t i = 0; i < pNode->numChildren; ++i)
		CalculateBoneTransform(&pNode->children[i], globalTransformation);
}
