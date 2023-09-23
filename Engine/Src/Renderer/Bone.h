#pragma once

namespace Dive
{
	struct KeyPosition
	{
		KeyPosition(float time, const DirectX::XMFLOAT3& value)
		{
			timeStamp = time;
			position = value;
		}

		float timeStamp = 0.0f;
		DirectX::XMFLOAT3 position;
	};

	struct KeyRotation
	{
		KeyRotation(float time, const DirectX::XMFLOAT4& value)
		{
			timeStamp = time;
			rotation = value;
		}

		float timeStamp = 0.0f;
		DirectX::XMFLOAT4 rotation;
	};

	struct KeyScale
	{
		KeyScale(float time, const DirectX::XMFLOAT3& value)
		{
			timeStamp = time;
			scale = value;
		}

		float timeStamp = 0.0f;
		DirectX::XMFLOAT3 scale;
	};

	class Bone
	{
	public:
		// 참고문서에서는 Animation의 생성자 과정에서 생성된다.
		Bone(const std::string& name);
		~Bone() = default;

		void Update(float delta);

		void InsertPositionKey(float time, const DirectX::XMFLOAT3& value) { m_Positions.emplace_back(time, value); }
		void InsertRotationKey(float time, const DirectX::XMFLOAT4& value) { m_Rotations.emplace_back(time, value); }
		void InsertScaleKey(float time, const DirectX::XMFLOAT3& value) { m_Scales.emplace_back(time, value); }

		uint32_t GetPositionIndex(float time);
		uint32_t GetRotationIndex(float time);
		uint32_t GetScaleIndex(float time);

		uint32_t GetNumPositionKeys() const { return static_cast<uint32_t>(m_Positions.size()); }
		uint32_t GetNumRotationKeys() const { return static_cast<uint32_t>(m_Rotations.size()); }
		uint32_t GetNumScaleKeys() const { return static_cast<uint32_t>(m_Scales.size()); }

		std::string GetBoneName() const { return m_Name; }
		DirectX::XMMATRIX GetLocalTransform() const { return DirectX::XMLoadFloat4x4(&m_LocalTransform); }

	private:
		float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float time);
		DirectX::XMMATRIX interpolatePosition(float time);
		DirectX::XMMATRIX interpolateRotation(float time);
		DirectX::XMMATRIX interpolateScale(float time);

	private:
		std::string m_Name;

		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		DirectX::XMFLOAT4X4 m_LocalTransform;
	};
}