#include "EditorScene.h"

EditorScene::EditorScene(const std::string name)
	: Dive::Scene(name)
{
}

// �� ������ �׸��� ������ Hazel�� ECS�� �׷� �� �ִ�.
// ���� ��¼�� GameObject�� Update�� ���� Renderer Component���� Draw �Ǿ�� �� �� �ִ�.
void EditorScene::UpdateEditor(float elapsedTime, EditorCamera* pCamera)
{
	// begin / end scene��
	// ����ȭ�� Renderer�� ���� ������ ȣ������.
	// �ֳ��ϸ� Scene ��ü�� Engine ��ü�̱� �����̴�.

	// begin: camera ����
	//Dive::Renderer::BeginScene();
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

	// Scene���� Renderer���� �̸� ������ ���� ���� �� ���� �� ����.
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
