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

		// 기본적인 resource bind
		{
			// 이전 set과 비교하는 편이 나을 것 같다. => 일단은 그냥 구성하는 편이 나으려나..?
			// spartan은 이 곳에서 PipelineState의 기본 설정을 구성하고 아래 Render Mesh에서 완성한 후 CommandList::BeginRenderPass()에 전달하여 bind한다.
			// 이때 BeginRenderPass()는 이전 리소스들과 비교를 수행하기도 한다.
			// 여기에서 CommandList는 GraphicsDevice에서 Bind(), Draw()를 전담하는 객체로, 모든 Pass()의 기본 매개변수이다. 
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

		HRESULT result;
		D3D11_DEPTH_STENCIL_DESC desc;

		// default
		{
			ZeroMemory(&desc, sizeof(desc));
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

		HRESULT result;
		D3D11_RASTERIZER_DESC desc;
		
		// cullback_solid
		{
			ZeroMemory(&desc, sizeof(desc));
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