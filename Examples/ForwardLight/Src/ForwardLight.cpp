#include "ForwardLight.h"

DEFINE_APPLICATION_MAIN(ForwardLight::ForwardLight)

// forward lights 적용
// material, mesh, shader를 resourceCache에서 관리
// gameObjects update - rotate
// input 적용
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
		, m_pSpotLightForCar(nullptr)
	{
	}

	ForwardLight::~ForwardLight()
	{
	}

	void ForwardLight::OnSetup()
	{
		// 여기는 서브 시스템 설정
		// 추후엔 파일 경로를 전달받아 서브 시스템을 설정

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
			m_pMainCam->SetPosition(0.0f, 10.0f, -55.0f);

			createGameObjects(pActiveScene);
			createLights(pActiveScene);
		}

		// setup renderLayer
		{
			// 실제로는 Scene을 전달할 필요가 없다.
			// ViewScreen 내부에서 ActiveScene을 획득하여 사용할 것이다.
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

		// rotate
		{
			if (m_pCube)
			{
				static bool bTouchBottom = false;
				const auto position = m_pCube->GetPosition();

				if (position.y <= 2.5f)
					bTouchBottom = true;
				else if (position.y >= 20.0f)
					bTouchBottom = false;

				if (bTouchBottom)
					m_pCube->Translate(0.0f, 0.05f, 0.0f);
				else if(!bTouchBottom)
					m_pCube->Translate(0.0f, -0.05f, 0.0f);
			}
			if (m_pTriangle)
				m_pTriangle->Rotate(0.0f, 0.0f, 1.0f);
			if (m_pCar)
				m_pCar->Rotate(0.0f, 0.1f, 0.0f);

			const auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/material_ball_in_3d-coat/Scene.gltf");
			if (pBallModel)
				pBallModel->GetRootObject()->Rotate(0.0f, 0.1f, 0.0f, Dive::eSpace::World);

			const auto pBoxModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Cube/Cube_fbx.fbx");
			if (pBoxModel)
				pBoxModel->GetRootObject()->Rotate(0.0f, -0.5f, 0.0f);
		}

		// change light color
		{
			static double elapsedTime = 0;
			if (elapsedTime < 1)
				elapsedTime += Dive::Timer::GetDeltaTimeSec();
			else if (elapsedTime >= 1)
			{
				elapsedTime = 0;

				static std::random_device rd;
				static std::mt19937 gen(rd());
				static std::uniform_real_distribution<> dist(0, 1);

				float r, g, b;
				r = (float)dist(gen);
				g = (float)dist(gen);
				b = (float)dist(gen);

				//auto pLightCom = m_pSpotLightForCar->GetComponent<Dive::Light>();
				//pLightCom->SetColor(r, g, b);
			}
		}

		{
			using namespace Dive;

			float deltaTime = static_cast<float>(Timer::GetDeltaTimeSec());

			static float m_MoveSpeed = 10.0f;
			static float boost = 1.0f;
			if (Input::KeyPress(DIK_LSHIFT))
				boost = 5.0f;
			else
				boost = 1.0f;

			// move controll
			{
				if (Input::KeyPress(DIK_UP))
				{
					m_pMainCam->Rotate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_DOWN))
				{
					m_pMainCam->Rotate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_LEFT))
				{
					m_pMainCam->Rotate(0.0f, -m_MoveSpeed * deltaTime * boost, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_RIGHT))
				{
					m_pMainCam->Rotate(0.0f, m_MoveSpeed * deltaTime * boost, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_W))
				{
					m_pMainCam->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_S))
				{
					m_pMainCam->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_A))
				{
					m_pMainCam->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_D))
				{
					m_pMainCam->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f, Dive::eSpace::Self);
				}

				if (Input::KeyPress(DIK_Q))
				{
					m_pMainCam->Translate(0.0f, m_MoveSpeed * deltaTime * boost, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_E))
				{
					m_pMainCam->Translate(0.0f, -m_MoveSpeed * deltaTime * boost, 0.0f, Dive::eSpace::Self);
				}
			}

			// window controll
			{
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
			}

			// light controll
			{
				if (Input::KeyDown(DIK_1))
				{
					auto pLightCom = m_pDirLightA->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_2))
				{
					if (!m_pPointLightA)
						return;
					auto pLightCom = m_pPointLightA->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_3))
				{
					if (!m_pPointLightB)
						return;
					auto pLightCom = m_pPointLightB->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_4))
				{
					if (!m_pPointLightC)
						return;
					auto pLightCom = m_pPointLightC->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_5))
				{
					if (!m_pSpotLightA)
						return;
					auto pLightCom = m_pSpotLightA->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_6))
				{
					if (!m_pSpotLightB)
						return;
					auto pLightCom = m_pSpotLightB->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
				}
				if (Input::KeyDown(DIK_7))
				{
					if (!m_pSpotLightC)
						return;
					auto pLightCom = m_pSpotLightC->GetComponent<Dive::Light>();
					pLightCom->SetEnabled(pLightCom->IsEnabled() ? false : true);
					return;
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

		auto pPlaneModel = Dive::ModelFactory::CreatePlane(80, 80);
		Dive::ResourceManager::AddManualResource(pPlaneModel);

		// materials
		// 셰이더를 직접 전달하지 않아도 RenderPath에 맞는 Default가 적용되어 있어야 한다.
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
		//m_pTriangle = pActiveScene->CreateGameObject("Triangle");
		//m_pTriangle->SetPosition(20.0f, 2.5f, -20.0f);
		//auto pTriangleRenderableCom = m_pTriangle->AddComponent<Dive::Renderable>();
		// 모델로 받으면 이 부분이 애매해진다.
		//Dive::Mesh* pMesh = pTriangleModel->GetMeshAt();
		//pTriangleRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
		//pTriangleRenderableCom->SetMaterial(pTriangleMaterial);
		//pTriangleModel->SetRootObject(m_pTriangle);
		
		// cube gameobject
		m_pCube = pActiveScene->CreateGameObject("Cube");
		m_pCube->SetPosition(-20.0f, 2.5f, -20.0f);
		auto pCubeRenderableCom = m_pCube->AddComponent<Dive::Renderable>();
		auto pMesh = pCubeModel->GetMeshAt();
		pCubeRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
		pCubeRenderableCom->SetMaterial(pCubeMaterial);
		pCubeModel->SetRootObject(m_pCube);

		// sphere gameObject
		//m_pSphere = pActiveScene->CreateGameObject("Sphere");
		//m_pSphere->SetPosition(0.0f, 5.0f, 10.0f);
		//auto pSphereRenderableCom = m_pSphere->AddComponent<Dive::Renderable>();
		//pSphereRenderableCom->SetGeometry(pSphereModel, 0, pSphereModel->GetVertexCount(), 0, pSphereModel->GetIndexCount());
		//pSphereRenderableCom->SetMaterial(pSphereMaterial);
		//pSphereModel->SetRootGameObject(m_pSphere);
		
		// bottom gameobject
		auto pBottom = pActiveScene->CreateGameObject("Bottom");
		auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
		auto pBottomMesh = pPlaneModel->GetMeshAt();
		pBottomRenderableCom->SetGeometry(pBottomMesh, 0, pBottomMesh->GetVertexCount(), 0, pBottomMesh->GetIndexCount());
		pBottomRenderableCom->SetMaterial(pPlaneMaterial);
		pPlaneModel->SetRootObject(pBottom);
		
		// test
		{
			// static
			{
				auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/material_ball_in_3d-coat/Scene.gltf");
				pBallModel->GetRootObject()->SetPosition(20.0f, 7.5f, 0.0f);
				
				auto pSphereModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Sphere.obj");
				pSphereModel->GetRootObject()->SetPosition(20.0f, 15.0f, -30.0f);
				pSphereModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
				pSphereModel->GetRootObject()->SetParent(pBallModel->GetRootObject());

				auto pBoxModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/Cube/Cube_fbx.fbx");
				pBoxModel->GetRootObject()->SetScale(0.1f, 0.1f, 0.1f);
				pBoxModel->GetRootObject()->SetPosition(-20.0f, 0.0f, 20.0f);
				pBoxModel->GetRootObject()->SetRotation(0.0f, -45.0f, 0.0f);

				auto pHelmetModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/damaged_helmet/DamagedHelmet.gltf");
				pHelmetModel->GetRootObject()->SetPosition(-20.0f, 20.0f, 20.0f);
				pHelmetModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f); 
				pHelmetModel->GetRootObject()->SetParent(pBoxModel->GetRootObject());

				const auto angles = pHelmetModel->GetRootObject()->GetRotationDegrees();
				DV_DEBUG("HelmetModel Rotation Angles: {0:f}, {1:f}, {2:f}", angles.x, angles.y, angles.z);
			}

			// scene
			{
				
				//auto pCarModel = Dive::ResourceManager::GetResource<Dive::Model>("Assets/Models/toyota_ae86_sprinter_trueno_zenki/scene.gltf");
				//m_pCar = pCarModel->GetRootObject(); 
				//m_pCar->SetScale(0.1f, 0.1f, 0.1f);

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
			m_pPointLightA->SetPosition(0.0f, 5.0f, -15.0f);
			auto pPointLightCom = m_pPointLightA->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);

			m_pPointLightB = pActiveScene->CreateGameObject("PointLightB");
			m_pPointLightB->SetPosition(-10.0f, 5.0f, 15.0f);
			pPointLightCom = m_pPointLightB->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);

			m_pPointLightC = pActiveScene->CreateGameObject("PointLightC");
			m_pPointLightC->SetPosition(10.0f, 5.0f, 15.0f);
			pPointLightCom = m_pPointLightC->AddComponent<Dive::Light>();
			pPointLightCom->SetType(Dive::eLightType::Point);
			pPointLightCom->SetRange(30.0f);
			pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
		}
		
		// SpotLights
		{
			
			m_pSpotLightA = pActiveScene->CreateGameObject("SpotLightA");
			m_pSpotLightA->SetPosition(0.0f, 20.0f, -30.0f);
			auto pSpotLightCom = m_pSpotLightA->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

			m_pSpotLightB = pActiveScene->CreateGameObject("SpotLightB");
			m_pSpotLightB->SetPosition(30.0f, 20.0f, 0.0f);
			pSpotLightCom = m_pSpotLightB->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

			m_pSpotLightC = pActiveScene->CreateGameObject("SpotLightC");
			m_pSpotLightC->SetPosition(-30.0f, 20.0f, 0.0f);
			pSpotLightCom = m_pSpotLightC->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(60.0f);
			pSpotLightCom->SetColor(1.0f, 0.0f, 1.0f);
			pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
			

			m_pSpotLightForCar = pActiveScene->CreateGameObject("SpotLightC");
			if (m_pSpotLightForCar)
			{
				m_pSpotLightForCar->SetPosition(m_pMainCam->GetPosition());
				// 왜인지 모르겠지만 카메라의 이동을 따라가지 못한다.
				// 정확하게는 의도와는 다르다.
				// 처음에는 디폴트 위치인 0, 0, 0 이지만
				// 첫 이동을 인식하면 카메라의 위치와 동기화된다.
				m_pSpotLightForCar->SetParent(m_pMainCam);
				auto pSpotLightCom = m_pSpotLightForCar->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(100.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
				// 회전이 먹히지 않는다.
				pSpotLightCom->SetLookAt(m_pMainCam->GetForward());
				pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
			}

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