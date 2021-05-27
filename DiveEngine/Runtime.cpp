#include "Runtime.h"
#include "Log.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Renderer.h"
#include "Input.h"
#include <memory>

using namespace std;

namespace Dive
{
	Runtime::Runtime()
	{
		Log::Initialize();
	}

	bool Runtime::Initialize()
	{
		assert(m_hWnd != 0);

		if (m_bInitialized)
		{
			return false;
		}
		
		CORE_TRACE("Dive Engine 초기화를 시작합니다...");

		// 원래 SetWindow에서 호출하던 부분 =============================================
		// Backbuffer의 크기 설정을 늦추기 위해 이 곳으로 옮겼다.
		// 하지만 Wicked의 경우 Renderer::Initialize() 과정의 마지막에 WM_SIZE 이벤트를 받도록 하여
		// ReiszeBuffers()를 호출토록 하였다. 
		auto& renderer = Renderer::GetInstance();
		renderer.SetGraphicsDevice(make_shared<GraphicsDevice>(m_hWnd, m_bFullScreen));
		//===============================================================================

		// 일단 싱글 쓰레드로 간다.
		// 이후 laoding은 asyn로 하고, parallel은 최후로 미룬다.
		// 전부 bool 타입으로 바꾸자
		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
		Input::GetInstance().Initialize(m_hWnd);

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
				CORE_TRACE("FixedUpdate Frame: {:n}", TimeManager::GetInstance().GetFrameCount());

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
				CORE_TRACE("Unlock Update Frame: {:n}", TimeManager::GetInstance().GetFrameCount());

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
		if (m_activePath)
		{
			m_activePath->PreUpdate();
		}

		// lua

		if (m_activePath)
		{
			m_activePath->Update(deltaTime);
			m_activePath->PostUpdate();
		}
	}
	
	void Runtime::FixedUpdate()
	{
		// lua

		if (m_activePath)
		{
			// 대상이 좀 애매하다.
			// 사실상 호출 함수는 3d가 상속한 2d의 FixedUpdate()이다.
			// 유니티에서는 rigid body update에 쓰인다고 한다.
			// 아마도 애니메이션의 속도를 일관성있게 만들기 위함인듯 하다.
			// 그런데 이 대상은 Scene에 존재할텐데...
			m_activePath->FixedUpdate();
		}
	}
	
	void Runtime::Render()
	{
		auto graphicsDevice = Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// lua

		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Runtime::Compose()
	{
		if (m_activePath)
		{
			m_activePath->Compose();
		}
	}

	void Runtime::ActivatePath(RenderPath* path)
	{
		// 뭔가 있다.
		{
			if (m_activePath)
			{
				m_activePath->Stop();
			}

			if (path != nullptr)
			{
				path->Start();
			}
			m_activePath = path;
		}
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		m_hWnd = windowHandle;
		m_bFullScreen = fullScreen;
	}
}