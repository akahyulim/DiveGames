#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace Dive
{
	enum class eVertexElementType
	{
		Int = 0,
		Float,
		Float2,
		Float3,
		Float4,
		UByte4,
		MaxCount
	};

	enum class eVertexElementSemantic
	{
		Position = 0,
		Normal,
		Color,
		TexCoord,
		Tangent,
		BlendWeight,
		MaxBlendIndex,
	};

	struct VertexElement
	{
		VertexElement()
			: type(eVertexElementType::Float3),
			semantic(eVertexElementSemantic::Position),
			index(0),
			offset(0)
		{}

		VertexElement(
			eVertexElementType type,
			eVertexElementSemantic semantic,
			unsigned char index = 0
		)
		{
			this->type = type;
			this->semantic = semantic;
			this->index = index;
			this->offset = 0;
		}

		bool operator==(const VertexElement& rhs) const
		{
			return type == rhs.type && semantic == rhs.semantic && index == rhs.index && offset == rhs.offset;
		}

		bool operator!=(const VertexElement& rhs) const
		{
			return !(*this == rhs);
		}

		eVertexElementType type;
		eVertexElementSemantic semantic;
		unsigned char index;
		unsigned int offset;
	};

	enum class eVertexType
	{
		Static,
		Skinned,
	};

	struct VertexType_Static
	{
		VertexType_Static() = default;
		VertexType_Static(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT4& col = {0.0f, 0.0f, 0.0f, 1.0f},
			const DirectX::XMFLOAT3& nor = {0.0f, 0.0f, 0.0f},
			const DirectX::XMFLOAT3& tan = {0.0f, 0.0f, 0.0f}
		)
		{
			this->position[0] = pos.x;
			this->position[1] = pos.y;
			this->position[2] = pos.z;

			this->texCoord[0] = tex.x;
			this->texCoord[1] = tex.y;

			this->color[0] = col.x;
			this->color[1] = col.y;
			this->color[2] = col.z;
			this->color[3] = col.w;

			this->normal[0] = nor.x;
			this->normal[1] = nor.y;
			this->normal[2] = nor.z;

			this->tangent[0] = tan.x;
			this->tangent[1] = tan.y;
			this->tangent[2] = tan.z;
		}

		float position[3] = { 0.0f, };
		float texCoord[2] = { 0.0f, };
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
	};

	struct VertexType_Skinned
	{
		VertexType_Skinned() = default;
		VertexType_Skinned(
			const DirectX::XMFLOAT3 & pos,
			const DirectX::XMFLOAT2 & tex,
			const DirectX::XMFLOAT4 & col = { 0.0f, 0.0f, 0.0f, 1.0f },
			const DirectX::XMFLOAT3 & nor = { 0.0f, 0.0f, 0.0f },
			const DirectX::XMFLOAT3 & tan = { 0.0f, 0.0f, 0.0f },
			const DirectX::XMFLOAT4 & wei = {0.0f, 0.0f, 0.0f, 0.0f},
			const DirectX::XMUINT4 & idx = {0, 0, 0, 0}
		)
		{
			this->position[0] = pos.x;
			this->position[1] = pos.y;
			this->position[2] = pos.z;

			this->texCoord[0] = tex.x;
			this->texCoord[1] = tex.y;

			this->color[0] = col.x;
			this->color[1] = col.y;
			this->color[2] = col.z;
			this->color[3] = col.w;

			this->normal[0] = nor.x;
			this->normal[1] = nor.y;
			this->normal[2] = nor.z;

			this->tangent[0] = tan.x;
			this->tangent[1] = tan.y;
			this->tangent[2] = tan.z;

			this->weight[0] = wei.x;
			this->weight[1] = wei.y;
			this->weight[2] = wei.z;
			this->weight[3] = wei.w;

			this->index[0] = idx.x;
			this->index[1] = idx.y;
			this->index[2] = idx.z;
			this->index[3] = idx.w;
		}

		float position[3] = { 0.0f, };
		float texCoord[2] = { 0.0f, };
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };

		float weight[4] = { 0.0f, };
		unsigned int index[4] = { 0, };
	};
}