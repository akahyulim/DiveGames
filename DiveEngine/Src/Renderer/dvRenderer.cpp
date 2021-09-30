#include "dvRenderer.h"
#include "../DiveCore.h"
#include "../Log.h"
using namespace std;

namespace dive
{
	bool dvRenderer::Initialize(HWND hWnd, bool fullScreen)
	{
		// GraphicsDevice
		{
			m_pGraphicsDevice = make_shared<GraphicsDevice>(hWnd, fullScreen);
			DV_ASSERT(m_pGraphicsDevice->IsInitialized());
		}

		// States
		{
			createSamplerStates();
			createDepthStencilStates();
			createRasterizerStates();
			createBlendStates();
		}

		// Resources
		{
			createShaders();
		}

		return true;
	}

	// wicked는 visibility, flag, renderpass 등을 받는다.
	void dvRenderer::DrawScene()
	{
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);

		// PipelineState
		{
			// PipelineState를 만들어서 Bind하느냐, 그냥 직접 Bind하느냐로 갈린다.
			// 멀티 쓰레드 렌더링 여부와 상관관계를 알아야 한다.
			// 단순히 편의성때문이라면 PipelineState를 만드는 편이 낫다.
			pImmediateContext->OMSetDepthStencilState(m_pDepthStencilStates[DEPTH_STENCIL_STATE_DEFAULT].Get(), 0);
			pImmediateContext->RSSetState(m_pRasterizerStates[RASTERIZER_STATE_CULLBACK_SOLID].Get());
			// sampler
			// vertex shader
			// pixel shader
			// inputlyaout
			// primitive topology??? 이건 flag를 따를텐데...
		}

		// 선행 object draw

		// sort visibility
		// render mesh: visibility에서 가공된 결과물이다.
	}

	bool dvRenderer::createSamplerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice);

		return true;
	}
	
	bool dvRenderer::createDepthStencilStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice);

		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		HRESULT result;

		// default
		{
			desc.DepthEnable		= TRUE;
			desc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc			= D3D11_COMPARISON_LESS;

			desc.StencilEnable		= TRUE;
			desc.StencilReadMask	= 0xFF;
			desc.StencilWriteMask	= 0xFF;

			// OP붙은 것들이 GBuffer와 다르다.
			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			result = pDevice->CreateDepthStencilState(&desc, m_pDepthStencilStates[DEPTH_STENCIL_STATE_DEFAULT].GetAddressOf());
			DV_ASSERT(SUCCEEDED(result));
		}

		return true;
	}
	
	bool dvRenderer::createRasterizerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice);

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		HRESULT result;

		// cullback_solid
		{
			desc.AntialiasedLineEnable	= false;
			desc.CullMode				= D3D11_CULL_BACK;
			desc.DepthBias				= 0;
			desc.DepthBiasClamp			= 0.0f;
			desc.DepthClipEnable		= true;
			desc.FillMode				= D3D11_FILL_SOLID;
			desc.FrontCounterClockwise	= false;
			desc.MultisampleEnable		= false;
			desc.ScissorEnable			= false;
			desc.SlopeScaledDepthBias	= 0.0f;

			result = pDevice->CreateRasterizerState(&desc, m_pRasterizerStates[RASTERIZER_STATE_CULLBACK_SOLID].GetAddressOf());
			DV_ASSERT(SUCCEEDED(result));
		}

		return true;
	}

	bool dvRenderer::createBlendStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice);

		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		
		HRESULT result;

		// opaque
		{
			result = pDevice->CreateBlendState(&desc, m_pBlendStates[BLEND_STATE_OPAQUE].GetAddressOf());
			DV_ASSERT(SUCCEEDED(result));
		}

		return true;
	}
	
	bool dvRenderer::createShaders()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice);

		// vertex shaders
		{

		}

		// pixel shaders
		{

		}

		return true;
	}
}