#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace dive
{
	struct VertexType_PosTexNorTan
	{
		VertexType_PosTexNorTan() = default;
		VertexType_PosTexNorTan(
			const Vector3& pos,
			const Vector2& tex,
			const Vector3& nor = Vector3::Zero,
			const Vector3& tan = Vector3::Zero)
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

		// 스파르탄 역시 배열로 관리한다.
		float position[3] = { 0.0f, };
		float texCoord[2] = { 0.0f, };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
	};

}