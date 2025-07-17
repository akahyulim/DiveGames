#pragma once
#include "RenderDefs.h"

namespace Dive
{
	class ConstantBuffer;
	class RenderTexture;
	class Shader;
	class RenderPass;

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void OnUpdate();

		static void ResizeRenderBuffers(UINT32 width, UINT32 height);

		static UINT32 GetRenderTargetWidth() { return s_RenderTargetWidth; }
		static UINT32 GetRenderTargetHeight() { return s_RenderTargetHeight; }

		static RenderTexture* GetGBuffer(eGBuffer type);
		static RenderTexture* GetRenderTarget(eRenderTarget type);
		static ID3D11RasterizerState* GetRasterizerState(eRasterizerState type);
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState type);
		static ID3D11BlendState* GetBlendState(eBlendState type);

	private:
		static void createConstantBuffers();
		static void createShaders();
		static void createRasterizerStates();
		static void createDepthStencilStates();
		static void createBlendStates();
		
	private:
		static UINT32 s_RenderTargetWidth;
		static UINT32 s_RenderTargetHeight;

		// 상수 버퍼
		static ConstantBuffer* s_DefaultVSConstantBuffer;

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
	

		static std::vector<std::unique_ptr<RenderPass>> s_RenderPasses;
	};
}
