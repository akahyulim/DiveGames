#include "DivePch.h"
#include "Animation.h"
#include "Bone.h"
#include "IO/Log.h"
#include "Animator.h"
#include "Core/EventHandler.h"
#include "Core/CoreEvents.h"
#include "Core/Timer.h"

#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"

namespace Dive
{
	Animator::Animator()
		: m_CurrentTime(0.0f),
		m_DeltaTime(0.0f),
		m_pCurrentAnimation(nullptr)
	{
		DirectX::XMFLOAT4X4 matIdentity;
		DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());

		m_FinalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; ++i)
			m_FinalBoneMatrices.emplace_back(matIdentity);

		SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	}

	Animator::Animator(Animation* pAnimation)
		: m_CurrentTime(0.0f),
		m_DeltaTime(0.0f),
		m_pCurrentAnimation(pAnimation)
	{
		DirectX::XMFLOAT4X4 matIdentity;
		DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());

		m_FinalBoneMatrices.reserve(100);
		for (int i = 0; i < 100; ++i)
			m_FinalBoneMatrices.emplace_back(matIdentity);

		SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	}

	void Animator::OnUpdate(const Event& evnt)
	{
		if (m_pCurrentAnimation)
		{
			m_CurrentTime += m_pCurrentAnimation->GetTickPerSecond() * Timer::GetDeltaTimeSec();
			m_CurrentTime = fmod(m_CurrentTime, m_pCurrentAnimation->GetDuration());

			calcuBoneTransform(m_pCurrentAnimation->GetRootGameObject(), DirectX::XMMatrixIdentity());
		}
	}

	void Animator::SetAnimation(Animation* pAnimation)
	{
		m_pCurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	// 그러니깐 계층구조에서 뼈대들만 누적변환하여 저장하는 것인데,
	// 1. 뼈대가 아닌 노드들을 누적 변환
	// 2. 뼈대일 경우 업데이트를 통해 현재 프레임의 변환을 사용
	// 3. 마지막으로 뼈대일 경우 offset을 이용해 뼈대  좌표계로 변환
	// 이러한 과정을 모두 끝마친 뼈대들을 index 순서대로 등록하면
	// gpu에서 각 정점이 자신에게 영향을 주는 뼈대의 index와 weight를 적용하여 최종 변환한다.
	void Animator::calcuBoneTransform(GameObject* pNode, DirectX::XMMATRIX parent)
	{
		DirectX::XMMATRIX nodeTransform = pNode->GetTransform()->GetLocalMatrix();
		
		// 뼈대노드일 경우 애니메이션 변환을 적용
		std::string nodeName = pNode->GetName();
		Bone* pBone = m_pCurrentAnimation->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_CurrentTime);

			DirectX::XMFLOAT4X4 local = pBone->GetLocalTransform();
			nodeTransform = DirectX::XMLoadFloat4x4(&local);
		}

		DirectX::XMMATRIX globalTransform = parent * nodeTransform;

		// 뼈대의 offset을 곱해 뼈대 좌표계로 변환
		auto boneInfoMap = m_pCurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			uint32_t index = boneInfoMap[nodeName].id;
			DirectX::XMFLOAT4X4 offset = boneInfoMap[nodeName].offsetTransform;

			DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[index],
				DirectX::XMMatrixMultiply(globalTransform, DirectX::XMLoadFloat4x4(&offset)));
		}

		for (Transform* pChild : pNode->GetTransform()->GetChildren())
		{
			calcuBoneTransform(pChild->GetGameObject(), globalTransform);
		}
	}
}