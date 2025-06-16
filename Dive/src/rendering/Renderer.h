#pragma once

namespace Dive
{
	class RenderTexture;
	class Shader;

	enum class eShader
	{
		DeferredPass_VS,
		DeferredPass_PS,
		LightingPass_VS,
		LightingPass_PS,
		DebugDraw_VS,
		DebugDraw_PS,
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

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void OnUpdate();

		static void DeferredPass();
		static void LightingPass();

		static void ResizeRenderBuffers(UINT32 width, UINT32 height);

		static UINT32 GetRenderTargetWidth() { return s_RenderTargetWidth; }
		static UINT32 GetRenderTargetHeight() { return s_RenderTargetHeight; }

		static RenderTexture* GetGBuffer(eGBuffer type);
		static RenderTexture* GetRenderTarget(eRenderTarget type);
		static ID3D11RasterizerState* GetRasterizerState(eRasterizerState type);
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState type);
		static ID3D11BlendState* GetBlendState(eBlendState type);

	private:
		//static void createConstantBuffers();
		static void createShaders();
		static void createRasterizerStates();
		static void createDepthStencilStates();
		static void createBlendStates();
		
	private:
		static UINT32 s_RenderTargetWidth;
		static UINT32 s_RenderTargetHeight;

		// vertex shaders
	
		
		// pixel shaders
		

		// 상수 버퍼

		// G-Buffer
		static RenderTexture* s_GBufferRT0;
		static RenderTexture* s_GBufferRT1;
		static RenderTexture* s_GBufferRT2;

		// RenderTargets
		static RenderTexture* s_FrameRT;
		static RenderTexture* s_OutputRT;
		
		static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> s_RenderTargets;
		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> s_RasterizerStates;
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> s_DepthStencilStates;
		static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> s_BlendStates;
	

		// camera
		// viewport
	};
}