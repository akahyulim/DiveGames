#include "Runtime.h"
#include "Log.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Renderer.h"
#include "Input.h"
#include "AssetManager.h"
#include "Event.h"
#include "IniHelper.h"
#include <memory>

using namespace std;

namespace Dive
{
	Runtime::Runtime()
	{
		m_pAssetManager = nullptr;
		m_title = "diveEngine";
	}

	Runtime::~Runtime()
	{
		if (m_pAssetManager)
		{
			delete m_pAssetManager;
			m_pAssetManager = nullptr;
		}
	}

	bool Runtime::Initialize()
	{
		assert(m_hWnd != 0);

		if (m_bInitialized)
		{
			return false;
		}

		// 순서에 유념해야 한다.
		Log::Initialize();
		Settings::GetInstance().Initialize(m_title);

		// 윈도우 모드, 크기가 매개변수 대상이다.
		{
			auto& settings = Settings::GetInstance();
			eWindowModes mode = settings.GetWindowMode();
			bool maximize = settings.IsMaximize();
			unsigned int width = settings.GetWidth();
			unsigned int height = settings.GetHeight();
			
			// override하려면 매개변수를 전달하는 게 맞다.
			ModifyWindow(mode, width, height, maximize);
		}

		// 원래 SetWindow에서 호출하던 부분 =============================================
		// Backbuffer의 크기 설정을 늦추기 위해 이 곳으로 옮겼다.
		// 하지만 Wicked의 경우 Renderer::Initialize() 과정의 마지막에 WM_SIZE 이벤트를 받도록 하여
		// ReiszeBuffers()를 호출토록 하였다. 
		auto& renderer = Renderer::GetInstance();
		// 그런데 이렇게 Set을 할 필요가 있나? 이건 멀티플랫폼의 잔재 같은데...
		renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(m_hWnd, m_bFullScreen));
		//===============================================================================

		// 일단 싱글 쓰레드로 간다.
		// 이후 laoding은 asyn로 하고, parallel은 최후로 미룬다.
		// 전부 bool 타입으로 바꾸자
		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
		Input::GetInstance().Initialize(m_hWnd);

		EVENT_SUBSCRIBE(eEventType::ChangedResolution, EVENT_HANDLE_DATA(OnResizeResolution));

		CORE_TRACE("Runtime::Initialize()");

		return true;
	}

	void Runtime::Run()
	{
		if (!m_bInitialized)
		{
			m_bInitialized = Initialize();
			
			if (!m_bInitialized)
			{
				CORE_ERROR("Rumtime 초기화에 실패하였습니다. 프로그램을 종료합니다.");
				PostQuitMessage(0);
			}
		}

		TimeManager::GetInstance().Update();

		auto delta = TimeManager::GetInstance().GetDeltaTimeSec();

		static float fixedFrameAccumulator = 0.0f;
		static float deltaFrameAccumulator = 0.0f;

	
		if (m_bWindowActive)
		{
			fixedFrameAccumulator += delta;
			deltaFrameAccumulator += delta;

			if (fixedFrameAccumulator >= TimeManager::GetInstance().GetFixedFrameRate())
			{
				//CORE_TRACE("FixedUpdate Frame: {:n}", TimeManager::GetInstance().GetFrameCount());

				FixedUpdate();
				fixedFrameAccumulator = 0.0f;
			}

			if (m_bFrameLock)
			{
				if (deltaFrameAccumulator >= static_cast<float>(1.0f / m_targetFPS))
				{
					Update(deltaFrameAccumulator);
					Input::GetInstance().Update();
					Render();

					deltaFrameAccumulator = 0.0f;
				}
			}
			else
			{
				//CORE_TRACE("Unlock Update Frame: {:n}", TimeManager::GetInstance().GetFrameCount());

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
		auto graphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// lua

		if (m_pActivePath)
		{
			m_pActivePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Runtime::Compose()
	{
		if (m_pActivePath)
		{
			m_pActivePath->Compose();
		}
	}

	void Runtime::ActivatePath(RenderPath* path)
	{
		// 뭔가 있다.
		{
			if (m_pActivePath)
			{
				m_pActivePath->Stop();
			}

			if (path != nullptr)
			{
				path->Start();
			}
			m_pActivePath = path;
		}

		CORE_TRACE("Runtime::ActivatePath()");
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		m_hWnd = windowHandle;
		m_bFullScreen = fullScreen;

		// 로그 객체가 생성되지 않았다.
		//CORE_TRACE("Runtime::SetWindow()");
	}

	// 그냥 App용으로 만들고 Editor는 override시키자.
	void Runtime::ModifyWindow(eWindowModes mode, unsigned int width, unsigned height, bool maximize)
	{
		unsigned int posX = 0;
		unsigned int posY = 0;

		if (mode == eWindowModes::FullScreen)
		{
			// 현재 전체 창모드가 되어버린다.
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

	// 전체크기... 즉 Resolution이 변경되는 거다.
	void Runtime::OnResizeResolution(unsigned int data)
	{
		unsigned int width = data & 0xFFFF;
		unsigned int height = (data >> 16) & 0xFFFF;

		CORE_INFO("Editor::OnResizeResolution() : {0:d}x{1:d}", width, height);

		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		if (pGraphicsDevice->IsInitialized())
			pGraphicsDevice->ResizeBuffers(width, height);

		// 이번엔 크기 저장이 좀 이상하다.
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