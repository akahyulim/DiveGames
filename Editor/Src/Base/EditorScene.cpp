#include "EditorScene.h"
#include "SceneViewCamera.h"

// 이건 제거 대상이다.
// Update / Render는 RenderPath로 옮겨진다.
EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}
/*
void EditorScene::UpdateEditor(float elapsedTime, SceneViewCamera* pCamera)
{
	// begin scene
	{
		auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();
		auto pRenderTargetView = Dive::Renderer::GetSampleTexture()->GetRenderTargetView();
		auto pDepthStencilView = Dive::Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera로부터 가져와야 한다.
		float clearColors[4] = { 0.35f, 0.35f, 0.7f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView ? pDepthStencilView : nullptr);
	}

	// temp: 그런데 GameObject dirty check를 이 곳에서 하고 있다.
	// 해당 코드를 함수화해서 이 곳에서 호출하는 것을 고려할 수 있다.
	Dive::Scene::Update(elapsedTime);

	// pass 0
	{
		// shader, state 등은 미리 bind 해놓는 편이 나을 듯 하다.
		// 코드가 너무 길어진다면 spartan처럼 pass를 미리 만들어 놓은 후 호출토록 하는 것도 생각해볼 수 있다.
		// 물론 이때 pass를 누가 가지느냐를 생각해 보아야 한다.
		// 스파르탄처럼 Renderer에 구현하려면 결국 GameObjects를 전달해야 한다.
		// 그게 싫다면 Scene에서 구현한 후 여기에선 호출만 하면 된다.

		// Scene에서 Renderer들을 미리 나누어 놓는 편이 더 나을 것 같다.
		for (auto pGameObject : GetGameObjects())
		{
			if (pGameObject->HasComponent<Dive::SpriteRenderable>())
			{
				// 역시 카메라 객체를 전달하는 편이 더 깔끔하다...
				// 아니면 View, Proj를 다른 cb에 map하는 것이 나을 수도 있다.
				//Dive::Renderer::DrawSprite(pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix(), pGameObject);
			}
		}
	}

	// end scene
	Dive::Renderer::EndScene();
}
*/
