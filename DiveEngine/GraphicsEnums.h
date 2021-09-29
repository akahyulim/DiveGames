#pragma once

namespace dive
{
	// ���� ���� ��
	enum DEPTH_STENCIL_STATE_TYPES
	{
		DEPTH_STENCIL_STATE_DEFAULT,
		DEPTH_STENCIL_STATE_TYPE_COUNT
	};

	enum RASTERIZER_STATE_TYPES
	{
		RASTERIZER_STATE_CULLBACK_SOLID,
		RASTERIZER_STATE_TYPE_COUNT
	};

	enum SHADER_TYPES
	{
		// Vertex Shaders
		VERTEX_SHADER_GBUFFER,

		// Pixel Shaders
		PIXEL_SHADER_GBUFFER,

		SHADER_TYPE_COUNT
	};

	enum INPUT_LAYOUT_TYPES
	{
		INPUT_LYAOUT_POS,

		INPUT_LAYOUT_TYPE_COUNT,
	};

	// ���� ���� �� END

	// �迭 �ε����� Ȱ���ϱ� ���� enum���� �������.
	enum SHADERTYPES
	{
		// vertex
		VSTYPE_FONTS,
		VSTYPE_LEGACY,

		// pixel
		PSTYPE_FONTS,
		PSTYPE_LEGACY,

		SHADERTYPE_COUNT
	};

	// shader �̸��� �޶� �ǳ�...
	enum ILTYPES
	{
		ILTYPE_POS_TEX2,	// �̰� ��Ʈ������ VertexType�� �ϳ��� ���������.
		ILTYPE_POS_TEX_NOR_TAN,

		ILTYPE_COUNT
	};

	enum class eBufferType
	{
		Vertex,
		Index,
		Constant,
		Invalid
	};

	enum class eShaderStage
	{
		Vs,
		Ps,
		Invalid
	};

	enum class eDepthStencilState
	{
		Default,
		Count,
	};

	enum class eRasterizerState
	{
		Cullback_Solid,
		Count
	};

	// �ӽ�... ���� �ٽ� ������� �Ѵ�.
	enum class eSamplerState
	{
		Default,
		Linear,
		Count
	};
}