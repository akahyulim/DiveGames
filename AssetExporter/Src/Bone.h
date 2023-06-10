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

// Bone�� �ִϸ��̼��� �����ϰ� time�� ���� ���������� �� ��ȯ ����� �����Ѵ�.
// �ذ�å���� ġ�ڸ� Track Ŭ������ ������ �����ϴ�.
class Bone
{
public:
	Bone(const std::string& name, int id, const aiNodeAnim* pChannel);
	~Bone();

	// ���޹��� �ð��� ���� �ִϸ��̼��� �� ��ҵ��� ���������� ��
	// �̵��� �̿��� ���� ��ȯ ����� ����
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