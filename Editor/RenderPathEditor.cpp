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

		// 카메라 설정?
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
		* 이 부분에 UpdateVisibility가 들어간다.
		* 이는 Spartan처럼 GameObject Container를 단순히 Update하고 분류하는 것 외에도
		* Camera를 통해 Culling하는 과정이 수행된다.
		* 한 가지 특이한 점은 이를 관리하는 것이 RenderPath이지만
		* 수행하는 것은 Renderer라는 것이다.
		*/

		// Frame Data Update
		{
			Renderer::GetInstance().UpdateCB();
		}
	}
	
	// WickedEngine의 사용법 정리
	// 1. GBuffer는 RenderPath가 관리한다.
	//	- 직접 생성 및 크기를 갱신한다.
	//	- 생성 후 RenderPass를 통해 GBuffer처럼 RenderTarget을 묶는다.
	// 2. Pass의 시작은 항상 RenderPassBegin. 끝은 RenderPassEnd
	//	- RenderPass를 전달하여 OMSet 및 Clear를 수행한다.
	//	- OMSet을 nullptr로 떼어준다.
	// 3. Viewport는 RenderPass를 구성하는 RenderTargetView의 크기로 설정
	// 4. DrawXXX() 함수에 Visibility 등을 전달하여 Draw
	//	- DrawXXX() 내부에서 또 여러가지 처리를 한 후 Draw
	void RenderPathEditor::Render() const
	{
		if (!m_pScene)
			return;

		auto pRenderer = &Renderer::GetInstance();
		assert(pRenderer);

		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext);

		{
			// 이 RenderTarget Texture는 RenderPath에서 만들고 관리하는게 맞다.
			// 왜... 맞다고 한거지...? 결과를 그리는 텍스쳐로 염두한 것일까?
			auto pRTV = pRenderer->GetRenderTarget()->GetColorRenderTargetView();//GetFrameTexture()->GetRenderTargetView();

			// RenerPassBegin
			{
				//pImmediateContext->OMSetRenderTargets(1, &pRTV, nullptr);
				// 이걸 굳이 Current Camera의 Update() 등에서 수행하지 않은 이유는
				// 모든 RenderTexture가 굳이 Camera를 가질 필요는 없다는 의미?
				// 그렇다면 아래 ClearRenderTargetView()도 RenderTexture가 처리할 수 있어야 하는데? 
				// 찾아보니 이게 존재하는 이유조차 의문이 든다.
				dvRenderTexture::SetActive(pRenderer->GetRenderTarget());

				// 사실 GBuffer라면 특정 색으로 초기화해야 할 거다.
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

				// 이건 아마도 Camera의 Update() 등에서 수행할 것 같다.
				pImmediateContext->ClearRenderTargetView(pRTV, clearColors);
			}

			// Wicked는 먼저 RenderTarget으로부터 크기를 가져와
			// Viewport를 Bind한다.
			// 하지만 현재 구현은 Camera를 통해 Viewport를 계산하고 있다.
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

				// 역시나 카메라가 걸린다.
				Renderer::GetInstance().SetViewport(width, height);
			}

			//======================================================//
			// 현재 오브젝트가 그려지지 않고 있다.					//
			// 이는 Deferred와 Compose shader를 추가하면서			//
			// Input layout이 꼬여 버렸기 때문이다.					//
			// 위 두 개의 shader 생성을 막으면 제대로 출력된다.		//
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
		// 이 함수는 Backbuffer의 RTV Set & Clean과 Present 사이에 호출된다.
		// 따라서 여기에서 RenderTexture들을 Backbuffer에 그려야 한다.
		
		//======================================================================================//
		// 현재는 단일 RenderTexture에 하나의 화면을 그린 결과물이 Present 되고 있다.			//
		// 사실 이 과정에서 구현처리가 더 필요할 것 같은데... 현재 별 이상없이 돌아가고 있다.	//
		// 추후 꼭 이를 살펴보고 해결해야 한다.													//
		//======================================================================================//
	}
}