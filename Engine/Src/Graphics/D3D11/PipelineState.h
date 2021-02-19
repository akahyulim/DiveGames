#pragma once
#include "DivePch.h"

namespace Dive
{
	class Shader;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class Dive_Texture;
	// sampler는 왜인지 모르겠지만 Data같다.

	// Spratan 기준으로 Buffer(Vertex, Index)와 Model Texture를 제외하곤 전부  State이다.
	struct PipelineState
	{
		PipelineState()		= default;
		~PipelineState()	= default;

		Shader* vertexShader						= nullptr;
		Shader* pixelShader							= nullptr;
		RasterizerState* rasterizerState			= nullptr;
		BlendState* blendState						= nullptr;
		DepthStencilState* depthStencilState		= nullptr;

		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		D3D11_VIEWPORT viewport;
		
		ID3D11RenderTargetView* swapChainRenderTargetView = nullptr;
		Dive_Texture* depthStencilTexture = nullptr;			// 따로 구분했다.
		std::vector<Dive_Texture*> renderTargets;				// 최대 8장이다. 이게 결국 GBuffer다.

	};
}