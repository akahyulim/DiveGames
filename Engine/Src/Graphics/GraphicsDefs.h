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
		Specular,
		GBuffer_DepthStencil,
		GBuffer_Color_SpecIntensity,
		GBuffer_Normal,
		GBuffer_SpecPower,
		Max_Num
	};

	enum class eVertexType
	{
		Undefined,
		Pos, 
		PosCol,
		PosTex,
		Model,
		Skinned,
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
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;
		}

		float position[3] = { 0.0f, };
		float texCoords[2] = { 0.0f, };
		float normal[3] = { 0.0f, };
		float tangent[3] = { 0.0f, };
	};

	struct VertexSkinned
	{
		VertexSkinned() = default;
		VertexSkinned(
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMUINT4& boneIDs,
			const DirectX::XMFLOAT4& weights)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->boneIDs[0] = boneIDs.x;
			this->boneIDs[1] = boneIDs.y;
			this->boneIDs[2] = boneIDs.z;
			this->boneIDs[3] = boneIDs.w;

			this->weights[0] = weights.x;
			this->weights[1] = weights.y;
			this->weights[2] = weights.z;
			this->weights[3] = weights.w;
		}

		float position[3] = { 0.0f, 0.0f, 0.0f };
		float texCoords[2] = { 0.0f, 0.0f };
		float normal[3] = { 0.0f, 0.0f, 0.0f };
		float tangent[3] = { 0.0f, 0.0f, 0.0f };
		uint32_t boneIDs[4] = { 100, 100, 100, 100 };
		float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	//struct BoneInfo
	//{
	//	int id;
	//	DirectX::XMFLOAT4X4 offsetTransform;
	//};

	inline constexpr uint32_t MAX_RENDERTARGETS = 4;
}