#pragma once
#include <assimp/scene.h>
#include "Dive.h"

struct KeyPosition
{
	DirectX::XMFLOAT3 position;
	double timeStamp;
};

struct KeyRotation
{
	DirectX::XMFLOAT4 orientation;
	double timeStamp;
};

struct KeyScale
{
	DirectX::XMFLOAT3 scale;
	double timeStamp;
};

// Bone의 애니메이션을 저장하고 time에 맞춰 선형보간한 후 변환 행렬을 리턴한다.
// 해골책으로 치자면 Track 클래스와 성격이 유사하다.
class Bone
{
public:
	Bone(const std::string& name, int id, const aiNodeAnim* pChannel);
	~Bone();

	// 전달받은 시간에 맞춰 애니메이션의 각 요소들을 선형보간한 후
	// 이들을 이용해 최종 변환 행렬을 구성
	void Update(float delta);

	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() const { return m_ID; }
	DirectX::XMFLOAT4X4 GetLocalTransform() const { return m_LocalTransform; }

private:
private:
	std::string m_Name;
	int m_ID;

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;

	uint32_t m_NumPositionKeys;
	uint32_t m_NumRotationKeys;
	uint32_t m_NumScaleKeys;

	DirectX::XMFLOAT4X4 m_LocalTransform;
};