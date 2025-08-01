#pragma once
#include <DirectXMath.h>
#include <algorithm>

namespace Dive
{
	struct PNVertex
	{
		PNVertex() = default;
		PNVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& nor)
			: Position(pos)
			, Normal(nor)
		{
		}

		DirectX::XMFLOAT3 Position{0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 Normal{ 0.0f, 0.0f, 0.0f };
	};

	struct PNTVertex : PNVertex
	{
		PNTVertex() = default;
		PNTVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& nor, const DirectX::XMFLOAT3& tan)
			: PNVertex(pos, nor)
			, Tangent(tan)
		{
		}

		DirectX::XMFLOAT3 Tangent{0.0f, 0.0f, 0.0f};
	};

	struct LitVertex : PNTVertex
	{
		LitVertex() = default;
		LitVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& nor, const DirectX::XMFLOAT3& tan, const DirectX::XMFLOAT2& tex)
			: PNTVertex(pos, nor, tan)
			, TexCoord(tex)
		{
		}

		DirectX::XMFLOAT2 TexCoord{0.0f, 0.0f};
	};

	struct SkinnedVertex : LitVertex
	{
		SkinnedVertex() = default;
		SkinnedVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& nor, const DirectX::XMFLOAT3& tan, const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT4& wgt, const uint32_t bi[4])
			: LitVertex(pos, nor, tan, tex)
			, Weights(wgt)
		{
			std::copy(bi, bi + 4, Indices);
		}

		DirectX::XMFLOAT4 Weights{0.0f, 0.0f, 0.0f, 0.0f};
		uint32_t Indices[4]{0, 0, 0, 0};
	};
}