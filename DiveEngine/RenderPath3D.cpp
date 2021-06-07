#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
//#include "ThreadPool.h"
#include "Log.h"

namespace Dive
{
	void RenderPath3D::PreUpdate()
	{
		CORE_TRACE("RenderPath3D::PreUpdate() - Previous Camera 저장");
	}

	// 그리기전 준비를 한다.
	void RenderPath3D::Update(float deltaTime)
	{
		// 2d update
		CORE_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// visibility 생성 및 초기화 그리고 설정
		// renderer::updatevisibility로 마저 설정
		CORE_TRACE("RenderPath3D::Update() - Visibility 구성");

		// renderer::updatePerFrameData: 매 프레임 업데이트 되는 데이터???
		// 여기에서 데이터를 얻어온 후 다른 곳에서 buffer를 만들어 전달하는 것 같다.
		CORE_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		CORE_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	// Renderer와 어떻게 나누느냐...
	void RenderPath3D::Render() const
	{
		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		Renderer::GetInstance().UpdateCB();
		Renderer::GetInstance().DrawColor();
		//Renderer::GetInstance().DrawTexturing();
		
		// TextMesh와 GameObject 구성 후 다시 테스트
		//Renderer::GetInstance().DrawText();
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget 설정");
	}
}