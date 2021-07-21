#include "RenderPathEditor.h"

namespace editor
{
	RenderPathEditor::RenderPathEditor()
	{
		//m_pScene = &dive::Scene::GetGlobalScene();
		// ���ŵǴ°� �����Ϸ��� �� ���� �̺�Ʈ�� ���� ������?
		//m_pScene = dive::SceneManager::GetInstance().GetActiveScene();
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
		if (!m_pScene)
			return;

		// ������Ʈ �ʿ� ���ο� ���� ����
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}
	}
	
	//================================================================================================
	// �ϴ� Renderer�� ������ Pass�� �������� ���� ������� �׸��� �Լ���.
	// Editor�� ��� Backbuffer�� RenderTargetView�� ImGui������ ����ؾ� �ϹǷ�,
	// Scene Widget�� �׸� �ؽ��Ĵ� Pass_Ldr�̶�� RenderTargetView�� RenderTarget���� ��� �ִ�.
	//================================================================================================
	void RenderPathEditor::Render() const
	{
		// ��� Scene�� ��� ������ �׳� �׸� �� �ִ�.
		// �ٸ� Scene�� ���ٴ� ����� �˸��� �;� �� �κ��� �߰��ϴ� �� ���� �� ����.
		if (!m_pScene)
			return;

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