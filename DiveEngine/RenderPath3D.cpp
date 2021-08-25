#include "RenderPath3D.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
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
		if (!m_pScene)	return;

		CORE_TRACE("RenderPath3D::PreUpdate() - Previous Camera 저장");

		// GameObject PreUpdate를 수행할 수 있다.
	}

	// 그리기전 준비를 한다.
	void RenderPath3D::Update(float deltaTime)
	{
		if (!m_pScene)	return;

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// 매 프레임 업데이트 되는 데이터를 전달
		{

		}
	}
	
	// 현재 Sandbox가 이를 사용하고 있다.
	// 추후 수정 구현이 필요하다.
	void RenderPath3D::Render() const
	{
		if (!m_pScene)	return;

		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// RenderPassBegin
		{
			// 생성 및 패키징된 RenderTarget들을 OMSet & Clear
		}

		// 이게 아니라 RenderTargetView로부터 크기를 가져와야 한다.
		float width = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionWidth();
		float height = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionHeight();
		Renderer::GetInstance().SetViewport(width, height);
		
		Renderer::GetInstance().UpdateCB();
		Renderer::GetInstance().DrawLegacy();

		// RenderPassEnd
		{
			// OMSet을 nullptr로 초기화
		}
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget 설정");
	}
}