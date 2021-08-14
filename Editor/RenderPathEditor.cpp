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

		// 2d update
		CORE_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// visibility ���� �� �ʱ�ȭ �׸��� ����
		// �׷��� �Ź� �� �ʿ䰡 ����. �̺�Ʈ�� ó���ϴ°� ����.
		CORE_TRACE("RenderPath3D::Update() - Visibility ����");

		// renderer::updatePerFrameData: �� ������ ������Ʈ �Ǵ� ������???
		// ���⿡�� �����͸� ���� �� �ٸ� ������ buffer�� ����� �����ϴ� �� ����.
		dive::Renderer::GetInstance().UpdateCB();
		CORE_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		// �̰͵� ���� �������� ��︮�� �ʴ� �� ����.
		CORE_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	/*
	* Renderer�� ������ Pass�� �����Ѵ�. 
	* Deferred Shading �������� ũ�� �� ���� Pass�� �����ȴ�.
	* ù ��°. GBuffer�� �׸���.
	* �� ��°. GBuffer�� ������ Lighting�� RenderTarget Ȥ�� Backbuffer�� �׸���.
	*/
	void RenderPathEditor::Render() const
	{
		// ��� Scene�� ��� ������ �׳� �׸� �� �ִ�.
		// �ٸ� Scene�� ���ٴ� ����� �˸��� �;� �� �κ��� �߰��ϴ� �� ���� �� ����.
		if (!m_pScene)
			return;

		// ������ ��ü���� �󸶳� �����ͼ� ����� �ų��̴�.
		// Renderer�� ������ Pass�����δ� ������ �� �ֱ� �����̴�.
		// ������ �ʹ� �����ϰ� �����ͼ� ����ϸ� ������ ������ �� �� �ִ�.
		// Tick�� Render�� �����ؾ� �Ѵ�. �׷��� ������ ���� �ȴ�.

		auto pRenderer = &dive::Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		// �� �κ� ������ Editor�� RenderPath�� �ʿ��ߴ�.
		// Editor�� Scene�� RenderTarget�� �׷��� �ϱ� �����̴�.
		// ���� �� RenderTarget�� �� RenderPath���� ���� �����Ͽ� ����ϰ� �ʹ�.
		// => ������ Renderer�� �����Լ��� ���� �����Ѵ� �ص�
		// => Resize�� ������...
		auto pRtv = pRenderer->GetFrameTexture()->GetRenderTargetView();
		pImmediateContext->OMSetRenderTargets(1, &pRtv, nullptr);

		// ���ο� Scene�� ���� ���� ī�޶� ���� ���ϳ�...
		// �׷��� ���� Camera�� �����Ϸ��� Camera�� Pass�� ����Ǿ�� �Ѵ�.
		// ���� �̷��� ClearColor ������ �ǹ̰� ����???
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

		// ���⿡�� Camera�� ClearFlags�� SolidColor, Depth, Nothing�� ����ȴ�. Cubemap�� ���� �𸣰ڴ�.
		// �̶� �ʿ��� ���� �ٷ� RenderTargetView�� DepthStencilView�̴�.
		// �̰� �ƿ� Camera�� �������� �ϴ°� �����ϱ�?
		// MeshRenderer�� �ڽ��� Draw�ϴµ�...
		pImmediateContext->ClearRenderTargetView(pRtv, clearColors);
		

		// pass 
		//dive::Renderer::GetInstance().DrawLegacy();
		dive::Renderer::GetInstance().PassMultiCamTest(pRtv);
	}
	
	void RenderPathEditor::Compose() const
	{
		// ����...
	}
}