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
	
	// 현재 Sandbox를 비활성화 시켰다.
	// 추후 Editor의 RenderPath를 참고하여 구현하자.
	void RenderPath3D::Render() const
	{
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget 설정");
	}
}