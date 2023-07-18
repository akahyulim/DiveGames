#pragma once

namespace Dive
{
	class Animation;
	class Bone;
	struct ModelNodeData;

	class GameObject;
	class Transform;

	class Event;

	class Animator
	{
	public:
		Animator();
		Animator(Animation* pAnimation);
		~Animator() = default;

		void OnUpdate(const Event& evnt);

		void SetAnimation(Animation* pAnimation);

		std::vector<DirectX::XMFLOAT4X4> GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

	private:
		void calcuBoneTransform(GameObject* pNode, DirectX::XMMATRIX parent);

	private:
		float m_CurrentTime;
		float m_DeltaTime;
		Animation* m_pCurrentAnimation;
		std::vector<DirectX::XMFLOAT4X4> m_FinalBoneMatrices;
	};
}