#include "RenderPathEditor.h"

namespace editor
{
	RenderPathEditor::RenderPathEditor()
	{
	}

	RenderPathEditor::~RenderPathEditor()
	{
	}
	
	void RenderPathEditor::PreUpdate()
	{
		if (!m_pScene)
			return;

		// ī�޶� ����?
	}
	
	void RenderPathEditor::Update(float deltaTime)
	{
		if (!m_pScene)
			return;

		// Scene Update
		{
			auto timeScale = dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		/*
		* �� �κп� UpdateVisibility�� ����.
		* �̴� Spartanó�� GameObject Container�� �ܼ��� Update�ϰ� �з��ϴ� �� �ܿ���
		* Camera�� ���� Culling�ϴ� ������ ����ȴ�.
		* �� ���� Ư���� ���� �̸� �����ϴ� ���� RenderPath������
		* �����ϴ� ���� Renderer��� ���̴�.
		*/

		// Frame Data Update
		{
			dive::Renderer::GetInstance().UpdateCB();
		}
	}
	
	/*
	* Wicked�� RenderPass��� ���� ����� ����ϴµ�... ���� �𸣰ڴ�.
	*/
	void RenderPathEditor::Render() const
	{
		if (!m_pScene)
			return;

		auto pRenderer = &dive::Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		/*
		* ù ��° Pass
		* ������� �м��� �ٿ� ������ 
		* ConstantBuffer Update, Viewport & RenderTarget Bind ��
		* Draw Call�� Visibility�� �����ϴ� ��缼��.
		* �̶� RenderPass��� ���� ����ϱ⵵ �ϴµ� ���� ���� �𸣰ڴ�.
		*/
		{
			// �� RenderTarget Texture�� RenderPath���� ����� �����ϴ°� �´�.
			auto pRTV = pRenderer->GetFrameTexture()->GetRenderTargetView();
			pImmediateContext->OMSetRenderTargets(1, &pRTV, nullptr);

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

			pImmediateContext->ClearRenderTargetView(pRTV, clearColors);

			pRenderer->PassMultiCamTest(pRTV);
		}
	}
	
	void RenderPathEditor::Compose() const
	{
		// �� �Լ��� Backbuffer�� RTV Set & Clean�� Present ���̿� ȣ��ȴ�.
		// ���� ���⿡�� RenderTexture���� Backbuffer�� �׷��� �Ѵ�.
		
		//======================================================================================//
		// ����� ���� RenderTexture�� �ϳ��� ȭ���� �׸� ������� Present �ǰ� �ִ�.			//
		// ��� �� �������� ����ó���� �� �ʿ��� �� ������... ���� �� �̻���� ���ư��� �ִ�.	//
		// ���� �� �̸� ���캸�� �ذ��ؾ� �Ѵ�.													//
		//======================================================================================//
	}
}