#pragma once

namespace Dive
{
	// bone�� transform�� ��︰��.
	// �̰� BoneInfo ������ ����.
	struct BoneInfo
	{
		std::string name;
		uint32_t index;
		DirectX::XMFLOAT4X4 offsetMatrix;
	};

	class Skeleton
	{
	public:
		Skeleton() {}
		~Skeleton() {}

		const std::vector<BoneInfo>& GetBones() const { return m_Bones; }
		std::vector<BoneInfo>& GetModifiableBones() { return m_Bones; }

		uint32_t GetNumBones() { return static_cast<uint32_t>(m_Bones.size()); }

	private:
	private:
		std::vector<BoneInfo> m_Bones;
	};
}