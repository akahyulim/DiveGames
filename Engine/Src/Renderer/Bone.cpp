#include "DivePch.h"
#include "Bone.h"
#include "io/Log.h"

namespace Dive
{
	Bone::Bone(const std::string& name, int32_t id)
		: m_Name(name),
		m_ID(id),
		m_NumPositionKeys(0),
		m_NumRotationKeys(0),
		m_NumScaleKeys(0)
	{
		DirectX::XMStoreFloat4x4(&m_LocalTransform, DirectX::XMMatrixIdentity());
	}

	Bone::~Bone()
	{
	}

	void Bone::Update(float delta)
	{
		DirectX::XMFLOAT3 pos = interpolatePosition(delta);
		DirectX::XMFLOAT4 rot = interpolateRotation(delta);
		DirectX::XMFLOAT3 scl = interpolateScale(delta);

		DirectX::XMMATRIX localTransform = 
			DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scl)) *
			DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rot)) *
			DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));

		DirectX::XMStoreFloat4x4(&m_LocalTransform, localTransform);
	}

	int Bone::GetPositionIndex(float delta)
	{
		for (int index = 0; index < m_NumPositionKeys - 1; ++index)
		{
			if (delta < m_Positions[index + 1].timeStamp)
				return index;
		}

		return -1;
	}

	int Bone::GetRotationIndex(float delta)
	{
		for (int index = 0; index < m_NumRotationKeys - 1; ++index)
		{
			if (delta < m_Rotations[index + 1].timeStamp)
				return index;
		}

		return -1;
	}

	int Bone::GetScaleIndex(float delta)
	{
		for (int index = 0; index < m_NumScaleKeys - 1; ++index)
		{
			if (delta < m_Scales[index + 1].timeStamp)
				return index;
		}

		return -1;
	}

	float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float delta)
	{
		return (delta - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
	}

	DirectX::XMFLOAT3 Bone::interpolatePosition(float deltaTime)
	{
		if (1 == m_NumPositionKeys)
		{
			return m_Positions[0].position;
		}

		int preIndex = GetPositionIndex(deltaTime);
		int nextIndex = preIndex + 1;
		float scaleFactor = getScaleFactor(m_Positions[preIndex].timeStamp, m_Positions[nextIndex].timeStamp, deltaTime);

		DirectX::XMFLOAT3 finalPosition;
		DirectX::XMStoreFloat3(&finalPosition,
			DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&m_Positions[preIndex].position),
				DirectX::XMLoadFloat3(&m_Positions[nextIndex].position),
				scaleFactor)
		);

		return finalPosition;
	}
	
	DirectX::XMFLOAT4 Bone::interpolateRotation(float deltaTime)
	{
		DirectX::XMFLOAT4 finalRotation;
		DirectX::XMVECTOR quaternion;

		if (1 == m_NumRotationKeys)
		{
			quaternion = DirectX::XMQuaternionNormalize(
				DirectX::XMLoadFloat4(&m_Rotations[0].rotation)
			);
			
			DirectX::XMStoreFloat4(&finalRotation, quaternion);

			return finalRotation;
		}

		int preIndex = GetRotationIndex(deltaTime);
		int nextIndex = preIndex + 1;
		float scaleFactor = getScaleFactor(m_Rotations[preIndex].timeStamp, m_Rotations[nextIndex].timeStamp, deltaTime);

		quaternion = DirectX::XMQuaternionNormalize(
			DirectX::XMQuaternionSlerp(
				DirectX::XMLoadFloat4(&m_Rotations[preIndex].rotation),
				DirectX::XMLoadFloat4(&m_Rotations[nextIndex].rotation),
				scaleFactor)
		);

		DirectX::XMStoreFloat4(&finalRotation, quaternion);

		return finalRotation;
	}
	
	DirectX::XMFLOAT3 Bone::interpolateScale(float deltaTime)
	{
		if (1 == m_NumScaleKeys)
		{
			return m_Scales[0].scale;
		}

		int preIndex = GetScaleIndex(deltaTime);
		int nextIndex = preIndex + 1;
		float scaleFactor = getScaleFactor(m_Scales[preIndex].timeStamp, m_Scales[nextIndex].timeStamp, deltaTime);

		DirectX::XMFLOAT3 finalScale;
		DirectX::XMStoreFloat3(&finalScale,
			DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&m_Scales[preIndex].scale),
				DirectX::XMLoadFloat3(&m_Scales[nextIndex].scale),
				scaleFactor)
		);

		return finalScale;
	}
}