#include "divepch.h"
#include "Engine.h"
#include "CoreDefs.h"
#include "StringUtils.h"
#include "Window.h"
#include "CoreEvents.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceManager.h"
#include "Input/Input.h"
#include "World/SceneManager.h"	// 제거 대상
#include "World/Scene.h"

namespace Dive
{
	DvEngine* DvEngine::s_pInstance = nullptr;
	DvEngine* GEngine = Dive::DvEngine::GetInstance();
	Scene* GWorld = nullptr;

	DvEngine::DvEngine()
		: m_pAppWindow(nullptr)
		, m_pGraphics(nullptr)
	{
	}

	DvEngine::~DvEngine()
	{
		DvRenderer::Shutdown();
		Window::Shutdown();
	}

	bool DvEngine::Initialize(bool fullScreen)
	{
		if (m_pAppWindow)
		{
			m_pGraphics = std::make_unique<DvGraphics>();
			if (!m_pGraphics->Initialize(m_pAppWindow->GetWidth(), m_pAppWindow->GetHeight(), m_pAppWindow->GetHandle(), fullScreen))
				return false;
		}
		else
		{
			DV_LOG(Engine, err, "메인 윈도우가 존재하지 않아 초기화에 실패하였습니다.");
			return false;
		}

		DV_LOG(Engine, trace, "초기화 성공");

		return true;
	}

	bool DvEngine::Initialize(const std::wstring& title, uint32_t width, uint32_t height, bool fullScreen)
	{
		LogManager::SetFilename(StringUtils::WStringToString(title) + ".log");

		if (!Window::Initialize())
			return false;
		Window::ShowWindow(SW_HIDE);

		if (!DvRenderer::Initialize())
			return false;

		Window::SetTitle(title.c_str());
		Window::ShowWindow(SW_SHOW);

		DV_LOG(Engine, trace, "초기화 성공");

		return true;
	}

	bool DvEngine::IsRun()
	{
		return Window::Run();
	}
	
	LRESULT DvEngine::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
		{
			RECT rt{};
			::GetClientRect(hWnd, &rt);
			DvRenderer::ChangeScreenSize(static_cast<uint32_t>(rt.right - rt.left), static_cast<uint32_t>(rt.bottom - rt.top));
			return 0;
		}
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// 임시...
	void DvEngine::Draw()
	{
		//pGraphics->BindRenderTargetView(pGraphics->GetDefaultRenderTargetView(), 0);
		//pGraphics->BindDepthStencilView(pGraphics->GetDefaultDepthStencilView());
		//pGraphics->ClearViews(eClearFlags::Color | eClearFlags::Depth | eClearFlags::Stencil, { 1.0f, 0.0f, 0.0f, 1.0f });

		//pGraphics->GetSwapChain()->Present(1, 0);
	}

