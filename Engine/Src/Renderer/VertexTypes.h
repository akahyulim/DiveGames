#pragma once
#include "Graphics/GraphicsInclude.h"

namespace Dive
{
	struct Vertex_StaticMesh
	{
		Vertex_StaticMesh(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& uv,
			const DirectX::XMFLOAT3& nor,
			const DirectX::XMFLOAT3& tan
		)
		{
			this->position = pos;
			this->uv = uv;
			this->normal = nor;
			this->tangent = tan;
		}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
	};

	struct VertexType_PosTexNorTan
	{
		VertexType_PosTexNorTan() = default;
		VertexType_PosTexNorTan(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT3& nor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			const DirectX::XMFLOAT3& tan = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))
		{
			this->position[0] = pos.x;
			this->position[1] = pos.y;
			this->position[2] = pos.z;

			this->texCoord[0] = tex.x;
			this->texCoord[1] = tex.y;

			this->normal[0] = nor.x;
			this->normal[1] = nor.y;
			this->normal[2] = nor.z;

			this->tangent[0] = tan.x;
			this->tangent[1] = tan.y;
			this->tangent[2] = tan.z;
		}

		// ���ĸ�ź ���� �迭�� �����Ѵ�.
		float position[3] = { 0.0f, };
		float texCoord[2] = { 0.0f, };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
	};

}