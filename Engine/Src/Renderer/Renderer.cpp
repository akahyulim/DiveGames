#include "divepch.h"
#include "Renderer.h"
#include "ViewScreen.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Shader.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Renderer* Renderer::s_pInstance = nullptr;

	Renderer::Renderer()
		: m_pDevice(Graphics::GetInstance()->GetDevice())
		, m_RenderTargets{}
		, m_Samplers{}
		, m_bInitialized(false)
	{
		DV_ENGINE_ASSERT(m_pDevice);
	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	bool Renderer::Initialize()
	{
		if (IsInitialized())
			Shutdown();
		
		if (!createSamplers())
			return false;

		m_bInitialized = true;

		DV_ENGINE_TRACE("렌더러 시스템 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pSampler : m_Samplers)
			DV_RELEASE(pSampler);

		for (auto pViewScreen : m_ViewScreens)
			DV_DELETE(pViewScreen);
		m_ViewScreens.clear();

		DV_ENGINE_TRACE("렌더러 시스템 셧다운에 성공하였습니다.");
	}

	void Renderer::Update()
	{
		for (auto pViewport : m_ViewScreens)
			pViewport->Update();
	}

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

	bool Renderer::createRenderTextures()
	{
		return true;
	}

	bool Renderer::createSamplers()
	{
		D3D11_SAMPLER_DESC desc;

		// base
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			if (FAILED(m_pDevice->CreateSamplerState(&desc, &m_Samplers[static_cast<uint8_t>(eSamplerType::Linear)])))
			{
				DV_ENGINE_ERROR("Base Sampler 생성에 실패하였습니다.");
				return false;
			}
		}

		// pcf
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			if (FAILED(m_pDevice->CreateSamplerState(&desc, &m_Samplers[static_cast<uint8_t>(eSamplerType::Pcf)])))
			{
				DV_ENGINE_ERROR("Pcf Sampler 생성에 실패하였습니다.");
				return false;
			}
		}

		return true;
	}

	Renderer* GetRenderer()
	{
		return Renderer::GetInstance();
	}
}
