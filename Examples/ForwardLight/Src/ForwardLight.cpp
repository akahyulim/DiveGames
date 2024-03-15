#include "ForwardLight.h"

DEFINE_APPLICATION_MAIN(ForwardLight::ForwardLight)

// forward lights ����
// material, mesh, shader�� resourceCache���� ����
// gameObjects update - rotate
// input ����
namespace ForwardLight
{
	ForwardLight::ForwardLight()
		: Dive::Application()
		, m_pMainCam(nullptr)
		, m_pCube(nullptr)
		, m_pTriangle(nullptr)
		, m_pSphere(nullptr)
		, m_pCar(nullptr)
		, m_pDirLightA(nullptr)
		, m_pPointLightA(nullptr)
		, m_pPointLightB(nullptr)
		, m_pPointLightC(nullptr)
		, m_pSpotLightA(nullptr)
		, m_pSpotLightB(nullptr)
		, m_pSpotLightC(nullptr)
	{
	}

	ForwardLight::~ForwardLight()
	{
	}

	void ForwardLight::OnSetup()
	{
		// ����� ���� �ý��� ����
		// ���Ŀ� ���� ��θ� ���޹޾� ���� �ý����� ����

		m_Title = L"Forward Light";
		m_WindowWidth = 1600;
		m_WindowHeight = 900;
	}

