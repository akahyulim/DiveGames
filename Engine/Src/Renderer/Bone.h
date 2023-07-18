#pragma once

namespace Dive
{
	struct KeyPosition
	{
		KeyPosition(double time, DirectX::XMFLOAT3 pos)
		{
			timeStamp = time;
			position = pos;
		}

		double timeStamp;
		DirectX::XMFLOAT3 position;
	};

	struct KeyRotation
	{
		KeyRotation(double time, DirectX::XMFLOAT4 rot)
		{
			timeStamp = time;
			rotation = rot;
		}

		double timeStamp;
		DirectX::XMFLOAT4 rotation;
	};

	struct KeyScale
	{
		KeyScale(double time, DirectX::XMFLOAT3 scl)
		{
			timeStamp = time;
			scale = scl;
		}

		double timeStamp;
		DirectX::XMFLOAT3 scale;
	};

	class Bone
	{
	public:
		Bone(const std::string& name, int32_t id);
		~Bone();

		void Update(float delta);

		// 상당히 허접하다.
		void SetPositions(const std::vector<KeyPosition>& positions) { m_Positions = positions; m_NumPositionKeys = static_cast<uint32_t>(m_Positions.size()); }
		void SetRotations(const std::vector<KeyRotation>& rotations) { m_Rotations = rotations; m_NumRotationKeys = static_cast<uint32_t>(m_Rotations.size()); }
		void SetScales(const std::vector<KeyScale>& scales) { m_Scales = scales; m_NumScaleKeys = static_cast<uint32_t>(m_Scales.size()); }

		DirectX::XMFLOAT4X4 GetLocalTransform() const { return m_LocalTransform; }

		std::string GetBoneName() const { return m_Name; }

		int GetPositionIndex(float delta);
		int GetRotationIndex(float delta);
		int GetScaleIndex(float delta);

	private:
		float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float delta);

		DirectX::XMFLOAT3 interpolatePosition(float deltaTime);
		DirectX::XMFLOAT4 interpolateRotation(float deltaTime);
		DirectX::XMFLOAT3 interpolateScale(float deltaTime);

	private:
		std::string m_Name;
		int32_t m_ID;

		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		uint32_t m_NumPositionKeys;
		uint32_t m_NumRotationKeys;
		uint32_t m_NumScaleKeys;

		DirectX::XMFLOAT4X4 m_LocalTransform;
	};
}