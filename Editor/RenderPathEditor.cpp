#include "RenderPathEditor.h"
using namespace DiveEngine;

namespace DiveEditor
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
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
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
			Renderer::GetInstance().UpdateCB();
		}
	}
	
	// WickedEngine�� ���� ����
	// 1. GBuffer�� RenderPath�� �����Ѵ�.
	//	- ���� ���� �� ũ�⸦ �����Ѵ�.
	//	- ���� �� RenderPass�� ���� GBufferó�� RenderTarget�� ���´�.
	// 2. Pass�� ������ �׻� RenderPassBegin. ���� RenderPassEnd
	//	- RenderPass�� �����Ͽ� OMSet �� Clear�� �����Ѵ�.
	//	- OMSet�� nullptr�� �����ش�.
	// 3. Viewport�� RenderPass�� �����ϴ� RenderTargetView�� ũ��� ����
	// 4. DrawXXX() �Լ��� Visibility ���� �����Ͽ� Draw
	//	- DrawXXX() ���ο��� �� �������� ó���� �� �� Draw
	void RenderPathEditor::Render() const
	{
		if (!m_pScene)
			return;

		auto pRenderer = &Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		{
			// �� RenderTarget Texture�� RenderPath���� ����� �����ϴ°� �´�.
			// ��... �´ٰ� �Ѱ���...? ����� �׸��� �ؽ��ķ� ������ ���ϱ�?
			auto pRTV = pRenderer->GetRenderTarget()->GetColorRenderTargetView();//GetFrameTexture()->GetRenderTargetView();

			// RenerPassBegin
			{
				//pImmediateContext->OMSetRenderTargets(1, &pRTV, nullptr);
				// �̰� ���� Current Camera�� Update() ��� �������� ���� ������
				// ��� RenderTexture�� ���� Camera�� ���� �ʿ�� ���ٴ� �ǹ�?
				// �׷��ٸ� �Ʒ� ClearRenderTargetView()�� RenderTexture�� ó���� �� �־�� �ϴµ�? 
				// ã�ƺ��� �̰� �����ϴ� �������� �ǹ��� ���.
				dvRenderTexture::SetActive(pRenderer->GetRenderTarget());

				// ��� GBuffer��� Ư�� ������ �ʱ�ȭ�ؾ� �� �Ŵ�.
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
					auto color = pCamera->GetComponent<Camera>()->GetBackgroundColor();
					clearColors[0] = color.x;
					clearColors[1] = color.y;
					clearColors[2] = color.z;
					clearColors[3] = color.w;
				}

				// �̰� �Ƹ��� Camera�� Update() ��� ������ �� ����.
				pImmediateContext->ClearRenderTargetView(pRTV, clearColors);
			}

			// Wicked�� ���� RenderTarget���κ��� ũ�⸦ ������
			// Viewport�� Bind�Ѵ�.
			// ������ ���� ������ Camera�� ���� Viewport�� ����ϰ� �ִ�.
			{
				auto width = pRenderer->GetRenderTarget()->GetWidth();
				auto height = pRenderer->GetRenderTarget()->GetHeight();

				D3D11_VIEWPORT vp;
				vp.Width = width;
				vp.Height = height;
				vp.TopLeftX = 0.0f;
				vp.TopLeftY = 0.0f;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				pImmediateContext->RSSetViewports(1, &vp);

				// ���ó� ī�޶� �ɸ���.
				Renderer::GetInstance().SetViewport(width, height);
			}

			//======================================================//
			// ���� ������Ʈ�� �׷����� �ʰ� �ִ�.					//
			// �̴� Deferred�� Compose shader�� �߰��ϸ鼭			//
			// Input layout�� ���� ���ȱ� �����̴�.					//
			// �� �� ���� shader ������ ������ ����� ��µȴ�.		//
			//======================================================//
			pRenderer->DrawScene();
			
			// RenderPassEnd
			{
				pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
			}
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