	void ForwardLight::OnStart()
	{
		// create scene
		{
			Dive::Scene* pActiveScene = Dive::SceneManager::CreateScene("ForwardLight World");
			Dive::SceneManager::SetActiveScene(pActiveScene);

			// main camera
			m_pMainCam = pActiveScene->CreateGameObject("MainCam");
			auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
			pCamCom->SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
			m_pMainCam->GetTransform()->SetPosition(0.0f, 10.0f, -55.0f);

			createGameObjects(pActiveScene);
			createLights(pActiveScene);
		}

		// setup renderLayer
		{
			// �����δ� Scene�� ������ �ʿ䰡 ����.
			// ViewScreen ���ο��� ActiveScene�� ȹ���Ͽ� ����� ���̴�.
			auto pViewScreen = new Dive::ViewScreen(m_pMainCam->GetComponent<Dive::Camera>());
			Dive::Renderer::SetViewScreen(0, pViewScreen);
		}

		// subscribe events
		{
			SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(HandleUpdate));
		}
	}
	
	void ForwardLight::OnStop()
	{
	}

	void ForwardLight::HandleUpdate(const Dive::Event& e)
	{
		static float angle = 0.0f;
		angle += 0.1f;
		if (angle >= 360.0f)
			angle = 0.0f;

		if(m_pCube)
			m_pCube->GetTransform()->Rotate(DirectX::XMFLOAT3(0.0f, -0.5f, 0.0f));
		if(m_pTriangle)
			m_pTriangle->GetTransform()->Rotate(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
		if(m_pSphere)
			m_pSphere->GetTransform()->SetRotation(DirectX::XMFLOAT3(0.0f, angle, 0.0f));
		if (m_pCar)
			m_pCar->GetTransform()->Rotate(DirectX::XMFLOAT3(0.0f, -0.1f, 0.0f));

		{
			using namespace Dive;

			float deltaTime = static_cast<float>(Timer::GetDeltaTimeSec());

			auto pTransform = m_pMainCam->GetTransform();

			static float m_MoveSpeed = 10.0f;
			static float boost = 1.0f;
			if (Input::KeyPress(DIK_LSHIFT))
				boost = 5.0f;
			else
				boost = 1.0f;

			if (Input::KeyPress(DIK_UP))
			{
				pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
			}
			if (Input::KeyPress(DIK_DOWN))
			{
				pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
			}
			if (Input::KeyPress(DIK_LEFT))
			{
				pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
			}
			if (Input::KeyPress(DIK_RIGHT))
			{
				pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
			}

			if (Input::KeyDown(DIK_F5))
			{
				Dive::Graphics::ResizeResolution(1920, 1080);
				return;
			}
			if (Input::KeyDown(DIK_F6))
			{
				Dive::Graphics::ResizeResolution(1600, 900);
				return;
			}
			if (Input::KeyDown(DIK_F7))
			{
				Dive::Graphics::ResizeResolution(1024, 768);
				return;
			}
			if (Input::KeyDown(DIK_F1))
			{
				Graphics::AdjustWindow(Graphics::GetResolutionWidth(), Graphics::GetResolutionHeight(), false);
				return;
			}
			if (Input::KeyDown(DIK_F2))
			{
				Graphics::AdjustWindow(Graphics::GetResolutionWidth(), Graphics::GetResolutionHeight(), true);
				return;
			}
			if (Input::KeyDown(DIK_F3))
			{
				Graphics::SetFullScreen(true);
				return;
			}
			if (Input::KeyDown(DIK_F4))
			{
				Graphics::SetFullScreen(false);
				return;
			}
			if (Input::KeyDown(DIK_1))
			{
				auto pLightCom = m_pDirLightA->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}
			if (Input::KeyDown(DIK_2))
			{
				auto pLightCom = m_pPointLightA->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}
			if (Input::KeyDown(DIK_3))
			{
				auto pLightCom = m_pPointLightB->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}
			if (Input::KeyDown(DIK_4))
			{
				auto pLightCom = m_pPointLightC->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);		
				return;
			}
			if (Input::KeyDown(DIK_5))
			{
				auto pLightCom = m_pSpotLightA->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}
			if (Input::KeyDown(DIK_6))
			{
				auto pLightCom = m_pSpotLightB->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}
			if (Input::KeyDown(DIK_7))
			{
				auto pLightCom = m_pSpotLightC->GetComponent<Dive::Light>();
				pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
				return;
			}


			if (Input::MouseButtonPress(1))
			{
				// ���� Ű �Է°� ���콺 �̵��� ���ÿ� �۵����� �ʴ´�.
				const DirectX::XMFLOAT2& delta = Input::GetMouseMoveDelta();
				pTransform->Rotate(delta.y * 0.1f, delta.x * 0.1f, 0.0f);

				if (Input::KeyPress(DIK_W))
				{
					pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
				}
				if (Input::KeyPress(DIK_S))
				{
					pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
				}
				if (Input::KeyPress(DIK_A))
				{
					pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
				}
				if (Input::KeyPress(DIK_D))
				{
					pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
				}

				if (Input::KeyPress(DIK_Q))
				{
					pTransform->Translate(0.0f, m_MoveSpeed * deltaTime * boost, 0.0f);
				}
				if (Input::KeyPress(DIK_E))
				{
					pTransform->Translate(0.0f, -m_MoveSpeed * deltaTime * boost, 0.0f);
				}
			}
		}
	}
	
	void ForwardLight::createGameObjects(Dive::Scene* pActiveScene)
	{
		// textures
		auto pTexDMC = Dive::ResourceManager::GetResource<Dive::Texture2D>("Assets/Textures/dmc.jpg");
		auto pTexDOKEV = Dive::ResourceManager::GetResource<Dive::Texture2D>("Assets/Textures/dokev.jpeg");
		auto pTexPlane = Dive::ResourceManager::GetResource<Dive::Texture2D>("Assets/Textures/WornWood/WornWood_Albedo.tga");
		auto pTexStone = Dive::ResourceManager::GetResource<Dive::Texture2D>("Assets/Textures/Stone01.tga");

		// shader
		auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("CoreData/Shaders/ForwardLight.hlsl");
		pShader->CreateInputLayout(Dive::eVertexLayout::Static_Model);
		
		// meshes
		auto pTriangleModel = Dive::ModelFactory::CreateTriangle(5.0f);
		Dive::ResourceManager::AddManualResource(pTriangleModel);

		auto pCubeModel = Dive::ModelFactory::CreateCube(5.0f);
		Dive::ResourceManager::AddManualResource(pCubeModel);

		auto pSphereModel = Dive::ModelFactory::CreateSphere(5.0f);
		Dive::ResourceManager::AddManualResource(pSphereModel);

		auto pPlaneModel = Dive::ModelFactory::CreatePlane(50, 50);
		Dive::ResourceManager::AddManualResource(pPlaneModel);

		// materials
		// ���̴��� ���� �������� �ʾƵ� RenderPath�� �´� Default�� ����Ǿ� �־�� �Ѵ�.
		auto pTriangleMaterial = Dive::CreateMaterial("TriangleMaterial");
		pTriangleMaterial->SetShader(pShader);
		pTriangleMaterial->SetDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
		pTriangleMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDMC);
		Dive::ResourceManager::AddManualResource(pTriangleMaterial);

		auto pCubeMaterial = Dive::CreateMaterial("CubeMaterial");
		pCubeMaterial->SetShader(pShader);
		pCubeMaterial->SetDiffuseColor(0.0f, 1.0f, 1.0f, 1.0f);
		pCubeMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDOKEV);
		Dive::ResourceManager::AddManualResource(pCubeMaterial);

		auto pSphereMaterial = Dive::CreateMaterial("SphereMaterial");
		pSphereMaterial->SetShader(pShader);
		pSphereMaterial->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
		Dive::ResourceManager::AddManualResource(pSphereMaterial);

		auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
		pPlaneMaterial->SetShader(pShader);
		pPlaneMaterial->SetDiffuseColor(0.1f, 0.1f, 0.1f, 1.0f);
		pPlaneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexPlane);
		Dive::ResourceManager::AddManualResource(pPlaneMaterial);
		
		// tirangle gameobject
		m_pTriangle = pActiveScene->CreateGameObject("Triangle");
		m_pTriangle->GetTransform()->SetPosition(20.0f, 2.5f, -20.0f);
		auto pTriangleRenderableCom = m_pTriangle->AddComponent<Dive::Renderable>();
		// �𵨷� ������ �� �κ��� �ָ�������.
		Dive::Mesh* pMesh = pTriangleModel->GetMeshAt();
		pTriangleRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
		pTriangleRenderableCom->SetMaterial(pTriangleMaterial);
		pTriangleModel->SetRootObject(m_pTriangle);

		// cube gameobject
		m_pCube = pActiveScene->CreateGameObject("Cube");
		m_pCube->GetTransform()->SetPosition(-20.0f, 2.5f, -20.0f);
		auto pCubeRenderableCom = m_pCube->AddComponent<Dive::Renderable>();
		pMesh = pCubeModel->GetMeshAt();
		pCubeRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
		pCubeRenderableCom->SetMaterial(pCubeMaterial);
		pCubeModel->SetRootObject(m_pCube);

		// sphere gameObject
		//m_pSphere = pActiveScene->CreateGameObject("Sphere");
		//m_pSphere->GetTransform()->SetPosition(0.0f, 5.0f, 10.0f);
		//auto pSphereRenderableCom = m_pSphere->AddComponent<Dive::Renderable>();
		//pSphereRenderableCom->SetGeometry(pSphereModel, 0, pSphereModel->GetVertexCount(), 0, pSphereModel->GetIndexCount());
		//pSphereRenderableCom->SetMaterial(pSphereMaterial);
		//pSphereModel->SetRootGameObject(m_pSphere);
		
		// bottom gameobject
		auto pBottom = pActiveScene->CreateGameObject("Bottom");
		auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
		pMesh = pPlaneModel->GetMeshAt();
		pBottomRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
		pBottomRenderableCom->SetMaterial(pPlaneMaterial);
		pPlaneModel->SetRootObject(pBottom);
		
		// test
		{
			// static
			{
				/*
				auto pHelmetModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/damaged_helmet/DamagedHelmet.gltf");
				pHelmetModel->GetRootObject()->GetTransform()->SetLocalScale(3.0f, 3.0f, 3.0f);
				pHelmetModel->GetRootObject()->GetTransform()->SetPosition(-5.0f, 3.0f, -20.0f);

				auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/material_ball_in_3d-coat/Scene.gltf");
				pBallModel->GetRootObject()->GetTransform()->SetPosition(15.0f, 7.5f, 0.0f);

				auto pSphere = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Sphere.obj");
				pSphere->GetRootObject()->GetTransform()->SetLocalScale(5.0f, 5.0f, 5.0f);
				pSphere->GetRootObject()->GetTransform()->SetPosition(0.0f, 10.0f, 20.0f);

				auto pCube = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Cube/Cube_fbx.fbx");
				pCube->GetRootObject()->GetTransform()->SetLocalScale(0.1f, 0.1f, 0.1f);
				pCube->GetRootObject()->GetTransform()->SetPosition(-20.0f, 0.0f, 0.0f);
				*/
				//auto pBackpack = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/survival-guitar-backpack/Survival_backpack_2.fbx");
			}

			// scene
			{
				auto pCarModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/toyota_ae86_sprinter_trueno_zenki/scene.gltf");
				m_pCar = pCarModel->GetRootObject(); 
				m_pCar->GetTransform()->SetLocalRotation(90.0f, 180.f, 0.f);
				m_pCar->GetTransform()->SetLocalScale(0.1f, 0.1f, 0.1f);
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/sponza/sponza.obj");
			}

			// skinned
			{
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/sitting.fbx");
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Y Bot@Zombie Stumbling.fbx");
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/start walking.fbx");
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/pilot/Pilot_LP_Animated.fbx");
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/vampire/dancing_vampire.dae");
				//Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/stormtrooper/silly_dancing.fbx");
			}
		}
	}

	void ForwardLight::createLights(Dive::Scene* pActiveScene)
	{
		// Directional Light
		m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
		auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
		pDirLightCom->SetColor(0.9f, 0.9f, 0.9f);
		pDirLightCom->SetType(Dive::eLightType::Directional);
		pDirLightCom->SetLookAt(1.0f, -1.0f, 1.0f);

		// PointLights
		{
			m_pPointLightA = pActiveScene->CreateGameObject("PointLightA");
			m_pPointLightA->GetTransform()->SetPosition(0.0f, 5.0f, -15.0f);
			auto pPointLightCom = m_pPointLightA->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);

			m_pPointLightB = pActiveScene->CreateGameObject("PointLightB");
			m_pPointLightB->GetTransform()->SetPosition(-10.0f, 5.0f, 15.0f);
			pPointLightCom = m_pPointLightB->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);

			m_pPointLightC = pActiveScene->CreateGameObject("PointLightC");
			m_pPointLightC->GetTransform()->SetPosition(10.0f, 5.0f, 15.0f);
			pPointLightCom = m_pPointLightC->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
		}

		// SpotLights
		{
			m_pSpotLightA = pActiveScene->CreateGameObject("SpotLightA");
			m_pSpotLightA->GetTransform()->SetPosition(0.0f, 20.0f, -30.0f);
			auto pSpotLightCom = m_pSpotLightA->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

			m_pSpotLightB = pActiveScene->CreateGameObject("SpotLightB");
			m_pSpotLightB->GetTransform()->SetPosition(30.0f, 20.0f, 0.0f);
			pSpotLightCom = m_pSpotLightB->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

			m_pSpotLightC = pActiveScene->CreateGameObject("SpotLightC");
			m_pSpotLightC->GetTransform()->SetPosition(-30.0f, 20.0f, 0.0f);
			pSpotLightCom = m_pSpotLightC->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(1.0f, 0.0f, 1.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

			/*
			auto pHeadlights = pActiveScene->CreateGameObject("Headlights");
			auto pCarModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/toyota_ae86_sprinter_trueno_zenki/scene.gltf");
			auto pHeadlightsObject = pCarModel->GetMeshByName("Headlights_LampCovers_0")->GetGameObject();
			pHeadlights->SetParentGameObject(pHeadlightsObject);
			pSpotLightCom = pHeadlights->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pSpotLightCom->SetLookAt(0.0f, 0.0f, 10.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
			*/
		}
	}
}