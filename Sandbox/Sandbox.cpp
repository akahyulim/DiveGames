#include "Sandbox.h"
using namespace DiveEngine;

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

		// 시스템 초기화 후 생성해야 한다.
		// 그게 아니라면 별도의 초기화 함수가 필요하다.
		m_pRenderPath = new RenderPath3D_Legacy;
		ActivatePath(m_pRenderPath);

		// 일단 Scene을 직접 생성한 후 Active로 설정한다.
		auto activeScene = SceneManager::GetInstance().CreateScene("Sandbox");
		SceneManager::GetInstance().SetActiveScene(activeScene);

	
		// Main Camera
		{
			auto pGameObject = activeScene->CreateGameObject();
			pGameObject->SetName("MainCamera");
			pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -5.0f);
			auto pCam = pGameObject->AddComponent<Camera>();

			// 크기 설정 위치가 에바다. 게다가 크기를 임의로 하드코딩 했다.
			pCam->SetScreenSize(800, 600);
		}

		// Objects
		{
			auto pGameObject = activeScene->CreateGameObject();
			auto pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
			pMeshRenderer->CreateDefaultMesh(eDefaultMeshType::Cube);
			pGameObject->SetName("Cube");
		}

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}