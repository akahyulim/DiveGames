#pragma once

namespace Dive
{
	enum class eGBuffer
	{
		Rt0,
		Rt1,
		Rt2
	};

	enum class eRenderTarget
	{
		FrameRender,
		FrameOutput,
		Count
	};

	enum class eRasterizerState
	{
		FillSolid_CullBack,
		FillSolid_CullNone,
		Count
	};

	enum class eBlendState
	{
		AlphaEnabled,
		AlphaDisabled,
		Addictive,
		Count
	};

	enum class eSamplerState
	{
		WrapLinear,
		ClampPoint,
		ClampLinear,
		ShadowCompare,
		Count
	};

	enum class eDepthStencilState
	{
		DepthReadWrite,
		DepthReadWrite_StencilReadWrite,
		GBuffer,
		DepthDiabled,   // skydome에서 off용으로...
		ForwardLight,
		Transparent,
		Count
	};
}
