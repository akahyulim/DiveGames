#pragma once
#include <DirectXMath.h>

namespace Dive
{
	struct VertexModel
	{
		VertexModel() = default;
		VertexModel(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent)
		{
			this->pos[0] = pos.x;
			this->pos[1] = pos.y;
			this->pos[2] = pos.z;

			this->tex[0] = tex.x;
			this->tex[1] = tex.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;
		}

		float pos[3] = { 0.0f, };
		float tex[2] = { 0.0f, };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
	};
}