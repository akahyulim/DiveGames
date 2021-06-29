#pragma once

namespace dive
{
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

	enum DSSTYPES
	{
		DSSTYPE_DEFAULT,
		DSSTYPE_COUNT
	};

	enum RSSTYPES
	{
		RSSTYPE_CULLBACK_SOLID,
		RSSTYPE_COUNT
	};

	
	enum SMPSTYPES
	{
	};
}