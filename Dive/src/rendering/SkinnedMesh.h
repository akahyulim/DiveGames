#pragma once
#include "StaticMesh.h"

namespace Dive
{
	struct SkinnedVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoords;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 weights;
		DirectX::XMUINT4 boneIndices;
	
		SkinnedVertex() = default;
		SkinnedVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT3& tan, const DirectX::XMFLOAT4& col,
			const DirectX::XMFLOAT4& wts, const DirectX::XMUINT4& bIndices)
			: position(pos), normal(norm), texCoords(tex), tangent(tan), color(col), weights(wts), boneIndices(bIndices) 
		{}
	};

	class SkinnedMesh : public StaticMesh
	{
	public:
		SkinnedMesh();
		~SkinnedMesh() override = default;

		bool CreateBuffers() override;
		void Clear() override;

		static constexpr eResourceType GetType() { return eResourceType::SkinnedMesh; }

	private:
		std::vector<SkinnedVertex> m_Vertices;
	};
}
