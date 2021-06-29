#include "Sandbox.h"

namespace sandbox
{
	Sandbox::Sandbox()
	{
		mAppTitle = "SandBox";
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

		ActivatePath(&mRenderPath);

		{
			auto gameObject = dive::Scene::GetGlobalScene().CreateGameObject();
			auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();
			meshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
			gameObject->SetName("Cube");
		}

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}