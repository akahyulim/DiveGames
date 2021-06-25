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
		// ī�޶� ����?
	}
	
	void RenderPathEditor::Update(float deltaTime)
	{
		// ������Ʈ �ʿ� ���ο� ���� ����
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			mScene->Update(deltaTime * timeScale);
		}
	}
	
	//================================================================================================
	// �ϴ� Renderer�� ������ Pass�� �������� ���� ������� �׸��� �Լ���.
	// Editor�� ��� Backbuffer�� RenderTargetView�� ImGui������ ����ؾ� �ϹǷ�,
	// Scene Widget�� �׸� �ؽ��Ĵ� Pass_Ldr�̶�� RenderTargetView�� RenderTarget���� ��� �ִ�.
	//================================================================================================
	void RenderPathEditor::Render() const
	{
		auto pRenderer = &dive::Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		// ClearColor�� ��𿡼� �����;� �ұ�?
		// �ƿ� GraphicsDevice�� RenderTarget�� �����ϸ� �Ǵ� �Լ��� ����°� ������?
		// ���ĸ�ź�� ��� Pass ���ο� ���ԵǾ� �ִ�. �׷��� �̴� Runtime�� Editor�� ���յ��� ���� ���ĸ�ź�� Ư¡�����ε� �ϴ�.
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
		// ����...
	}
}