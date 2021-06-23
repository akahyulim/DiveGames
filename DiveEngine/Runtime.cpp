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

namespace dive
{
	Runtime::Runtime()
	{
		mAssetManager = nullptr;
		mAppTitle = "diveEngine";
	}

	Runtime::~Runtime()
	{
		if (mAssetManager)
		{
			delete mAssetManager;
			mAssetManager = nullptr;
		}
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
		assert(mWindowHandle != 0);

		if (mbInitialized)
		{
			return false;
		}

		Log::Initialize();
		Settings::GetInstance().Initialize(mAppTitle);

		{
			auto& settings = Settings::GetInstance();
			eWindowModes mode = settings.GetWindowMode();
			if (mode == eWindowModes::FullScreen)
				mbFullScreen = true;
			else
				mbFullScreen = false;
			bool maximize = settings.IsMaximize();
			unsigned int width = settings.GetWidth();
			unsigned int height = settings.GetHeight();
			
			ModifyWindow(mode, width, height, maximize);
		}

		// �� �κ��� �Ʒ� Renderer::GetInstance().Initialize()�� ��������.
		auto& renderer = Renderer::GetInstance();
		renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(mWindowHandle, mbFullScreen));

		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
		Input::GetInstance().Initialize(mWindowHandle);

		EVENT_SUBSCRIBE(eEventType::ChangedResolution, EVENT_HANDLE_DATA(OnResizeResolution));

		CORE_TRACE("Runtime::Initialize()");

		return true;
	}

	void Runtime::Run()
	{
		if (!mbInitialized)
		{
			mbInitialized = Initialize();
			
			if (!mbInitialized)
			{
				CORE_ERROR("Rumtime �ʱ�ȭ�� �����Ͽ����ϴ�. ���α׷��� �����մϴ�.");
				PostQuitMessage(0);
			}
		}

		TimeManager::GetInstance().Update();

		auto delta = TimeManager::GetInstance().GetDeltaTimeSec();

		static float fixedFrameAccumulator = 0.0f;
		static float deltaFrameAccumulator = 0.0f;

	
		if (mbActiveWindow)
		{
			fixedFrameAccumulator += delta;
			deltaFrameAccumulator += delta;

			if (fixedFrameAccumulator >= TimeManager::GetInstance().GetFixedFrameRate())
			{
				FixedUpdate();
				fixedFrameAccumulator = 0.0f;
			}

			if (mbFrameLock)
			{
				if (deltaFrameAccumulator >= static_cast<float>(1.0f / mTargetFPS))
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
	}
	
	void Runtime::Update(float deltaTime)
	{
		if (mActivePath)
		{
			mActivePath->PreUpdate();
		}

		// lua

		if (mActivePath)
		{
			mActivePath->Update(deltaTime);
			mActivePath->PostUpdate();
		}
	}
	
	void Runtime::FixedUpdate()
	{
		// lua

		if (mActivePath)
		{
			// ����� �� �ָ��ϴ�.
			// ��ǻ� ȣ�� �Լ��� 3d�� ����� 2d�� FixedUpdate()�̴�.
			// ����Ƽ������ rigid body update�� ���δٰ� �Ѵ�.
			// �Ƹ��� �ִϸ��̼��� �ӵ��� �ϰ����ְ� ����� �����ε� �ϴ�.
			// �׷��� �� ����� Scene�� �������ٵ�...
			mActivePath->FixedUpdate();
		}
	}
	
	void Runtime::Render()
	{
		auto graphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// lua

		if (mActivePath)
		{
			mActivePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Runtime::Compose()
	{
		if (mActivePath)
		{
			mActivePath->Compose();
		}
	}

	void Runtime::ActivatePath(RenderPath* path)
	{
		// ���� �ִ�.
		{
			if (mActivePath)
			{
				mActivePath->Stop();
			}

			if (path != nullptr)
			{
				path->Start();
			}
			mActivePath = path;
		}

		CORE_TRACE("Runtime::ActivatePath()");
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		mWindowHandle = windowHandle;
		mbFullScreen = fullScreen;
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

			SetWindowLong(mWindowHandle, GWL_STYLE, style);

			posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		}

		SetWindowPos(mWindowHandle, NULL, posX, posY, width, height, 0);

		ShowWindow(mWindowHandle, SW_SHOWDEFAULT);
		UpdateWindow(mWindowHandle);
	}

	//==============================================================================//
	// 1. Minimize�� ũ�Ⱑ 0, 0���� ���޵ȴ�.										//
	// �̶� Backbuffer�� RenderTarget�� ũ�⸦ 0, 0���� �����ؾ��ϳ�?				//
	//==============================================================================//
	void Runtime::OnResizeResolution(unsigned int data)
	{
		unsigned int width = data & 0xFFFF;
		unsigned int height = (data >> 16) & 0xFFFF;

		CORE_INFO("Editor::OnResizeResolution() : {0:d}x{1:d}", width, height);

		auto pGraphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		if (pGraphicsDevice->IsInitialized())
			pGraphicsDevice->ResizeBuffers(width, height);

		auto& settings = Settings::GetInstance();
		if (IsZoomed(mWindowHandle))
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