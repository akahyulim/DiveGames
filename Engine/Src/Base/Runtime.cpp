#include "divepch.h"
#include "Runtime.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Event.h"
#include "../Renderer/Renderer.h"
#include "../Resource/ResourceManager.h"
#include "../Scene/SceneManager.h"
#include "../Input/Input.h"
#include "IniHelper.h"
#include "Timer.h"
#include "Log.h"


using namespace std;

namespace Dive
{
	Runtime::Runtime()
	{
		m_AppTitle = "diveEngine";
	}

	Runtime::~Runtime()
	{
	}

	//==========================================================================================//
	// 1. 태생적으로 순서에 민감할 수 밖에 없다.												//
	// 2. 추후 Graphics Device Setting와 같이 추가 설정 객체가 필요할 수 있다.					//
	// 3. Multi-Thread를 적용하지 않았다.														//
	// 4. Renderer와 Graphics의 생성 과정을 수정해야 한다.										//
	// 참고한 인터페이스는 멀티 플랫폼 지원을 염두해 둔 것이다.									//
	//==========================================================================================//
	bool Runtime::Initialize()
	{
		assert(m_hWnd != 0);

		if (m_bInitialized)
		{
			return false;
		}

		Log::Initialize();
		Settings::GetInstance().Initialize(m_AppTitle);

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

		// 이 부분을 아래 Renderer::GetInstance().Initialize()에 통합하자.
		//auto& renderer = Renderer::GetInstance();
		//renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(m_hWnd, m_bFullScreen));

		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize(m_hWnd, m_bFullScreen);
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

	
		if (m_bActiveWindow)
		{
			fixedFrameAccumulator += delta;
			deltaFrameAccumulator += delta;

			if (fixedFrameAccumulator >= TimeManager::GetInstance().GetFixedFrameRate())
			{
				FixedUpdate();
				fixedFrameAccumulator = 0.0f;
			}

			if (m_bFrameLock)
			{
				if (deltaFrameAccumulator >= static_cast<float>(1.0f / m_TargetFPS))
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

		CORE_TRACE("Runtime::ActivatePath()");
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		m_hWnd = windowHandle;
		m_bFullScreen = fullScreen;
	}

	//==================================================================================================//
	// 1. Virtual로 구현했다. Editor는 오직 Windowed만 필요하기 때문이다.								//
	// 2. FullScreen의 Size는 크기가 아니라 해상도다. 이는 추후 좀 더 생각해보자.						//
	//==================================================================================================//										
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