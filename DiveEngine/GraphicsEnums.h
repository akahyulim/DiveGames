#pragma once

namespace Dive
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
		VSTYPE_COLOR,

		// index
		PSTYPE_COLOR,

		SHADERTYPE_COUNT
	};

	// shader �̸��� �޶� �ǳ�...
	enum ILTYPES
	{
		ILTYPE_POS,
		ILTYPE_POS_COL,
		ILTYPE_POS_TEX,

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