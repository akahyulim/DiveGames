#include "RenderPathEditor.h"

namespace editor
{
	RenderPathEditor::RenderPathEditor()
	{
		mScene = &dive::Scene::GetGlobalScene();
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
		// 업데이트 필요 여부에 따라 선택
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			mScene->Update(deltaTime * timeScale);
		}
	}
	
	//================================================================================================
	// 일단 Renderer에 구성한 Pass의 조합으로 최종 결과물을 그리는 함수다.
	// Editor의 경우 Backbuffer의 RenderTargetView는 ImGui용으로 사용해야 하므로,
	// Scene Widget에 그릴 텍스쳐는 Pass_Ldr이라는 RenderTargetView를 RenderTarget으로 삼고 있다.
	//================================================================================================
	void RenderPathEditor::Render() const
	{
		auto pRenderer = &dive::Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		// ClearColor는 어디에서 가져와야 할까?
		// 아에 GraphicsDevice에 RenderTarget만 전달하면 되는 함수를 만드는게 나을까?
		// 스파르탄의 경우 Pass 내부에 포함되어 있다. 그런데 이는 Runtime과 Editor의 결합도가 높은 스파르탄의 특징때문인듯 하다.
		auto pRtv = pRenderer->GetFrameTexture()->GetRenderTargetView();
		pImmediateContext->OMSetRenderTargets(1, &pRtv, nullptr);
		float clearColors[4] = { 0.5f, 0.5f, 0.75f, 1.0f };
		pImmediateContext->ClearRenderTargetView(pRtv, clearColors);

		dive::Renderer::GetInstance().UpdateCB();
		//dive::Renderer::GetInstance().DrawColor();
		dive::Renderer::GetInstance().DrawLegacy();
	}
	
	void RenderPathEditor::Compose() const
	{
		// 흐음...
	}
}