	void DvEngine::Present()
	{
		ID3D11RenderTargetView* pRenderTargetView[] = { m_pGraphics->GetDefaultRenderTargetView() };
		m_pGraphics->GetDeviceContext()->OMSetRenderTargets(1, pRenderTargetView, m_pGraphics->GetDefaultDepthStencilView());

		FLOAT clearColor[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_pGraphics->GetDeviceContext()->ClearRenderTargetView(pRenderTargetView[0], clearColor);
		m_pGraphics->GetDeviceContext()->ClearDepthStencilView(
			m_pGraphics->GetDefaultDepthStencilView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f, 0);

		m_pGraphics->GetSwapChain()->Present(1, 0);
	}

	Scene* DvEngine::GetWorldByName(const std::string& name) const
	{
		for (auto pWorld : m_Worlds)
		{
			if (pWorld->GetName() == name)
				return pWorld;
		}
		return nullptr;
	}

	void DvEngine::AddWorld(Scene* pWorld)
	{
		if (pWorld)
		{
			m_Worlds.emplace_back(pWorld);
		}
	}

	void DvEngine::RemoveWorld(Scene* pWorld)
	{
		if (pWorld)
		{
			auto it = std::find(m_Worlds.begin(), m_Worlds.end(), pWorld);
			if (it != m_Worlds.end())
			{
				m_Worlds.erase(it);
				return;
			}
		}
	}

	//==================================================================================================================================================

	Engine* Engine::s_pInstance = nullptr;

	Engine::Engine()
		: m_bInitialized(false)
		, m_bExit(false)
		, m_TimeMS(0.0)
		, m_DeltaTimeMS(0.0)
		, m_LastTickTime(std::chrono::steady_clock::now())
		, m_FrameCount(0)
		, m_LastTime(0)
		, m_Fps(0.0)
	{
	}

	bool Engine::Initialize(uint32_t width, uint32_t height, bool fullScreen)
	{
		//Log::GetInstance()->Initialize(spdlog::level::trace, spdlog::level::trace);

		if (!Graphics::GetInstance()->Initialize(width, height, fullScreen, false))
		{
			DV_LOG(Engine, critical, "그래픽스 시스템 초기화에 실패하였습니다.");
			return false;
		}

		if (!Renderer::GetInstance()->Initialize())
		{
			DV_LOG(Engine, critical, "렌더러 시스템 초기화에 실패하였습니다.");
			return false;
		}

		if (!Input::GetInstance()->Initialize())
		{
			DV_LOG(Engine, critical, "인풋 시스템 초기화에 실패하였습니다.");
			return false;
		}

		m_bInitialized = true;

		DV_LOG(Engine, trace,  "엔진 초기화 성공.");

		return true;
	}

	void Engine::Shutdown()
	{
		SceneManager::GetInstance()->Shutdown();	// 자주 사용하지 않는 객체를 굳이 멤버 변수로 관리할 필요는 없어보인다.
		ResourceManager::GetInstance()->Shutdown();

		Input::GetInstance()->Shutdown();
		Renderer::GetInstance()->Shutdown();
		Graphics::GetInstance()->Shutdown();

		DV_LOG(Engine, trace,  "엔진 셧다운에 성공하였습니다.");
	}
	
	void Engine::RunFrame()
	{
		if (!Graphics::GetInstance()->RunWindow())
		{
			m_bExit = true;
			return;
		}

		m_DeltaTimeMS = static_cast<double>(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_LastTickTime).count());
		m_TimeMS += m_DeltaTimeMS;
		m_LastTickTime = std::chrono::steady_clock::now();

		m_FrameCount++;
		if (m_TimeMS - m_LastTime >= 1000.0)
		{
			m_Fps = m_FrameCount;
			m_FrameCount = 0;
			m_LastTime = m_TimeMS;
		}

		OnUpdate();
		OnRender();
	}
	
	void Engine::OnUpdate()
	{
		FIRE_EVENT(PreUpdateEvent());

		Renderer::GetInstance()->Update();
		Input::GetInstance()->Update();

		FIRE_EVENT(PostUpdateEvent());
	}
	// Update와 Render는 확장성을 고려해 앱에서 직접 호출하도록 만드는 편이 나을 것 같다.
	void Engine::OnRender()
	{
		Graphics::GetInstance()->BeginFrame();
		
		FIRE_EVENT(PreRenderEvent());

		Renderer::GetInstance()->Render();

		FIRE_EVENT(PostRenderEvent());

		Graphics::GetInstance()->EndFrame();
	}
	
	bool Engine::IsInitialized()
	{
		return m_bInitialized;
	}
	
	bool Engine::IsExit()
	{
		return m_bExit;
	}

	double Engine::GetFps()
	{
		return m_Fps;
	}

	double Engine::GetTimeMS()
	{
		return m_TimeMS;
	}

	double Engine::GetTimeSec()
	{
		return m_TimeMS / 1000.0;
	}

	double Engine::GetDeltaTimeMS()
	{
		return m_DeltaTimeMS;
	}

	double Engine::GetDeltaTimeSec()
	{
		return m_DeltaTimeMS / 1000.0;
	}

	Engine* GetEngine()
	{
		if (Engine::GetInstance()->IsInitialized())
			return Engine::GetInstance();

		return nullptr;
	}
}