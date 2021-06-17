#include "Runtime.h"
#include "Log.h"
#include "Settings.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Renderer.h"
#include "Input.h"
#include "AssetManager.h"
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

		// ���Ⱑ �´�.
		Log::Initialize();


		// ���� SetWindow���� ȣ���ϴ� �κ� =============================================
		// Backbuffer�� ũ�� ������ ���߱� ���� �� ������ �Ű��.
		// ������ Wicked�� ��� Renderer::Initialize() ������ �������� WM_SIZE �̺�Ʈ�� �޵��� �Ͽ�
		// ReiszeBuffers()�� ȣ����� �Ͽ���. 
		auto& renderer = Renderer::GetInstance();
		renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(m_hWnd, m_bFullScreen));
		//===============================================================================

		// �ϴ� �̱� ������� ����.
		// ���� laoding�� asyn�� �ϰ�, parallel�� ���ķ� �̷��.
		// ���� bool Ÿ������ �ٲ���
		Settings::GetInstance().Initialize(m_title);
		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
		Input::GetInstance().Initialize(m_hWnd);

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
}