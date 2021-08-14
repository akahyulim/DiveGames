#include "RenderPathEditor.h"

namespace editor
{
	RenderPathEditor::RenderPathEditor()
	{
		//m_pScene = &dive::Scene::GetGlobalScene();
		// 갱신되는걸 염두하려면 이 역시 이벤트가 맞지 않을까?
		//m_pScene = dive::SceneManager::GetInstance().GetActiveScene();
	}

	RenderPathEditor::~RenderPathEditor()
	{
	}
	
	void RenderPathEditor::PreUpdate()
	{
		// 카메라 설정?
	}
	
	void RenderPathEditor::Update(float deltaTime)
	{
		if (!m_pScene)
			return;

		// 2d update
		CORE_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// visibility 생성 및 초기화 그리고 설정
		// 그런데 매번 할 필요가 없다. 이벤트로 처리하는게 낫다.
		CORE_TRACE("RenderPath3D::Update() - Visibility 구성");

		// renderer::updatePerFrameData: 매 프레임 업데이트 되는 데이터???
		// 여기에서 데이터를 얻어온 후 다른 곳에서 buffer를 만들어 전달하는 것 같다.
		dive::Renderer::GetInstance().UpdateCB();
		CORE_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		// 이것도 현재 구현과는 어울리지 않는 것 같다.
		CORE_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	/*
	* Renderer에 구성한 Pass로 구성한다. 
	* Deferred Shading 기준으로 크게 두 개의 Pass로 구성된다.
	* 첫 번째. GBuffer에 그린다.
	* 두 번째. GBuffer를 참조해 Lighting을 RenderTarget 혹은 Backbuffer에 그린다.
	*/
	void RenderPathEditor::Render() const
	{
		// 사실 Scene이 없어도 배경색은 그냥 그릴 수 있다.
		// 다만 Scene이 없다는 사실을 알리고 싶어 이 부분을 추가하는 게 나을 것 같다.
		if (!m_pScene)
			return;

		// 문제는 객체들을 얼마나 가져와서 사용할 거냐이다.
		// Renderer에 구현된 Pass만으로는 부족할 수 있기 때문이다.
		// 하지만 너무 난잡하게 가져와서 사용하면 구현이 복잡해 질 수 있다.
		// Tick과 Render를 구분해야 한다. 그렇지 않으면 헬이 된다.

		auto pRenderer = &dive::Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		// 이 부분 때문에 Editor용 RenderPath가 필요했다.
		// Editor는 Scene을 RenderTarget에 그려야 하기 때문이다.
		// 나는 이 RenderTarget을 이 RenderPath에서 직접 생성하여 사용하고 싶다.
		// => 생성은 Renderer에 생성함수를 만들어서 적용한다 해도
		// => Resize가 문제다...
		auto pRtv = pRenderer->GetFrameTexture()->GetRenderTargetView();
		pImmediateContext->OMSetRenderTargets(1, &pRtv, nullptr);

		// 새로운 Scene을 만들어도 이전 카메라를 놓지 못하네...
		// 그런데 다중 Camera를 지원하려면 Camera별 Pass가 진행되어야 한다.
		// 따라서 이러한 ClearColor 구현은 의미가 없다???
		auto pCamera = pRenderer->GetCamera();
		float clearColors[4];
		if (!pCamera)
		{
			clearColors[0] = 0.0f;
			clearColors[1] = 0.0f;
			clearColors[2] = 0.0f;
			clearColors[3] = 1.0f;
		}
		else
		{
			auto color = pCamera->GetComponent<dive::Camera>()->GetBackgroundColor();
			clearColors[0] = color.x;
			clearColors[1] = color.y;
			clearColors[2] = color.z;
			clearColors[3] = color.w;
		}

		// 여기에서 Camera의 ClearFlags의 SolidColor, Depth, Nothing이 적용된다. Cubemap은 아직 모르겠다.
		// 이때 필요한 것이 바로 RenderTargetView와 DepthStencilView이다.
		// 이걸 아에 Camera가 가지도록 하는건 에바일까?
		// MeshRenderer도 자신이 Draw하는데...
		pImmediateContext->ClearRenderTargetView(pRtv, clearColors);
		

		// pass 
		//dive::Renderer::GetInstance().DrawLegacy();
		dive::Renderer::GetInstance().PassMultiCamTest(pRtv);
	}
	
	void RenderPathEditor::Compose() const
	{
		// 흐음...
	}
}