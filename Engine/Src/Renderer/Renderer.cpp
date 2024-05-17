#include "divepch.h"
#include "Renderer.h"
#include "ViewScreen.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Renderer* Renderer::s_pInstance = nullptr;

	Renderer::Renderer()
		: m_RasterizerStates{}
		, m_DepthStencilStates{}
		, m_BlendStates{}
		, m_RenderTargets{}
		, m_VSConstantBuffers{}
		, m_PSConstantBuffers{}
		, m_Shaders{}
		, m_GBuffer{}
	{
	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	// ���ο��� Graphics�� ���� ȣ���ϴ� ���� ������ ���� �ʴ´�.
	// �嵥 �̱������� ���� ������ ���� ������ ���ؼ��ݾ�.. ����������ΰ�..?
	bool Renderer::Initialize()
	{
		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
		
		createConstantBuffers();
		createShaders();
		createGBuffer();

		DV_ENGINE_TRACE("������ �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pRenderTexture : m_GBuffer)
			DV_DELETE(pRenderTexture);

		for (auto pViewScreen : m_ViewScreens)
			DV_DELETE(pViewScreen);
		m_ViewScreens.clear();

		for (auto pConstantBuffer : m_PSConstantBuffers)
			DV_DELETE(pConstantBuffer);
		for (auto pConstantBuffer : m_VSConstantBuffers)
			DV_DELETE(pConstantBuffer);

		for (auto pBlendState : m_BlendStates)
			DV_RELEASE(pBlendState);

		for (auto pDepthStencilState : m_DepthStencilStates)
			DV_RELEASE(pDepthStencilState);

		for (auto pRasterizerState : m_RasterizerStates)
			DV_RELEASE(pRasterizerState);

		DV_ENGINE_TRACE("������ �ý��� �˴ٿ �����Ͽ����ϴ�.");
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
		DV_ENGINE_ASSERT(count >= 0);
		m_ViewScreens.resize(count);
	}

	ViewScreen* Renderer::GetViewScreen(uint32_t index)
	{
		DV_ENGINE_ASSERT(index >= 0);
		return index < m_ViewScreens.size() ? m_ViewScreens[index] : nullptr;
	}

	void Renderer::SetViewScreen(uint32_t index, ViewScreen* pViewScreen)
	{
		DV_ENGINE_ASSERT(index >= 0);
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

		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullBack)])))
		{
			DV_ENGINE_ERROR("RasterizerState FillSolid_CullBack ������ �����Ͽ����ϴ�.");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetInstance()->GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_ENGINE_ERROR("RasterizerState FillSolid_CullNode ������ �����Ͽ����ϴ�.");
		}

		{
			// NoDepthClipFront
			D3D11_RASTERIZER_DESC desc = {
				D3D11_FILL_SOLID,
				D3D11_CULL_FRONT,
				FALSE,
				D3D11_DEFAULT_DEPTH_BIAS,
				D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
				D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
				TRUE,
				FALSE,
				FALSE,
				FALSE };

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::NoDepthClipFront)])))
			{
				DV_ENGINE_ERROR("RasterizerState NoDepthClipFront ������ �����Ͽ����ϴ�.");
			}

			// ShadowGen
			desc.CullMode = D3D11_CULL_BACK;
			desc.DepthBias = 85;
			desc.SlopeScaledDepthBias = 5.0f;
			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerState::ShadowGen)])))
			{
				DV_ENGINE_ERROR("RasterizerState ShadowGen ������ �����Ͽ����ϴ�.");
			}
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
			{
				DV_ENGINE_ERROR("DepthStencilState DepthReadWrite ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
			{
				DV_ENGINE_ERROR("DepthStencilState DepthReadWrite_StencilReadWrite ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
			{
				DV_ENGINE_ERROR("DepthStencilState GBuffer ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthDisabled)])))
			{
				DV_ENGINE_ERROR("DepthDisabledStencilState ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilState::ForwardLight)])))
			{
				DV_ENGINE_ERROR("DepthDisabledStencilState ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendState::Additive)])))
			{
				DV_ENGINE_ERROR("BlandState Additive ������ �����Ͽ����ϴ�.");
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendState::Transparent)])))
			{
				DV_ENGINE_ERROR("BlandState Transparent ������ �����Ͽ����ϴ�.");
			}
		}
	}

	void Renderer::createRenderTextures()
	{
	}

	void Renderer::createConstantBuffers()
	{
		// vs
		m_VSConstantBuffers[static_cast<uint32_t>(eVSConstBuf::Model)] =
			ConstantBuffer::Create<VSConstBuf_Model>("Model", eShaderType::Vertex, (uint32_t)eVSConstBuf::Model);

		m_VSConstantBuffers[static_cast<uint32_t>(eVSConstBuf::Camera)] =
			ConstantBuffer::Create<VSConstBuf_Camera>("Camera", eShaderType::Vertex, (uint32_t)eVSConstBuf::Camera);

		m_VSConstantBuffers[static_cast<uint32_t>(eVSConstBuf::Light)] =
			ConstantBuffer::Create<VSConstBuf_Light>("Light", eShaderType::Vertex, (uint32_t)eVSConstBuf::Light);

		// ps
		m_PSConstantBuffers[static_cast<uint32_t>(ePSConstBuf::Model)] =
			ConstantBuffer::Create<PSConstBuf_Model>("Material", eShaderType::Pixel, (uint32_t)ePSConstBuf::Model);

		m_PSConstantBuffers[static_cast<uint32_t>(ePSConstBuf::Camera)] =
			ConstantBuffer::Create<PSConstBuf_Camera>("Camera", eShaderType::Pixel, (uint32_t)ePSConstBuf::Camera);

		m_PSConstantBuffers[static_cast<uint32_t>(ePSConstBuf::Light)] =
			ConstantBuffer::Create<PSConstBuf_Light>("Light", eShaderType::Pixel, (uint32_t)ePSConstBuf::Light);
	}

	void Renderer::createShaders()
	{
		auto pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/LightDepth.hlsl", eShaderType::Vertex, eInputLayout::Pos);
		m_Shaders[static_cast<uint32_t>(eRendererShader::Depth_VS)] = pShader;

		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/ForwardLightShadow.hlsl", eShaderType::Vertex, eInputLayout::Static_Model);
		m_Shaders[static_cast<uint32_t>(eRendererShader::ForwardLight_VS)] = pShader;
		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/ForwardLightShadow.hlsl", eShaderType::Pixel);
		m_Shaders[static_cast<uint32_t>(eRendererShader::ForwardLight_PS)] = pShader;

		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/Deferred.hlsl", eShaderType::Vertex, eInputLayout::Static_Model);
		m_Shaders[static_cast<uint32_t>(eRendererShader::GBuffer_VS)] = pShader;
		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/Deferred.hlsl", eShaderType::Pixel);
		m_Shaders[static_cast<uint32_t>(eRendererShader::GBuffer_PS)] = pShader;

		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredLights.hlsl", eShaderType::Vertex, eInputLayout::Pos);
		m_Shaders[static_cast<uint32_t>(eRendererShader::DeferredLight_VS)] = pShader;
		pShader = new Shader();
		pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredLights.hlsl", eShaderType::Pixel);
		m_Shaders[static_cast<uint32_t>(eRendererShader::DeferredLight_PS)] = pShader;
	}

	void Renderer::createGBuffer()
	{
		const int width = Graphics::GetInstance()->GetResolutionWidth();
		const int height = Graphics::GetInstance()->GetResolutionHeight();

		// DepthStencil
		// ���� ������ ��. �̷��� ������ �����ϸ� Texture2D�� �������� ���� �� �ִ�.
		// ���� RTV�� DSV�� �����ϴ� �Լ��� �����ϰ� Texture2D�� ������ �����ϵ��� �����ؾ� �Ѵ�.
		auto pDepthStencilTexture = new RenderTexture();
		pDepthStencilTexture->SetDepthStencil(width, height, DXGI_FORMAT_R24G8_TYPELESS, true);
		m_GBuffer[static_cast<size_t>(eGBuffer::DepthStencil)] = pDepthStencilTexture;

		// ColorSpecIntensity
		auto pColorTexture = new RenderTexture();
		pColorTexture->SetRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_GBuffer[static_cast<size_t>(eGBuffer::ColorSpecIntensity)] = pColorTexture;

		// Normal
		auto pNormalTexture = new RenderTexture();
		pNormalTexture->SetRenderTarget(width, height, DXGI_FORMAT_R11G11B10_FLOAT);
		m_GBuffer[static_cast<size_t>(eGBuffer::Normal)] = pNormalTexture;

		// SpecPower
		auto pSpecTexture = new RenderTexture();
		pSpecTexture->SetRenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_GBuffer[static_cast<size_t>(eGBuffer::SpecPower)] = pSpecTexture;
	}

	Renderer* GetRenderer()
	{
		return Renderer::GetInstance();
	}
}
