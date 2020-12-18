#pragma once
#include "DivePch.h"


namespace Dive
{
	struct Vertex_Undefined {};

	struct Vertex_Pos
	{
		Vertex_Pos() = default;
		Vertex_Pos(const DirectX::XMFLOAT3& position)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;
		}

		float position[3] = { 0.0f, };
	};

	struct Vertex_PosCol
	{
		Vertex_PosCol() = default;
		Vertex_PosCol(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->color[0] = color.x;
			this->color[1] = color.y;
			this->color[2] = color.z;
			this->color[3] = color.w;
		}

		float position[3]	= { 0.0f, };
		float color[4]		= { 0.0f, };
	};

	struct Vertex_PosTex
	{
		Vertex_PosTex() = default;
		Vertex_PosTex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& uv)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->uv[0] = uv.x;
			this->uv[1] = uv.y;
		}

		float position[3]	= { 0.0f, };
		float uv[2]			= { 0.0f, };
	};

	struct Vertex_2dPosTexCol
	{
		Vertex_2dPosTexCol() = default;

		float position[2]	= { 0.0f, };
		float uv[2]			= { 0.0f, };
		unsigned int color	= 0;
	};

	struct Vertex_PosTexNorTan
	{
		Vertex_PosTexNorTan() = default;
		Vertex_PosTexNorTan(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->uv[0] = uv.x;
			this->uv[1] = uv.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;
		}

		float position[3]	= { 0.0f, };
		float uv[2]			= { 0.0f, };
		float normal[3]		= { 0.0f, };
		float tangent[3]	= { 0.0f, };
	};

	struct Vertex_Skinned
	{
		Vertex_Skinned() = default;
		Vertex_Skinned(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent,
			const DirectX::XMFLOAT4& weight, const DirectX::XMFLOAT4& index)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->uv[0] = uv.x;
			this->uv[1] = uv.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->weight[0] = weight.x;
			this->weight[1] = weight.y;
			this->weight[2] = weight.z;
			this->weight[3] = weight.w;

			this->index[0] = static_cast<unsigned int>(index.x);
			this->index[1] = static_cast<unsigned int>(index.y);
			this->index[2] = static_cast<unsigned int>(index.z);
			this->index[3] = static_cast<unsigned int>(index.w);
		}

		float position[3]		= { 0.0f, };
		float uv[2]				= { 0.0f, };
		float normal[3]			= { 0.0f, };
		float tangent[3]		= { 0.0f, };
		float weight[4]			= { 0.0f, };
		unsigned int index[4]	= { 0, };		// bone index
	};

	// 타입들을 enum으로 묶는다. 이는 템플릿 타입으로 전달하기 위해서이다.
	enum class eVertexType
	{
		Unknown,
		Position,
		PositionColor,
		PositionUv,
		PositionUvNormalTangent,
		PositionUvColor2D
	};

	template<typename T>
	constexpr eVertexType VertexTypeToEnum() { return eVertexType::Unknown; }

	template<> inline eVertexType VertexTypeToEnum<Vertex_Pos>()			{ return eVertexType::Position; }
	template<> inline eVertexType VertexTypeToEnum<Vertex_PosCol>()			{ return eVertexType::PositionColor; }
	template<> inline eVertexType VertexTypeToEnum<Vertex_PosTex>()			{ return eVertexType::PositionUv; }
	template<> inline eVertexType VertexTypeToEnum<Vertex_PosTexNorTan>()	{ return eVertexType::PositionUvNormalTangent; }
	template<> inline eVertexType VertexTypeToEnum<Vertex_2dPosTexCol>()	{ return eVertexType::PositionUvColor2D; }
}