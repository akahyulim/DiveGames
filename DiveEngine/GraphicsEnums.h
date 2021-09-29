#pragma once

namespace dive
{
	// 새로 만든 것
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

	// 새로 만든 것 END

	// 배열 인덱스로 활용하기 위해 enum으로 만들었다.
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

	// shader 이름과 달라도 되나...
	enum ILTYPES
	{
		ILTYPE_POS_TEX2,	// 이건 폰트용으로 VertexType이 하나로 만들어졌다.
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

	// 임시... 추후 다시 나누어야 한다.
	enum class eSamplerState
	{
		Default,
		Linear,
		Count
	};
}