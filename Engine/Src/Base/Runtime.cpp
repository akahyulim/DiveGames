#include "divepch.h"
#include "Runtime.h"
#include "DiveCore.h"
#include "RenderPath.h"
#include "Event.h"
#include "IniHelper.h"
#include "Time.h"
#include "Log.h"
#include "Renderer/Renderer.h"
#include "Resource/ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Input/Input.h"

namespace Dive
{
	Runtime* Runtime::s_pInstance = nullptr;

	Runtime::Runtime(const std::string& name)
		: m_AppName(name)
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		Log::Initialize();
		Time::Initialize();
	}

	Runtime::~Runtime()
	{
		// static class 중 몇 가지는 직접 Shutdown() 호출
	}

	// 그냥 여기에서 윈도우 데이터를 받는 편이 나을 것 같은데...
	bool Runtime::Initialize()
	{
		DV_ASSERT(m_hWnd);

		if (m_bInitialized)
		{
			return false;
		}

		Settings::GetInstance().Initialize(m_AppName);
		{
			auto& settings = Settings::GetInstance();
			eWindowModes mode = settings.GetWindowMode();
			if (mode == eWindowModes::FullScreen)
				m_bFullScreen = true;
			else
				m_bFullScreen = false;
			bool maximize = settings.IsMaximize();
			unsigned int width = settings.GetWidth();
			unsigned int height = settings.GetHeight();
			
			ModifyWindow(mode, width, height, maximize);
		}

		Renderer::GetInstance().Initialize(m_hWnd, m_bFullScreen);
		Input::GetInstance().Initialize(m_hWnd);

		EVENT_SUBSCRIBE(eEventType::ChangedResolution, EVENT_HANDLE_DATA(OnResizeResolution));

		return true;
	}

	void Runtime::Run()
	{
		// 윈도우 핸들을 받기위해 초기화를 이 곳에 둔 것 같은데 이상하다.
		if (!m_bInitialized)
		{
			m_bInitialized = Initialize();
			
			if (!m_bInitialized)
			{
				CORE_ERROR("Rumtime 초기화에 실패하였습니다. 프로그램을 종료합니다.");
				PostQuitMessage(0);
			}
		}

		Time::Update();

		auto delta = Time::GetDeltaTimeSec();

		static float fixedFrameAccumulator = 0.0f;
		static float deltaFrameAccumulator = 0.0f;

	
		if (m_bActiveWindow)
		{
			fixedFrameAccumulator += delta;
			deltaFrameAccumulator += delta;

			if (fixedFrameAccumulator >= Time::GetFixedFrameRate())
			{
				FixedUpdate();
				fixedFrameAccumulator = 0.0f;
			}

			if (m_bFrameLock)
			{
				if (deltaFrameAccumulator >= static_cast<float>(1.0f / m_TargetFrameRate))
				{
					Update(deltaFrameAccumulator);
					Input::GetInstance().Update();
					Render();

					deltaFrameAccumulator = 0.0f;
				}
			}
			else
			{
				Update(delta);
				Input::GetInstance().Update();
				Render();

				deltaFrameAccumulator = 0.0f;
			}
		}
		else
		{
			// 입력 버퍼만 수행
		}

		auto graphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		Compose();

		graphicsDevice->PresentEnd();
	}
	
	void Runtime::Update(float deltaTime)
	{
		if (m_pActivePath)
		{
			m_pActivePath->PreUpdate();
		}

		// lua

		if (m_pActivePath)
		{
			m_pActivePath->Update(deltaTime);
			m_pActivePath->PostUpdate();
		}
	}
	
	void Runtime::FixedUpdate()
	{
		// lua

		if (m_pActivePath)
		{
			// 대상이 좀 애매하다.
			// 사실상 호출 함수는 3d가 상속한 2d의 FixedUpdate()이다.
			// 유니티에서는 rigid body update에 쓰인다고 한다.
			// 아마도 애니메이션의 속도를 일관성있게 만들기 위함인듯 하다.
			// 그런데 이 대상은 Scene에 존재할텐데...
			m_pActivePath->FixedUpdate();
		}
	}
	
	void Runtime::Render()
	{
		// lua

		if (m_pActivePath)
		{
			m_pActivePath->Render();
		}
	}

	void Runtime::Compose()
	{
		if (m_pActivePath)
		{
			m_pActivePath->Compose();
		}
	}

	void Runtime::ActivatePath(RenderPath* pPath)
	{
		// 뭔가 있다.
		{
			if (m_pActivePath)
			{
				m_pActivePath->Stop();
			}

			if (pPath != nullptr)
			{
				pPath->Start();
			}
			m_pActivePath = pPath;
		}
	}
	
	// 이것도 추후 필요 없을 것 같다.
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		m_hWnd = windowHandle;
		m_bFullScreen = fullScreen;
	}

	// 이건 굳이 필요 없을 것 같다.
	// App에서 Window를 제어하는 것이 더 직관적이다.
	void Runtime::ModifyWindow(eWindowModes mode, unsigned int width, unsigned int height, bool maximize)
	{
		unsigned int posX = 0;
		unsigned int posY = 0;

		if (mode == eWindowModes::FullScreen)
		{
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);

			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = static_cast<DWORD>(width);
			dmScreenSettings.dmPelsHeight = static_cast<DWORD>(height);
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		}
		else
		{
			LONG style = 0;

			if (mode == eWindowModes::Windowed)
			{
				style = WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

				RECT rt = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
				AdjustWindowRect(&rt, style, 0);

				width = rt.right - rt.left;
				height = rt.bottom - rt.top;
			}
			else
			{
				style = WS_POPUP;
			}

			SetWindowLong(m_hWnd, GWL_STYLE, style);

			posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		}

		SetWindowPos(m_hWnd, NULL, posX, posY, width, height, 0);

		ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);
	}

	//==============================================================================//
	// 1. Minimize시 크기가 0, 0으로 전달된다.										//
	// 이때 Backbuffer와 RenderTarget의 크기를 0, 0으로 변경해야하나?				//
	//==============================================================================//
	void Runtime::OnResizeResolution(unsigned int data)
	{
		// 이건 클라이언트 영역 크기인가?
		unsigned int width = data & 0xFFFF;
		unsigned int height = (data >> 16) & 0xFFFF;

		CORE_INFO("Editor::OnResizeResolution() : {0:d}x{1:d}", width, height);

		// 이러지 말고 직접 이벤트를 받도록 하는 편이 낫지 않을까?
		// GraphicsDevice에 직접 접근하는게 꺼림직하다.
		Renderer::GetInstance().GetGraphicsDevice()->SetResolution(width, height);

		// 이벤트를 직접 받는다면 이 부분은 어떻게 할 것인가?
		auto& settings = Settings::GetInstance();
		if (IsZoomed(m_hWnd))
		{
			settings.SetMaximize(true);
		}
		else
		{
			settings.SetMaximize(false);
			settings.SetWidth(width);
			settings.SetHeight(height);
		}
	}
}