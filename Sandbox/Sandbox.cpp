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

		// �ý��� �ʱ�ȭ �� �����ؾ� �Ѵ�.
		// �װ� �ƴ϶�� ������ �ʱ�ȭ �Լ��� �ʿ��ϴ�.
		m_pRenderPath = new RenderPath3D_Legacy;
		ActivatePath(m_pRenderPath);

		// �ϴ� Scene�� ���� ������ �� Active�� �����Ѵ�.
		auto activeScene = SceneManager::GetInstance().CreateScene("Sandbox");
		SceneManager::GetInstance().SetActiveScene(activeScene);

	
		// Main Camera
		{
			auto pGameObject = activeScene->CreateGameObject();
			pGameObject->SetName("MainCamera");
			pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -5.0f);
			auto pCam = pGameObject->AddComponent<Camera>();

			// ũ�� ���� ��ġ�� ���ٴ�. �Դٰ� ũ�⸦ ���Ƿ� �ϵ��ڵ� �ߴ�.
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