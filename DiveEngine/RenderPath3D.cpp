#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
//#include "ThreadPool.h"

namespace Dive
{
	void RenderPath3D::PreUpdate()
	{
	}

	void RenderPath3D::Update(float deltaTime)
	{
		// 2d update

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_scene->Update(deltaTime * timeScale);
		}

		// visibility 생성 및 초기화 그리고 설정
		// renderer::updatevisibility로 마저 설정

		// renderer::updatePerFrameData: 매 프레임 업데이트 되는 데이터???
		// 여기에서 데이터를 얻어온 후 다른 곳에서 buffer를 만들어 전달하는 것 같다.

		// camera update
		
	}
	
	void RenderPath3D::Render() const
	{
		// device를 직접 사용하는 건 좀 에바 같다.
		// 그런데 bind를 위해선 어쩔수 없는듯?
		auto device = Renderer::GetInstance().GetDevice();

	}

	void RenderPath3D::Compose() const
	{
		// 결과물을 텍스쳐에 그리는 것 같다.
	}
}