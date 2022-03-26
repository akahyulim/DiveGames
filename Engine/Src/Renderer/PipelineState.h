#pragma once

namespace Dive
{
	struct PipelineState
	{
		PipelineState() = default;
		
		PipelineState& operator=(const PipelineState& ref);
		bool operator==(const PipelineState& ref);

		bool IsValid();

		// Input Assembly
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		ID3D11InputLayout* pInputLayout = nullptr;

		// Vertex Shader
		ID3D11VertexShader* pVertexShader = nullptr;

		// Rasterizer
		ID3D11RasterizerState* pRasterizerState = nullptr;
		D3D11_VIEWPORT* pViewport = nullptr;

		// Pixel Shader
		ID3D11PixelShader* pPixelShader = nullptr;

		// Output Merge
		ID3D11BlendState* pBlendState = nullptr;
		ID3D11DepthStencilState* pDepthStencilState = nullptr;
		ID3D11DepthStencilView* pDepthStencilView = nullptr;		// 추후 readonly가 추가될 수 있다.
		std::array<ID3D11RenderTargetView*, 8> renderTargetViews = { nullptr };
	};
}