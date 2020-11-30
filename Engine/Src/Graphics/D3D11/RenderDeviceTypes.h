#pragma once
#include "DivePch.h"

namespace Dive
{
	using ResourceH = ID3D11Resource;	// 바꿔야 할듯?

	using BufferH = ID3D11Buffer;

	using QueryH = ID3D11Query;

	using hSwapChain = IDXGISwapChain;
	using hDevice = ID3D11Device;
	using hDeviceContext = ID3D11DeviceContext;

	using hRTV = ID3D11RenderTargetView;
	using hUAV = ID3D11UnorderedAccessView;
	using hDSV = ID3D11DepthStencilView;
	using hSRV = ID3D11ShaderResourceView;

	using VertexShaderH = ID3D11VertexShader;
	using PixelShaderH = ID3D11PixelShader;

	using InputLayoutH = ID3D11InputLayout;

	using BlendStateH = ID3D11BlendState;
	using RasterizerStateH = ID3D11RasterizerState;
	using DepthStencilStateH = ID3D11DepthStencilState;
	using SamplerStateH = ID3D11SamplerState;

	using ViewportH = D3D11_VIEWPORT;

	struct PipelineState
	{
		InputLayoutH* pInputLauout;

		VertexShaderH* pVertexShader;
		PixelShaderH* pPixelShader;

		// 전부 uint8_t로 선언되어 있었다.
		RasterizerStateH* rasterizerState;
		BlendStateH* blendState;
		DepthStencilStateH* depthStencilState;
	};

	struct SamplerGroup
	{
		SamplerStateH** ppSamplers;
		uint8_t numSamplers;
	};
}