#pragma once

namespace Dive
{
	struct PipelineState
	{
		ID3D11DepthStencilView* pDepthStencilView = nullptr;
		ID3D11RasterizerState* pRasterizerState = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		ID3D11VertexShader* pVertexShader = nullptr;
		ID3D11InputLayout* pInputLayout = nullptr;	
		ID3D11PixelShader* pPixelShader = nullptr;

		// render targets
	};
}