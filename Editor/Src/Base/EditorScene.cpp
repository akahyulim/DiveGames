#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

// 이 곳에서 그리는 이유는 Hazel이 ECS라 그럴 수 있다.
// 따라서 어쩌면 GameObject의 Update를 통해 Renderer Component에서 Draw 되어야 할 수 있다.
void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// begin / end scene은
	// 전역화된 Renderer를 직접 가져와 호출하자.
	// 왜냐하면 Scene 자체가 Engine 객체이기 때문이다.

	// begin: camera 전달
	//Dive::Renderer::BeginScene();
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

	// Scene에서 Renderer들을 미리 나누어 놓는 편이 더 나을 것 같다.
	for (auto pGameObject : GetGameObjects())
	{
		if (pGameObject->HasComponent<Dive::SpriteRenderer>())
		{
			Dive::Renderer::DrawSprite(pGameObject->GetComponent<Dive::Transform>(), pGameObject->GetComponent<Dive::SpriteRenderer>());
		}
	}

	// end
	Dive::Renderer::EndScene();
}
