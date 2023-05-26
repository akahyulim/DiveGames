#pragma once

namespace Dive
{
	enum eClearTarget : uint8_t
	{
		Color = 0x1,
		Depth = 0x2,
		Stencil = 0x4
	};

	enum class eShaderType
	{
		UnknownShader,
		VertexShader,
		PixelShader,
		ComputeShader
	};
	
	enum class eTextureUsage
	{
		Static,
		Dynamic,
		RenderTarget,
		DepthStencil
	};

	enum class eTextureUnit : uint32_t
	{
		Diffuse = 0,
		Normal,
		DepthTex,
		ColorSpecIntTex,
		NormalTex,
		SpecPowTex,
		Max_Num
	};

	enum class eVertexType
	{
		Undefined,
		Pos, 
		PosCol,
		PosTex,
		Model,
	};

	enum class eConstantBufferIndex
	{
		Frame = 0,
		Camera,
		Light,
		Material
	};

	struct VertexStatic
	{
		VertexStatic() = default;
		VertexStatic(
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

	struct VertexDynamic
	{
		VertexDynamic() = default;
		VertexDynamic(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT2& tex,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMFLOAT4& boneIdx,
			const DirectX::XMFLOAT4& weights)
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

			this->boneIndexes[0] = boneIdx.x;
			this->boneIndexes[1] = boneIdx.y;
			this->boneIndexes[2] = boneIdx.z;
			this->boneIndexes[3] = boneIdx.w;

			this->weights[0] = weights.x;
			this->weights[1] = weights.y;
			this->weights[2] = weights.z;
			this->weights[3] = weights.w;
		}

		float pos[3] = { 0.0f, };
		float tex[2] = { 0.0f, };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
		float boneIndexes[4] = { 0.0f, };
		float weights[4] = { 0.0f, };
	};

	inline constexpr uint32_t MAX_RENDERTARGETS = 4;
}