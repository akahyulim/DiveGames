#include "Runtime.h"
#include "Log.h"
#include "Settings.h"
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

		// ������ �����ؾ� �Ѵ�.
		Log::Initialize();
		Settings::GetInstance().Initialize(m_title);

		// �ᱹ �̰� �ɼ� ��� ������ �� ��Ȱ���� �� �ְ� ������ �Ѵ�.
		// ��, ���� ������ �񱳰� �ʿ��� �� �ִ�.
		{
			unsigned int width = Settings::GetInstance().GetWidth();
			unsigned int height = Settings::GetInstance().GetHeight();
			unsigned int posX = 0;
			unsigned int posY = 0;
			bool bFullScreen = Settings::GetInstance().IsFullScreen();
			if (bFullScreen)
			{
				// ���� ��ü â��尡 �Ǿ������.
				width = GetSystemMetrics(SM_CXSCREEN);
				height = GetSystemMetrics(SM_CYSCREEN);

				DEVMODE dmScreenSettings;
				memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
				dmScreenSettings.dmSize = sizeof(dmScreenSettings);
				dmScreenSettings.dmPelsWidth = (unsigned long)width;
				dmScreenSettings.dmPelsHeight = (unsigned long)height;
				dmScreenSettings.dmBitsPerPel = 32;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
			}
			else
			{
				bool bBorderless = Settings::GetInstance().IsBorderlessWindow();
				auto style = GetWindowLong(m_hWnd, GWL_STYLE);
				if (bBorderless)
				{
					if (style != WS_POPUP)
					{
						SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
					}
				}
				else
				{
					// maximize�� ���⼭�� ����ȴ�.
					if (style != WS_OVERLAPPEDWINDOW)
					{

						SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

						RECT rt = { 0, 0, (LONG)width, (LONG)height };
						AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, 0);

						width = rt.right - rt.left;
						height = rt.bottom - rt.top;
					}
				}

				posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
				posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
			}

			// ���������� ������ ũ�⸦ ������ �� Show
			// ���� ũ��� ������ Ȯ���ؾ� �Ѵ�. => �̰� �ϴ� �̷���.
			SetWindowPos(m_hWnd, NULL, posX, posY, width, height, 0);

			ShowWindow(m_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(m_hWnd);

		}

		// ���� SetWindow���� ȣ���ϴ� �κ� =============================================
		// Backbuffer�� ũ�� ������ ���߱� ���� �� ������ �Ű��.
		// ������ Wicked�� ��� Renderer::Initialize() ������ �������� WM_SIZE �̺�Ʈ�� �޵��� �Ͽ�
		// ReiszeBuffers()�� ȣ����� �Ͽ���. 
		auto& renderer = Renderer::GetInstance();
		// �׷��� �̷��� Set�� �� �ʿ䰡 �ֳ�? �̰� ��Ƽ�÷����� ���� ������...
		renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(m_hWnd, m_bFullScreen));
		//===============================================================================

		// �ϴ� �̱� ������� ����.
		// ���� laoding�� asyn�� �ϰ�, parallel�� ���ķ� �̷��.
		// ���� bool Ÿ������ �ٲ���
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
				CORE_ERROR("Rumtime �ʱ�ȭ�� �����Ͽ����ϴ�. ���α׷��� �����մϴ�.");
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
			// �Է� ���۸� ����
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
		// ���� �ִ�.
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

		// �α� ��ü�� �������� �ʾҴ�.
		//CORE_TRACE("Runtime::SetWindow()");
	}

	// ��üũ��... �� Resolution�� ����Ǵ� �Ŵ�.
	void Runtime::OnResizeResolution(unsigned int data)
	{
		unsigned int width = data & 0xFFFF;
		unsigned int height = (data >> 16) & 0xFFFF;

		APP_INFO("Editor::OnResizeResolution() : {0:d}x{1:d}", width, height);

		auto pGraphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		if (pGraphicsDevice->IsInitialized())
			pGraphicsDevice->ResizeBuffers(width, height);

		// �ϴ� �� �κ��� Setting�� ���̵Ǿ�� �Ѵ�.
		// �׸��� �Ź� �����ϴ� �� ����
		// �� �����ؾ��� ������ Ȯ���� �ʿ��ϹǷ� �Լ�ȭ�ϴ� �� �´�.
		// �׷��� ������ ��Ұ� ���� �ý��ۿ� �����ִٴ°� ������.
		IniHelper ini(m_iniFilePath);
		if (IsZoomed(m_hWnd))
		{
			ini["Window"]["bMaximize"] = true;
		}
		else
		{
			ini["Window"]["bMaximize"] = false;
			ini["Window"]["Width"] = width;
			ini["Window"]["Height"] = height;
		}
	}
}