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

	// shader �̸��� �޶� �ǳ�...
	enum ILTYPES
	{
		ILTYPE_POS_TEX2,	// �̰� ��Ʈ������ VertexType�� �ϳ��� ���������.
		ILTYPE_POS_TEX_NOR_TAN,

		ILTYPE_COUNT
	};
	// ���� enum�鵵 ���� enum class�� �ٲ���.

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