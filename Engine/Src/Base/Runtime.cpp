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
	// 1. �»������� ������ �ΰ��� �� �ۿ� ����.												//
	// 2. ���� Graphics Device Setting�� ���� �߰� ���� ��ü�� �ʿ��� �� �ִ�.					//
	// 3. Multi-Thread�� �������� �ʾҴ�.														//
	// 4. Renderer�� Graphics�� ���� ������ �����ؾ� �Ѵ�.										//
	// ������ �������̽��� ��Ƽ �÷��� ������ ������ �� ���̴�.									//
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

		// �� �κ��� �Ʒ� Renderer::GetInstance().Initialize()�� ��������.
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
				CORE_ERROR("Rumtime �ʱ�ȭ�� �����Ͽ����ϴ�. ���α׷��� �����մϴ�.");
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
			// �Է� ���۸� ����
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
			// ����� �� �ָ��ϴ�.
			// ��ǻ� ȣ�� �Լ��� 3d�� ����� 2d�� FixedUpdate()�̴�.
			// ����Ƽ������ rigid body update�� ���δٰ� �Ѵ�.
			// �Ƹ��� �ִϸ��̼��� �ӵ��� �ϰ����ְ� ����� �����ε� �ϴ�.
			// �׷��� �� ����� Scene�� �������ٵ�...
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
		// ���� �ִ�.
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
	// 1. Virtual�� �����ߴ�. Editor�� ���� Windowed�� �ʿ��ϱ� �����̴�.								//
	// 2. FullScreen�� Size�� ũ�Ⱑ �ƴ϶� �ػ󵵴�. �̴� ���� �� �� �����غ���.						//
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
	// 1. Minimize�� ũ�Ⱑ 0, 0���� ���޵ȴ�.										//
	// �̶� Backbuffer�� RenderTarget�� ũ�⸦ 0, 0���� �����ؾ��ϳ�?				//
	//==============================================================================//
	void Runtime::OnResizeResolution(unsigned int data)
	{
		// �̰� Ŭ���̾�Ʈ ���� ũ���ΰ�?
		unsigned int width = data & 0xFFFF;
		unsigned int height = (data >> 16) & 0xFFFF;

		CORE_INFO("Editor::OnResizeResolution() : {0:d}x{1:d}", width, height);

		// �̷��� ���� ���� �̺�Ʈ�� �޵��� �ϴ� ���� ���� ������?
		// GraphicsDevice�� ���� �����ϴ°� �������ϴ�.
		Renderer::GetInstance().GetGraphicsDevice()->SetResolution(width, height);

		// �̺�Ʈ�� ���� �޴´ٸ� �� �κ��� ��� �� ���ΰ�?
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