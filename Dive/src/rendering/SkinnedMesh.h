#pragma once
#include "StaticMesh.h"

namespace Dive
{
	class SkinnedMesh : public StaticMesh
	{
	public:
		SkinnedMesh();
		~SkinnedMesh() override = default;

		bool CreateBuffers() override;
		void Clear() override;

		void SetBoneWeights(const std::vector<DirectX::XMFLOAT4>& boneWeights) { m_BoneWeights = boneWeights; }
		void SetBoneIndices(const std::vector<DirectX::XMFLOAT4>& boneIndices) { m_BoneIndices = boneIndices; }

		static constexpr eResourceType GetType() { return eResourceType::SkinnedMesh; }

	private:
		std::vector<DirectX::XMFLOAT4> m_BoneWeights;
		std::vector<DirectX::XMFLOAT4> m_BoneIndices;
	};
}