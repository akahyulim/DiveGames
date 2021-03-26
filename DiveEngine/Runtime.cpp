#include "Runtime.h"
#include "Log.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Renderer.h"
#include <memory>

using namespace std;

namespace Dive
{
	Runtime::Runtime()
	{
		Log::Initialize();
	}

	void Runtime::Initialize()
	{
		if (m_bInitialized)
		{
			return;
		}
		m_bInitialized = true;
		
		CORE_TRACE("Dive Engine 초기화를 시작합니다...");

		// 일단 싱글 쓰레드로 간다.
		// 이후 laoding은 asyn로 하고, parallel은 최후로 미룬다.
		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
	}

	void Runtime::Run()
	{
		if (!m_bInitialized)
		{
			Initialize();
			
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
					// 입력 버퍼
					Render();

					deltaFrameAccumulator = 0.0f;
				}
			}
			else
			{
				CORE_TRACE("Unlock Update Frame: {:n}", TimeManager::GetInstance().GetFrameCount());

				Update(delta);
				// 입력 버퍼
				Render();

				deltaFrameAccumulator = 0.0f;
			}
		}
		else
		{
			// 입력 버퍼만 수행
		}

		// device의 직접 호출은 에바다.
		// 그리고 위치도 Render()로 옮겨야 한다.
		// 문제는 Compose()이다.
		auto device = Renderer::GetInstance().GetDevice();
		device->PresentBegin();
		Compose();
		device->PresentEnd();
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
		// lua

		if (m_activePath)
		{
			m_activePath->Render();
		}
	}

	void Runtime::Compose()
	{
		if (m_activePath)
		{
			m_activePath->Compose();
		}
	}

	void Runtime::SetActivePath(RenderPath* path)
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
		auto& renderer = Renderer::GetInstance();
		renderer.SetDevice(make_shared<GraphicsDevice>(windowHandle, fullScreen));

		auto device = renderer.GetDevice();
		CORE_TRACE("Resolution: {0:d} x {1:d}", device->GetResolutionWidth(), device->GetResolutionHeight());

		RECT rt;
		GetClientRect(windowHandle, &rt);
		auto width = static_cast<unsigned int>(rt.right - rt.left);
		auto height = static_cast<unsigned int>(rt.bottom - rt.top);
		CORE_TRACE("Changed ClientRect: {0:d} x {1:d}", width, height);

		device->SetResolution(800, 600);
		CORE_TRACE("Changed Resolution: {0:d} x {1:d}", device->GetResolutionWidth(), device->GetResolutionHeight());
	}
}