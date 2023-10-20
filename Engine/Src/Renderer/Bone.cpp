#include "DivePch.h"
#include "Bone.h"
#include "Core/Log.h"

namespace Dive
{
	Bone::Bone(const std::string& name)
		: m_Name(name)
	{
		DirectX::XMStoreFloat4x4(&m_LocalTransform, DirectX::XMMatrixIdentity());
	}

	void Bone::Update(float delta)
	{
		DirectX::XMStoreFloat4x4(&m_LocalTransform,
			interpolateScale(delta) * interpolateRotation(delta) * interpolatePosition(delta));
	}

	uint32_t Bone::GetPositionIndex(float time)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(m_Positions.size()); ++index)
		{
			if (time < m_Positions[(size_t)index + 1].timeStamp)
				return index;
		}

		return 0;
	}
	
	uint32_t Bone::GetRotationIndex(float time)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(m_Rotations.size()); ++index)
		{
			if (time < m_Rotations[(size_t)index + 1].timeStamp)
				return index;
		}

		return 0;
	}
	
	uint32_t Bone::GetScaleIndex(float time)
	{
		for (uint32_t index = 0; index < static_cast<uint32_t>(m_Scales.size()); ++index)
		{
			if (time < m_Scales[(size_t)index + 1].timeStamp)
				return index;
		}

		return 0;
	}

	float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float time)
	{
		float scaleFactor = 0.0f;
		float midWayLength = time - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;

		return scaleFactor;
	}

	DirectX::XMMATRIX Bone::interpolatePosition(float time)
	{
		if (1 == m_Positions.size())
			return DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_Positions[0].position));

		uint32_t lastIndex = GetPositionIndex(time);
		uint32_t nextIndex = lastIndex + 1;
		float scaleFactor = getScaleFactor(
			m_Positions[lastIndex].timeStamp,
			m_Positions[nextIndex].timeStamp,
			time);
		auto finalPos = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&m_Positions[lastIndex].position),
			DirectX::XMLoadFloat3(&m_Positions[nextIndex].position),
			scaleFactor);

		return DirectX::XMMatrixTranslationFromVector(finalPos);
	}
	
	DirectX::XMMATRIX Bone::interpolateRotation(float time)
	{
		if (1 == m_Rotations.size())
		{
			auto rotation = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&m_Rotations[0].rotation));
			return DirectX::XMMatrixRotationQuaternion(rotation);
		}
		uint32_t lastIndex = GetRotationIndex(time);
		uint32_t nextIndex = lastIndex + 1;
		float scaleFactor = getScaleFactor(
			m_Rotations[lastIndex].timeStamp, 
			m_Rotations[nextIndex].timeStamp,
			time);
		auto finalRot = DirectX::XMQuaternionSlerp(
			DirectX::XMLoadFloat4(&m_Rotations[lastIndex].rotation),
			DirectX::XMLoadFloat4(&m_Rotations[nextIndex].rotation),
			scaleFactor);
		finalRot = DirectX::XMQuaternionNormalize(finalRot);

		return DirectX::XMMatrixRotationQuaternion(finalRot);
	}
	
	DirectX::XMMATRIX Bone::interpolateScale(float time)
	{
		if (1 == m_Scales.size())
			return DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_Scales[0].scale));

		uint32_t lastIndex = GetScaleIndex(time);
		uint32_t nextIndex = lastIndex + 1;
		float scaleFactor = getScaleFactor(
			m_Scales[lastIndex].timeStamp, 
			m_Scales[nextIndex].timeStamp,
			time);
		auto finalScl = DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&m_Scales[lastIndex].scale),
			DirectX::XMLoadFloat3(&m_Scales[nextIndex].scale),
			scaleFactor);

		return DirectX::XMMatrixScalingFromVector(finalScl);
	}
}
