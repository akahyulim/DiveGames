#pragma once
#include <assimp/scene.h>
#include "Dive.h"
#include "Animation.h"
#include "Bone.h"

// animation 실행 후 변환 결과를 관리
class Animator
{
public:
	Animator(Animation* pAnimation);
	~Animator() = default;

	void UpdateAnimation(float delta);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const AssimpNodeData* pNode, DirectX::XMFLOAT4X4 parentTransform);
	std::vector<DirectX::XMFLOAT4X4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

private:
	std::vector<DirectX::XMFLOAT4X4> m_FinalBoneMatrices;
	Animation* m_pCurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};
