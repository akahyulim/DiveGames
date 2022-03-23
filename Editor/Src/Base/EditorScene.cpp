#include "EditorScene.h"
#include "SceneViewCamera.h"

// �̰� ���� ����̴�.
// Update / Render�� RenderPath�� �Ű�����.
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

		// active camera�κ��� �����;� �Ѵ�.
		float clearColors[4] = { 0.35f, 0.35f, 0.7f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView ? pDepthStencilView : nullptr);
	}

	// temp: �׷��� GameObject dirty check�� �� ������ �ϰ� �ִ�.
	// �ش� �ڵ带 �Լ�ȭ�ؼ� �� ������ ȣ���ϴ� ���� ����� �� �ִ�.
	Dive::Scene::Update(elapsedTime);

	// pass 0
	{
		// shader, state ���� �̸� bind �س��� ���� ���� �� �ϴ�.
		// �ڵ尡 �ʹ� ������ٸ� spartanó�� pass�� �̸� ����� ���� �� ȣ����� �ϴ� �͵� �����غ� �� �ִ�.
		// ���� �̶� pass�� ���� �������ĸ� ������ ���ƾ� �Ѵ�.
		// ���ĸ�źó�� Renderer�� �����Ϸ��� �ᱹ GameObjects�� �����ؾ� �Ѵ�.
		// �װ� �ȴٸ� Scene���� ������ �� ���⿡�� ȣ�⸸ �ϸ� �ȴ�.

		// Scene���� Renderer���� �̸� ������ ���� ���� �� ���� �� ����.
		for (auto pGameObject : GetGameObjects())
		{
			if (pGameObject->HasComponent<Dive::SpriteRenderable>())
			{
				// ���� ī�޶� ��ü�� �����ϴ� ���� �� ����ϴ�...
				// �ƴϸ� View, Proj�� �ٸ� cb�� map�ϴ� ���� ���� ���� �ִ�.
				//Dive::Renderer::DrawSprite(pCamera->GetViewMatrix(), pCamera->GetProjectionMatrix(), pGameObject);
			}
		}
	}

	// end scene
	Dive::Renderer::EndScene();
}
*/
