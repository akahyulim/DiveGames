#pragma once
#include "RenderDefs.h"
#include "RenderPass.h"
#include "LightManager.h"

namespace Dive
{
	class RenderTexture;
	class Shader;
	//class RenderPass;
	//class LightManager;

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void OnUpdate();

		static void ResizeRenderBuffers(uint32_t width, uint32_t height);

		static uint32_t GetRenderTargetWidth() { return s_renderTargetWidth; }
		static uint32_t GetRenderTargetHeight() { return s_renderTargetHeight; }

		static RenderTexture* GetGBuffer(eGBuffer type);
		static RenderTexture* GetRenderTarget(eRenderTarget type);
		static ID3D11RasterizerState* GetRasterizerState(eRasterizerState type);
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState type);
		static ID3D11BlendState* GetBlendState(eBlendState type);
		// 이게 아니라 그냥 배열로 전달하는 편이 나을 것 같다.
		// 아니면 아에 PSSetSmaplerState를 구성해 놓는 편이 나을지도...
		static ID3D11SamplerState* GetSamplerState(eSamplerState type);
		static void BindSamplerStates();

	private:
		static void CreateRasterizerStates();
		static void CreateDepthStencilStates();
		static void CreateBlendStates();
		static void CreateSamplerStates();
		
	private:
		static uint32_t s_renderTargetWidth;
		static uint32_t s_renderTargetHeight;

		// G-Buffer
		static RenderTexture* s_gBufferRT0;
		static RenderTexture* s_gBufferRT1;
		static RenderTexture* s_gBufferRT2;

		// RenderTargets
		static RenderTexture* s_frameRenderTarget;
		static RenderTexture* s_outputRenderTarget;
		
		static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> s_renderTargets;
		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> s_rasterizerStates;
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> s_depthSteniclStates;
		static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> s_blendStates;
		static std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> s_samplerStates;
	
		static std::vector<std::unique_ptr<RenderPass>> s_renderPasses;
		static std::unique_ptr<LightManager> s_lightManager;
	};
}
