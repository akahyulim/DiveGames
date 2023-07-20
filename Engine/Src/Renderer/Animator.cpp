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

	// �׷��ϱ� ������������ ����鸸 ������ȯ�Ͽ� �����ϴ� ���ε�,
	// 1. ���밡 �ƴ� ������ ���� ��ȯ
	// 2. ������ ��� ������Ʈ�� ���� ���� �������� ��ȯ�� ���
	// 3. ���������� ������ ��� offset�� �̿��� ����  ��ǥ��� ��ȯ
	// �̷��� ������ ��� ����ģ ������� index ������� ����ϸ�
	// gpu���� �� ������ �ڽſ��� ������ �ִ� ������ index�� weight�� �����Ͽ� ���� ��ȯ�Ѵ�.
	void Animator::calcuBoneTransform(GameObject* pNode, DirectX::XMMATRIX parent)
	{
		DirectX::XMMATRIX nodeTransform = pNode->GetTransform()->GetLocalMatrix();
		
		// �������� ��� �ִϸ��̼� ��ȯ�� ����
		std::string nodeName = pNode->GetName();
		Bone* pBone = m_pCurrentAnimation->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_CurrentTime);

			DirectX::XMFLOAT4X4 local = pBone->GetLocalTransform();
			nodeTransform = DirectX::XMLoadFloat4x4(&local);
		}

		DirectX::XMMATRIX globalTransform = parent * nodeTransform;

		// ������ offset�� ���� ���� ��ǥ��� ��ȯ
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