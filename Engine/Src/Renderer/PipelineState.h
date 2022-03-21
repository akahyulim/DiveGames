#pragma once

namespace Dive
{
	struct PipelineState
	{
		bool IsValid();

		// Input Assembly
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		ID3D11InputLayout* pInputLayout = nullptr;

		// Vertex Shader
		ID3D11VertexShader* pVertexShader = nullptr;

		// Rasterizer
		ID3D11RasterizerState* pRasterizerState = nullptr;

		// Pixel Shader
		ID3D11PixelShader* pPixelShader = nullptr;

		// Output Merge
		ID3D11DepthStencilView* pDepthStencilView = nullptr;
		ID3D11DepthStencilState* pDepthStencilState = nullptr;
		

		// render targets
	};
}