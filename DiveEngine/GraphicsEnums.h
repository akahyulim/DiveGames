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
		VSTYPE_TEXTURING,
		VSTYPE_FONTS,

		// pixel
		PSTYPE_COLOR,
		PSTYPE_TEXTURING,
		PSTYPE_FONTS,

		SHADERTYPE_COUNT
	};

	// shader �̸��� �޶� �ǳ�...
	enum ILTYPES
	{
		ILTYPE_POS,
		ILTYPE_POS_COL,
		ILTYPE_POS_TEX,
		ILTYPE_POS_TEX2,	// �̰� ��Ʈ������ VertexType�� �ϳ��� ���������.

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