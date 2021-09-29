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

	// wicked�� visibility, flag, renderpass ���� �޴´�.
	void dvRenderer::DrawScene()
	{
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext);

		// �⺻���� resource bind
		{
			// ���� set�� ���ϴ� ���� ���� �� ����. => �ϴ��� �׳� �����ϴ� ���� ��������..?
			// spartan�� �� ������ PipelineState�� �⺻ ������ �����ϰ� �Ʒ� Render Mesh���� �ϼ��� �� CommandList::BeginRenderPass()�� �����Ͽ� bind�Ѵ�.
			// �̶� BeginRenderPass()�� ���� ���ҽ���� �񱳸� �����ϱ⵵ �Ѵ�.
			// ���⿡�� CommandList�� GraphicsDevice���� Bind(), Draw()�� �����ϴ� ��ü��, ��� Pass()�� �⺻ �Ű������̴�. 
			pImmediateContext->OMSetDepthStencilState(m_pDepthStencilStates[DEPTH_STENCIL_STATE_DEFAULT].Get(), 0);
			pImmediateContext->RSSetState(m_pRasterizerStates[RASTERIZER_STATE_CULLBACK_SOLID].Get());
			// sampler
			// vertex shader
			// pixel shader
			// inputlyaout
			// primitive topology??? �̰� flag�� �����ٵ�...
		}

		// ���� object draw

		// sort visibility
		// render mesh: visibility���� ������ ������̴�.
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

			// OP���� �͵��� GBuffer�� �ٸ���.
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