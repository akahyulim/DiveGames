#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
#include "Log.h"

/*
* 다시 잘 살려보자.
*/
namespace dive
{
	/*
	* ResizeBuffer
	* 사실상 추가 Texture들을 포함한 추가 리소들을 생성하는 함수다.
	* 크기는 GraphicsDevice의 Resolution을 기반으로 한다.
	* 갱신 주기는 RenderPath2D의 Load, Update 등이며, WM_SIZE 이벤트도 수신하는 것 같다.
	*/

	void RenderPath3D::PreUpdate()
	{
		CORE_TRACE("RenderPath3D::PreUpdate() - Previous Camera 저장");
	}

	// 그리기전 준비를 한다.
	void RenderPath3D::Update(float deltaTime)
	{
		if (!m_pScene)
			return;

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

		// Eidtor에선 Backbuffer와 RenderTarget의 크기가 다르므로
		// Sandbox에선 이렇게 Backbuffer 크기로 설정을 해 주어야 한다.
		float width = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionWidth();
		float height = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionHeight();
		Renderer::GetInstance().SetViewport(width, height);
		
		Renderer::GetInstance().UpdateCB();
		//Renderer::GetInstance().DrawColor();
		//Renderer::GetInstance().DrawTexturing();
		Renderer::GetInstance().DrawLegacy();

		// TextMesh와 GameObject 구성 후 다시 테스트
		//Renderer::GetInstance().DrawText();
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget 설정");
	}
}