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

// root node부터 재귀적으로 수행
void Animator::CalculateBoneTransform(const AssimpNodeData* pNode, DirectX::XMFLOAT4X4 parentTransform)
{
	auto nodeName = pNode->name;
	auto nodeTransform = pNode->transformation;

	Bone* pBone = m_pCurrentAnimation->FindBone(nodeName);

	// 뼈대가 존재할 경우 node가 아닌 Bone의 Animation LocalTransform을 사용
	if (pBone) 
	{
		pBone->Update(m_CurrentTime);
		nodeTransform = pBone->GetLocalTransform();
	}

	// 누적 = 현재 노드 * 부모 노드
	DirectX::XMFLOAT4X4 globalTransformation;
	//DirectX::XMStoreFloat4x4(&globalTransformation, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&globalTransformation, 
		DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&parentTransform), DirectX::XMLoadFloat4x4(&nodeTransform)));

	// 다시 offsetTransform을 곱한다.
	// boneInfoMap을 사용하는 이유는 pBone를 통해 ID는 접근할 순 있지만
	// offsetTransform은 BoneInfo에만 저장되어 있기 때문인듯 하다.
	auto boneInfoMap = m_pCurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		auto offset = boneInfoMap[nodeName].offsetTransform;
		DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[index], 
			DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&globalTransformation), DirectX::XMLoadFloat4x4(&offset)));
	}

	// 재귀호출을 통해 모든 노드를 누적 변환
	for (uint32_t i = 0; i < pNode->numChildren; ++i)
		CalculateBoneTransform(&pNode->children[i], globalTransformation);
}
