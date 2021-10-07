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

		// �ý��� �ʱ�ȭ �� �����ؾ� �Ѵ�.
		// �װ� �ƴ϶�� ������ �ʱ�ȭ �Լ��� �ʿ��ϴ�.
		m_pRenderPath = new dive::RenderPath3D_Legacy;
		ActivatePath(m_pRenderPath);

		// �ϴ� Scene�� ���� ������ �� Active�� �����Ѵ�.
		auto activeScene = dive::SceneManager::GetInstance().CreateScene("Sandbox");
		dive::SceneManager::GetInstance().SetActiveScene(activeScene);

	
		// Main Camera
		{
			auto pGameObject = activeScene->CreateGameObject();
			pGameObject->SetName("MainCamera");
			pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -5.0f);
			auto pCam = pGameObject->AddComponent<dive::Camera>();

			// ũ�� ���� ��ġ�� ���ٴ�. �Դٰ� ũ�⸦ ���Ƿ� �ϵ��ڵ� �ߴ�.
			pCam->SetScreenSize(800, 600);
		}

		// Objects
		{
			auto pGameObject = activeScene->CreateGameObject();
			auto pMeshRenderer = pGameObject->AddComponent<dive::MeshRenderer>();
			pMeshRenderer->CreateDefaultMesh(dive::eDefaultMeshType::Cube);
			pGameObject->SetName("Cube");
		}

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}
}