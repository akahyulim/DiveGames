#include "Sandbox.h"

namespace sandbox
{
	Sandbox::Sandbox()
	{
		m_AppTitle = "SandBox";
	}

	Sandbox::~Sandbox()
	{
	}

	bool Sandbox::Initialize()
	{
		if (!Runtime::Initialize())
		{
			return false;
		}

		ActivatePath(&m_RenderPath);

		/*

		// �ϴ� Scene�� ���� ������ �� Active�� �����Ѵ�.
		auto activeScene = dive::SceneManager::GetInstance().CreateScene("Sandbox");
		dive::SceneManager::GetInstance().SetActiveScene(activeScene);

		

		{
			auto gameObject = activeScene->CreateGameObject();
			auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
			meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
			gameObject->SetName("Cube");
		}

		// ���� ī�޶� ������ �ȵȴ�?
		{
			auto pGameObject = activeScene->CreateGameObject();
			pGameObject->SetName("Cam");
			pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -5.0f);
			auto pCam = pGameObject->AddComponent<dive::Camera>();
			// ī�޶� ScreenSize�� ������ �־�� �Ѵ�.
			// ���� ã���� �ϴ°� ���� �� ������...
		}

		*/

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}