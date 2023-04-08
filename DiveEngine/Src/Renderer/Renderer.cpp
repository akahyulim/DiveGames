#include "DivePch.h"
#include "Renderer.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderVariation.h"
#include "Graphics/InputLayout.h"
#include "Graphics/ConstantBuffer.h"
#include "IO/Log.h"

namespace Dive
{
	static std::vector<View*> s_Views;

	static ShaderVariation* s_pBasicVertexShader = nullptr;
	static ShaderVariation* s_pBasicPixelShader = nullptr;

	static ConstantBuffer* s_pMatrixBuffer = nullptr;

	static ID3D11DepthStencilState* s_pDepthStencilState = nullptr;
	static ID3D11RasterizerState* s_pRasterizerState = nullptr;

	bool Renderer::Initialize()
	{
		if (!createShaders())
		{
			DV_CORE_WARN("셰이더 객체 생성에 실패하였습니다.");
		}

		if (!createConstantBuffers())
		{
			DV_CORE_WARN("CreateBuffers 생성에 실패하였습니다.");
		}

		if (!createDepthStencilStates())
		{
			DV_CORE_WARN("DepthStencilStates 생성에 실패하였습니다.");
		}

		if (!createRasterizerStates())
		{
			DV_CORE_WARN("RasterizerStates 생성에 실패하였습니다.");
		}
		
		DV_CORE_TRACE("Renderer 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		{
			DV_RELEASE(s_pRasterizerState);
			DV_RELEASE(s_pDepthStencilState);
			DV_DELETE(s_pMatrixBuffer);
			DV_DELETE(s_pBasicPixelShader);
			DV_DELETE(s_pBasicVertexShader);
		}

		for (auto pView : s_Views)
			DV_DELETE(pView);

		DV_CORE_TRACE("Renderer 종료에 성공하였습니다.");
	}

	void Renderer::Update(float delta)
	{
		for (auto pView : s_Views)
			pView->Update(delta);
	}

	void Renderer::Render()
	{
		for (auto pView : s_Views)
			pView->Render();
	}

	View* Renderer::GetView(uint32_t index)
	{
		return GetViewCount() > index ? s_Views[index] : nullptr;
	}

	void Renderer::SetView(uint32_t index, View* pView)
	{
		if (GetViewCount() >= index)
			s_Views.insert(s_Views.begin() + index, pView);
		else
			s_Views.emplace_back(pView);
	}

	uint32_t Renderer::GetViewCount()
	{
		return static_cast<uint32_t>(s_Views.size());
	}

	ShaderVariation* Renderer::GetVertexShaderVariation()
	{
		return s_pBasicVertexShader;
	}

	ShaderVariation* Renderer::GetPixelShaderVariation()
	{
		return s_pBasicPixelShader;
	}

	ConstantBuffer* Renderer::GetMatrixBuffer()
	{
		return s_pMatrixBuffer;
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState()
	{
		return s_pDepthStencilState;
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState()
	{
		return s_pRasterizerState;
	}

	bool Renderer::createShaders()
	{
		s_pBasicVertexShader = new ShaderVariation;
		if (!s_pBasicVertexShader->CompileAndCreate(eShaderType::VertexShader, "Assets/Shaders/basic.hlsl", eVertexType::Model))
			return false;

		s_pBasicPixelShader = new ShaderVariation;
		if (!s_pBasicPixelShader->CompileAndCreate(eShaderType::PixelShader, "Assets/Shaders/basic.hlsl"))
			return false;

		return true;
	}

	bool Renderer::createConstantBuffers()
	{
		// matrix buffer
		{
			s_pMatrixBuffer = new ConstantBuffer("MatrixBuffer");
			if (!s_pMatrixBuffer->Create<MatrixBuffer>())
				return false;
		}

		return true;
	}

	bool Renderer::createDepthStencilStates()
	{
		DV_ASSERT(Graphics::IsInitialized());

		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// Set up the description of the stencil state.
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		desc.StencilEnable = true;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_pDepthStencilState)))
		{
			DV_CORE_ERROR("DepthStencilState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool Renderer::createRasterizerStates()
	{
		DV_ASSERT(Graphics::IsInitialized());

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AntialiasedLineEnable = false;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = false;
		desc.MultisampleEnable = false;
		desc.ScissorEnable = false;
		desc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_pRasterizerState)))
		{
			DV_CORE_ERROR("RasterizerState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
}