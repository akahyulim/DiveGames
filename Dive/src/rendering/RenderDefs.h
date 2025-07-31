#pragma once

namespace Dive
{
	enum class eSamplerType : uint8_t
	{
		LinearWrap,
		LinearClamp,
		PointWrap,
		PointClamp,
		AnisotropicWrap,
		ShadowComparison,
		Count
	};

	enum class eShader : UINT
	{
		DeferredPass_VS,
		DeferredPass_PS,
		LightingPass_VS,
		LightingPass_PS,
		DebugDraw_VS,
		DebugDraw_PS,
		Count
	};

	// 엔진은 uint8_t로, 윈도우 api 사용시엔 타입변환
	enum class eShaderTextureSlot : uint8_t
	{
		// 🔹 머티리얼 텍스처 슬롯 (Geometry Pass)
		Diffuse = 0,
		NormalMap = 1,
		RoughnessMap = 2,
		MetallicMap = 3,
		EmissiveMap = 4,
		OpacityMap = 5,

		// 🔸 G-Buffer 결과물 슬롯 (Lighting Pass)
		GBuffer_Albedo = 10,
		GBuffer_Normal = 11,
		GBuffer_RoughMetal = 12,
		GBuffer_Emissive = 13,
		GBuffer_Depth = 14,
		GBuffer_MaterialID = 15,

		// 🔸 조명/쉐도우/환경맵
		ShadowMap = 20,
		SSAOMap = 21,
		EnvironmentMap = 22,
		IBL_Irradiance = 23,
		IBL_Specular = 24,
		BRDF_LUT = 25,

		// 🔸 후처리용
		SceneColor = 30,
		SceneDepth = 31,
		BloomTexture = 32,
		MotionVector = 33,

		// ✅ 마지막 슬롯 수 확인용
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
		Addictive,
		Count
	};

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

	enum class eDepthStencilState
	{
		DepthReadWrite,
		DepthReadWrite_StencilReadWrite,
		GBuffer,
		DepthDiabled,   // skydome에서 off용으로...
		ForwardLight,
		Count
	};
}
