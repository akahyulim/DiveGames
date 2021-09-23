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

		// 일단 Scene을 직접 생성한 후 Active로 설정한다.
		auto activeScene = dive::SceneManager::GetInstance().CreateScene("Sandbox");
		dive::SceneManager::GetInstance().SetActiveScene(activeScene);

		

		{
			auto gameObject = activeScene->CreateGameObject();
			auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
			meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
			gameObject->SetName("Cube");
		}

		// 이제 카메라가 없으면 안된다?
		{
			auto pGameObject = activeScene->CreateGameObject();
			pGameObject->SetName("Cam");
			pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -5.0f);
			auto pCam = pGameObject->AddComponent<dive::Camera>();
			// 카메라에 ScreenSize를 설정해 주어야 한다.
			// 직접 찾도록 하는게 나을 것 같은데...
		}

		*/

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}