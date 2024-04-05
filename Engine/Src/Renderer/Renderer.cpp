#include "divepch.h"
#include "Renderer.h"
#include "ViewScreen.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	std::vector<ViewScreen*> Renderer::m_ViewScreens;

	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Total)> Renderer::m_RasterizerStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> Renderer::m_DepthStencilStates;
	std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Total)> Renderer::m_BlendStates;

	std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Total)> Renderer::m_RenderTargets;
	
	std::array<ConstantBuffer*, static_cast<size_t>(eConstantBuffer::Count)> Renderer::m_ConstantBuffers;

	bool Renderer::Initialize()
	{
		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
		createConstantBuffers();

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pViewScreen : m_ViewScreens)
			DV_DELETE(pViewScreen);
		m_ViewScreens.clear();

		for (auto pConstantBuffer : m_ConstantBuffers)
			DV_DELETE(pConstantBuffer);

		for (auto pBlendState : m_BlendStates)
			DV_RELEASE(pBlendState);

		for (auto pDepthStencilState : m_DepthStencilStates)
			DV_RELEASE(pDepthStencilState);

		for (auto pRasterizerState : m_RasterizerStates)
			DV_RELEASE(pRasterizerState);
	}

	// renderable�� �з��Ѵ�.
	void Renderer::Update()
	{
		for (auto pViewport : m_ViewScreens)
			pViewport->Update();
	}

	// renderPath�� ���� �׸���.
	void Renderer::Render()
	{
		for (auto pViewport : m_ViewScreens)
			pViewport->Render();
	}

	void Renderer::SetNumViewScreens(uint32_t count)
	{
		DV_CORE_ASSERT(count >= 0);
		m_ViewScreens.resize(count);
	}

	ViewScreen* Renderer::GetViewScreen(uint32_t index)
	{
		DV_CORE_ASSERT(index >= 0);
		return index < m_ViewScreens.size() ? m_ViewScreens[index] : nullptr;
	}
	
	void Renderer::SetViewScreen(uint32_t index, ViewScreen* pViewScreen)
	{
		DV_CORE_ASSERT(index >= 0);
		if (index >= m_ViewScreens.size())
			m_ViewScreens.resize(index + 1);

		m_ViewScreens[index] = pViewScreen;
	}

	void Renderer::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// FillSolid_CullBack
		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.ScissorEnable = FALSE;
		desc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullBack)])))
		{
			DV_CORE_ERROR("RasterizerState FillSolid_CullBack ������ �����Ͽ����ϴ�.");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_CORE_ERROR("RasterizerState FillSolid_CullNode ������ �����Ͽ����ϴ�.");
		}
	}
	
	void Renderer::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc;

		// DepthReadWrite
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite ������ �����Ͽ����ϴ�.");
			}
		}

		// DepthReadWrite_StencilReadWrite => Skydome���� on
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite_StencilReadWrite ������ �����Ͽ����ϴ�.");
			}
		}

		// GBuffer���� �ϳ� �ִ�.
		// DepthReadWrite_StencilReadWrite
		// ������ Face ������ ���� �ٸ���.
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
			{
				DV_CORE_ERROR("DepthStencilState GBuffer ������ �����Ͽ����ϴ�.");
			}
		}

		// skydome���� depth off��
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = FALSE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthDiabled)])))
			{
				DV_CORE_ERROR("DepthDisabledStencilState ������ �����Ͽ����ϴ�.");
			}
		}

		// forwardLight
		{
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			desc.FrontFace = noSkyStencilOp;
			desc.BackFace = noSkyStencilOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::ForwardLight)])))
			{
				DV_CORE_ERROR("DepthDisabledStencilState ������ �����Ͽ����ϴ�.");
			}
		}
	}
	
	void Renderer::createBlendStates()
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// Additive
		{
			desc.AlphaToCoverageEnable = FALSE;
			desc.IndependentBlendEnable = FALSE;
			const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				TRUE,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_COLOR_WRITE_ENABLE_ALL,
			};
			for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				desc.RenderTarget[i] = defaultRenderTargetBlendDesc;

			if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendState::Additive)])))
			{
				DV_CORE_ERROR("BlandState Additive ������ �����Ͽ����ϴ�.");
			}
		}

		// transparent
		{
			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendState::Transparent)])))
			{
				DV_CORE_ERROR("BlandState Transparent ������ �����Ͽ����ϴ�.");
			}
		}
	}
	
	void Renderer::createRenderTextures()
	{
	}

	void Renderer::createConstantBuffers()
	{
		m_ConstantBuffers[static_cast<size_t>(eConstantBuffer::Frame)] =
			ConstantBuffer::Create<FrameBuffer>("Frame", eShaderType::VertexShader, 0);

		m_ConstantBuffers[static_cast<size_t>(eConstantBuffer::Material)] =
			ConstantBuffer::Create<MaterialBuffer>("Material", eShaderType::PixelShader, 0);

		m_ConstantBuffers[static_cast<size_t>(eConstantBuffer::Camera)] =
			ConstantBuffer::Create<CameraBuffer>("Camera", eShaderType::PixelShader, 1);

		m_ConstantBuffers[static_cast<size_t>(eConstantBuffer::Light)] =
			ConstantBuffer::Create<LightBuffer>("Light", eShaderType::PixelShader, 2);
	}
}
