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
		
		m_bInitialized = true;

		DV_ENGINE_TRACE("렌더러 시스템 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
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

	Renderer* GetRenderer()
	{
		return Renderer::GetInstance();
	}
}
