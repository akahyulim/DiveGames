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

		// 일단 Scene을 직접 생성한 후 Active로 설정한다.
		auto activeScene = dive::SceneManager::GetInstance().CreateScene("Sandbox");
		dive::SceneManager::GetInstance().SetActiveScene(activeScene);

		ActivatePath(&mRenderPath);

		{
			auto gameObject = activeScene->CreateGameObject();
			auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
			meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
			gameObject->SetName("Cube");
		}

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}