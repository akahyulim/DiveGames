#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox()
		: Dive::Application()
		, m_pGraphics(nullptr)
		, m_pInput(nullptr)
		, m_pMainCam(nullptr)
		, m_pDummy(nullptr)
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
		, m_pFlashLight(nullptr)
		, m_SceneType(eSceneType::Base)		// Scene을 런타임에서 바꾸고 싶다.
	{
	}

	Sandbox::~Sandbox()
	{
	}
	
	void Sandbox::OnSetup()
	{
		// 여기는 서브 시스템 설정
		// 추후엔 파일 경로를 전달받아 서브 시스템을 설정
		m_Title = L"Sandbox";
		m_WindowWidth = 1600;
		m_WindowHeight = 900;
	}

	void Sandbox::OnStart()
	{
		m_pGraphics = Dive::Graphics::GetInstance();
		m_pInput = Dive::Input::GetInstance();

		// create scene
		{
			switch (m_SceneType)
			{
			case eSceneType::Base:
				createBaseScene();
				break;
			case eSceneType::Car:
				createCarScene();
				break;
			case eSceneType::Sponza:
				createSponzaScene();
				break;
			case eSceneType::Field:
				createFieldScene();
				break;
			case eSceneType::Texture:
				createTextureScene();
				break;
			default:
				DV_CRITICAL("잘못된 씬을 선택하였습니다. 시스템을 종료합니다.");
				OnStop();
				return;
			}
		}

		// setup renderLayer
		{
			auto pViewScreen = new Dive::ViewScreen(m_pMainCam->GetComponent<Dive::Camera>(), Dive::eRenderPathType::Deferred);
			Dive::Renderer::GetInstance()->SetViewScreen(0, pViewScreen);
		}

		// subscribe events
		{
			SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(HandleUpdate));
		}
	}
	
	void Sandbox::OnStop()
	{
	}

	void Sandbox::HandleUpdate(const Dive::Event& e)
	{
		//using namespace Dive;
		float deltaTime = static_cast<float>(Dive::GetEngine()->GetDeltaTimeSec());

		static bool bMove = false;

		static float bSlow = 0;
		if (m_pInput->KeyPress(DIK_LSHIFT))
			bSlow = 1.0f;
		else
			bSlow = 5.0f;

		if (bMove)
		{
			if (m_SceneType == eSceneType::Base)
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
						m_pCube->Translate(0.0f, 5.0f * deltaTime * bSlow, 0.0f);
					else if (!bTouchBottom)
						m_pCube->Translate(0.0f, -5.0f * deltaTime * bSlow, 0.0f);
				}

				if (m_pTriangle)
					m_pTriangle->Rotate(0.0f, 0.0f, 100.0f * deltaTime * bSlow);

				const auto pBallModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf");
				if (pBallModel)
					pBallModel->GetRootObject()->Rotate(0.0f, 5.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);

				const auto pBoxModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Cube/Cube_fbx.fbx");
				if (pBoxModel)
					pBoxModel->GetRootObject()->Rotate(0.0f, -25.00f * deltaTime * bSlow, 0.0f);
			}
			else if (m_SceneType == eSceneType::Car)
			{
				m_pCar->Rotate(0.0f, 10.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);
			}
			else if (m_SceneType == eSceneType::Field)
			{
				//if(m_pDirLightA)
				//	m_pDirLightA->Rotate(0.0f, 30.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);

				if(m_pDummy)
					m_pDummy->Rotate(0.0f, 10.0f * deltaTime * bSlow, 0.0f);

				if (m_pCube)
				{
					static bool bTouchBottom = false;
					const auto position = m_pCube->GetPosition();

					if (position.y <= 5.0f)
						bTouchBottom = true;
					else if (position.y >= 20.0f)
						bTouchBottom = false;

					if (bTouchBottom)
						m_pCube->Translate(0.0f, 5.0f * deltaTime * bSlow, 0.0f);
					else if (!bTouchBottom)
						m_pCube->Translate(0.0f, -5.0f * deltaTime * bSlow, 0.0f);
				}
			}
		}

		{
			static float rotateSpeed = 100.0f;
			static float moveSpeed = (Dive::GetSceneManager()->GetActiveScene()->GetName() == "Test World") ? 10.0f : 100.0f;

			// move controll
			{
				if (m_pInput->KeyPress(DIK_UP))
				{
					m_pMainCam->Rotate(-rotateSpeed * deltaTime, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_DOWN))
				{
					m_pMainCam->Rotate(rotateSpeed * deltaTime, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_LEFT))
				{
					m_pMainCam->Rotate(0.0f, -rotateSpeed * deltaTime, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_RIGHT))
				{
					m_pMainCam->Rotate(0.0f, rotateSpeed * deltaTime, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_W))
				{
					m_pMainCam->Translate(0.0f, 0.0f, moveSpeed * deltaTime * bSlow, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_S))
				{
					m_pMainCam->Translate(0.0f, 0.0f, -moveSpeed * deltaTime * bSlow, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_A))
				{
					m_pMainCam->Translate(-moveSpeed * deltaTime * bSlow, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_D))
				{
					m_pMainCam->Translate(moveSpeed * deltaTime * bSlow, 0.0f, 0.0f, Dive::eSpace::Self);
				}

				if (m_pInput->KeyPress(DIK_Q))
				{
					m_pMainCam->Translate(0.0f, moveSpeed * deltaTime * bSlow, 0.0f, Dive::eSpace::Self);
				}
				if (m_pInput->KeyPress(DIK_E))
				{
					m_pMainCam->Translate(0.0f, -moveSpeed * deltaTime * bSlow, 0.0f, Dive::eSpace::Self);
				}
			}

			// window controll
			{
				if (m_pInput->KeyDown(DIK_F5))
				{
					m_pGraphics->ResizeResolution(1920, 1080);
					return;
				}
				if (m_pInput->KeyDown(DIK_F6))
				{
					m_pGraphics->ResizeResolution(1600, 900);
					return;
				}
				if (m_pInput->KeyDown(DIK_F7))
				{
					m_pGraphics->ResizeResolution(1024, 768);
					return;
				}
				if (m_pInput->KeyDown(DIK_F1))
				{
					m_pGraphics->AdjustWindow(m_pGraphics->GetResolutionWidth(), m_pGraphics->GetResolutionHeight(), false);
					return;
				}
				if (m_pInput->KeyDown(DIK_F2))
				{
					m_pGraphics->AdjustWindow(m_pGraphics->GetResolutionWidth(), m_pGraphics->GetResolutionHeight(), true);
					return;
				}
				if (m_pInput->KeyDown(DIK_F3))
				{
					m_pGraphics->SetFullScreen(true);
					return;
				}
				if (m_pInput->KeyDown(DIK_F4))
				{
					m_pGraphics->SetFullScreen(false);
					return;
				}
			}

			// light controll
			{
				if (m_pInput->KeyDown(DIK_1) && m_pDirLightA)
				{
					m_pDirLightA->SetActive(m_pDirLightA->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_2) && m_pPointLightA)
				{
					m_pPointLightA->SetActive(m_pPointLightA->IsActive() ? false : true);
					return;
				}
		
				if (m_pInput->KeyDown(DIK_3) && m_pPointLightB)
				{
					m_pPointLightB->SetActive(m_pPointLightB->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_4) && m_pPointLightC)
				{
					m_pPointLightC->SetActive(m_pPointLightC->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_5) && m_pSpotLightA)
				{
					m_pSpotLightA->SetActive(m_pSpotLightA->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_6) && m_pSpotLightB)
				{
					m_pSpotLightB->SetActive(m_pSpotLightB->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_7) && m_pSpotLightC)
				{
					m_pSpotLightC->SetActive(m_pSpotLightC->IsActive() ? false : true);
					return;
				}
				
				if (m_pInput->KeyDown(DIK_8) && m_pFlashLight)
				{
					m_pFlashLight->SetActive(m_pFlashLight->IsActive() ? false : true);
					return;
				}		
			}

			// etc
			{
				if (m_pInput->KeyDown(DIK_SPACE))
				{
					bMove = !bMove;
					return;
				}
			}
		}
	}

	void Sandbox::createBaseScene()
	{
		m_pGraphics->SetWindowTitle(L"Opaque and Transparent");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Test World");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 50.0f, -100.0f);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);

		// objects
		{
			// textures
			auto pTexDMC = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/dmc.jpg");
			auto pTexDOKEV = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/dokev.jpeg");
			auto pTexPlane = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Albedo.tga");
			auto pTexPlaneNormal = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Normal.tga");
			auto pTexStone = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/stone01.tga");
			auto pTexNormal = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/normal01.tga");
			auto pTexNoTexture = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/bright-squares.png");

			// meshes
			auto pTriangleModel = Dive::ModelFactory::CreateTriangle(5.0f);
			Dive::GetResourceManager()->AddManualResource(pTriangleModel);

			auto pCubeModel = Dive::ModelFactory::CreateCube(5.0f);
			Dive::GetResourceManager()->AddManualResource(pCubeModel);

			auto pPlaneModel = Dive::ModelFactory::CreatePlane(80, 80);
			Dive::GetResourceManager()->AddManualResource(pPlaneModel);

			// materials
			// 셰이더를 직접 전달하지 않아도 RenderPath에 맞는 Default가 적용되어 있어야 한다.
			auto pTriangleMaterial = Dive::CreateMaterial("TriangleMaterial");
			pTriangleMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexDMC);
			Dive::GetResourceManager()->AddManualResource(pTriangleMaterial);

			auto pCubeMaterial = Dive::CreateMaterial("CubeMaterial");
			pCubeMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexDOKEV);
			Dive::GetResourceManager()->AddManualResource(pCubeMaterial);

			auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
			pPlaneMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexPlane);
			pPlaneMaterial->SetTexture(Dive::eTextureUnitType::Normal, pTexPlaneNormal);
			Dive::GetResourceManager()->AddManualResource(pPlaneMaterial);

			auto pStoneMaterial = Dive::CreateMaterial("StoneMaterial");
			pStoneMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexStone);
			pStoneMaterial->SetTexture(Dive::eTextureUnitType::Normal, pTexNormal);
			Dive::GetResourceManager()->AddManualResource(pStoneMaterial);

			auto pTransparentMaterial = Dive::CreateMaterial("TransparentMaterial");
			pTransparentMaterial->SetDiffuseColor(1.0f, 1.0f, 0.0f, 0.75f);
			//pTransparentMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexNoTexture);
			Dive::GetResourceManager()->AddManualResource(pTransparentMaterial);

			// tirangle gameobject
			m_pTriangle = pActiveScene->CreateGameObject("Triangle");
			m_pTriangle->SetPosition(20.0f, 2.5f, -20.0f);
			auto pTriangleRenderableCom = m_pTriangle->AddComponent<Dive::Renderable>();
			// 모델로 받으면 이 부분이 애매해진다.
			Dive::Mesh* pMesh = pTriangleModel->GetMeshAt();
			pTriangleRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
			pTriangleRenderableCom->SetMaterial(pTriangleMaterial);
			pTriangleModel->SetRootObject(m_pTriangle);

			// cube gameobject
			m_pCube = pActiveScene->CreateGameObject("Cube");
			m_pCube->SetPosition(-20.0f, 2.5f, -20.0f);
			auto pCubeRenderableCom = m_pCube->AddComponent<Dive::Renderable>();
			pMesh = pCubeModel->GetMeshAt();
			pCubeRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
			pCubeRenderableCom->SetMaterial//(pTransparentMaterial);
				(pStoneMaterial);
			pCubeModel->SetRootObject(m_pCube);

			// bottom gameobject
			auto pBottom = pActiveScene->CreateGameObject("Bottom");
			auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
			auto pBottomMesh = pPlaneModel->GetMeshAt();
			pBottomRenderableCom->SetGeometry(pBottomMesh, 0, pBottomMesh->GetVertexCount(), 0, pBottomMesh->GetIndexCount());
			pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			pPlaneModel->SetRootObject(pBottom);

			auto pBallModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf");
			pBallModel->GetRootObject()->SetPosition(20.0f, 7.5f, 0.0f);
					
			auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
			pSphereModel->GetRootObject()->SetPosition(20.0f, 15.0f, -30.0f);
			pSphereModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
			pSphereModel->GetRootObject()->SetParent(pBallModel->GetRootObject());

			auto pBoxModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Cube/Cube_fbx.fbx");
			pBoxModel->GetRootObject()->SetScale(0.1f, 0.1f, 0.1f);
			pBoxModel->GetRootObject()->SetPosition(-20.0f, 0.0f, 20.0f);
			pBoxModel->GetRootObject()->SetRotation(0.0f, -45.0f, 0.0f);
		
			auto pHelmetModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/damaged_helmet/DamagedHelmet.gltf");
			pHelmetModel->GetRootObject()->SetPosition(-20.0f, 20.0f, 20.0f);
			pHelmetModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
			pHelmetModel->GetRootObject()->SetParent(pBoxModel->GetRootObject());

			auto pFlightHelmetModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/flight_helmet/FlightHelmet.gltf");
			pFlightHelmetModel->GetRootObject()->SetScale(30.0f, 30.0f, 30.0f);
		}

		// lights
		{
			// Directional Light
			//m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			//m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			//auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
			//pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
			//pDirLightCom->SetType(Dive::eLightType::Directional);

			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLight = new Dive::DirectionalLight();
			pDirLight->SetColor(1.0f, 1.0f, 1.0f);
			m_pDirLightA->AddComponent<Dive::DvLight>(static_cast<Dive::DvLight*>(pDirLight));

			auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj")->GetRootObject();
			auto pBallModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf")->GetRootObject();
			auto pHelmetModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/damaged_helmet/DamagedHelmet.gltf")->GetRootObject();

			// PointLights
			{
				m_pPointLightA = pActiveScene->CreateGameObject("PointLightA");
				m_pPointLightA->SetPosition(20.0f, 5.0f, -20.0f);
				auto pPointLightCom = new Dive::PointLight();//m_pPointLightA->AddComponent<Dive::Light>();
				//pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);
				m_pPointLightA->AddComponent<Dive::DvLight>(pPointLightCom);
				
				m_pPointLightB = pActiveScene->CreateGameObject("PointLightB");
				m_pPointLightB->SetPosition(0.0f, 5.0f, -20.0f);
				pPointLightCom = new Dive::PointLight();//m_pPointLightB->AddComponent<Dive::Light>();
				//pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);
				m_pPointLightB->AddComponent<Dive::DvLight>(pPointLightCom);

				m_pPointLightC = pActiveScene->CreateGameObject("PointLightC");
				m_pPointLightC->SetPosition(-20.0f, 5.0f, -20.0f);
				pPointLightCom = new Dive::PointLight();//m_pPointLightC->AddComponent<Dive::Light>();
				//pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
				m_pPointLightC->AddComponent<Dive::DvLight>(pPointLightCom);
			}

			// SpotLights
			{
				m_pSpotLightA = pActiveScene->CreateGameObject("SpotLightA");
				m_pSpotLightA->SetPosition(pBallModel->GetPosition().x, pBallModel->GetPosition().y + 10.0f, pBallModel->GetPosition().z);
				m_pSpotLightA->LookAt(pSphereModel);
				m_pSpotLightA->SetParent(pBallModel);
				auto pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 0.0f, 0.0f);
				pSpotLightCom->SetAngles(35.0f, 45.0f);
				m_pSpotLightA->AddComponent<Dive::DvLight>(pSpotLightCom);
				
				m_pSpotLightB = pActiveScene->CreateGameObject("SpotLightB");
				m_pSpotLightB->SetPosition(m_pCube->GetPosition());
				m_pSpotLightB->SetRotation(90.0f, 0.0f, 0.0f);
				m_pSpotLightB->SetParent(m_pCube);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
				pSpotLightCom->SetAngles(55.0f, 65.0f);
				m_pSpotLightB->AddComponent<Dive::DvLight>(pSpotLightCom);

				m_pSpotLightC = pActiveScene->CreateGameObject("SpotLightC");
				m_pSpotLightC->SetPosition(pHelmetModel->GetPosition().x, pHelmetModel->GetPosition().y + 10.0f, pHelmetModel->GetPosition().z);
				m_pSpotLightC->LookAt(pHelmetModel->GetPosition().x, 0.0f, -100.0f);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
				pSpotLightCom->SetAngles(20.0f, 30.0f);
				m_pSpotLightC->AddComponent<Dive::DvLight>(pSpotLightCom);

				m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
				m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
				m_pFlashLight->LookAt(m_pMainCam->GetForward(), m_pMainCam->GetUpward());
				m_pFlashLight->SetParent(m_pMainCam);
				m_pFlashLight->SetActive(false);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f); 
				pSpotLightCom->SetAngles(15.0f, 30.0f);
				m_pFlashLight->AddComponent<Dive::DvLight>(pSpotLightCom);
			}
		}

		m_SceneType = eSceneType::Base;
	}

	void Sandbox::createCarScene()
	{
		m_pGraphics->SetWindowTitle(L"Car");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Car World");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 100.0f, -400.0f);

		// car sceneauto 
		auto pCarModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/toyota_ae86_sprinter_trueno_zenki/scene.gltf");
		m_pCar = pCarModel->GetRootObject();

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pDirLightCom->SetType(Dive::eLightType::Directional);

			m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
			m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
			m_pFlashLight->SetParent(m_pMainCam);
			m_pFlashLight->SetActive(false);
			auto pSpotLightCom = m_pFlashLight->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(1000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pSpotLightCom->SetSpotLightAngles(60.0f, 45.0f);
		}

		m_SceneType = eSceneType::Car;
	}

	void Sandbox::createSponzaScene()
	{
		m_pGraphics->SetWindowTitle(L"Sponza");
	
		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Sponza World");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		m_pMainCam->SetPosition(0.0f, 10.0f, 0.0f);
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		pCamCom->SetFarClipPlane(5000.0f);

		// sponza scene
		Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/sponza/sponza.obj");

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = new Dive::DirectionalLight();
			m_pDirLightA->AddComponent<Dive::DvLight>(pDirLightCom);
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);

			// SpotLight
			auto pSpotLightObj = pActiveScene->CreateGameObject("SpotLight");
			pSpotLightObj->SetPosition(-50.0f, 100.0f, 0.0f);
			//pSpotLightObj->LookAt(50.0f, 100.0f, 0.0f);
			pSpotLightObj->SetRotation(90.0f, 0.0f, 0.0f);
			//pSpotLightObj->SetActive(false);
			auto pSpotLightCom = new Dive::SpotLight();
			pSpotLightObj->AddComponent<Dive::DvLight>(pSpotLightCom);
			pSpotLightCom->SetRange(1000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pSpotLightCom->SetAngles(50.0f, 60.0f);


			// Flash Light
			m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
			m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
			m_pFlashLight->SetParent(m_pMainCam);
			m_pFlashLight->SetActive(false);
			pSpotLightCom = new Dive::SpotLight();
			m_pFlashLight->AddComponent<Dive::DvLight>(pSpotLightCom);
			pSpotLightCom->SetRange(1000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 0.7f);
			pSpotLightCom->SetAngles(10.0f, 30.0f);
		}

		m_SceneType = eSceneType::Sponza;
	}

	void Sandbox::createFieldScene()
	{
		m_pGraphics->SetWindowTitle(L"Field - Shadow Test");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Field");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.65f, 0.65f, 1.0f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 20.0f, -50.0f);
		m_pMainCam->LookAt(0.0f, 10.0f, 0.0f);

		// objects
		{
			// texture
			auto pTexPlane = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Albedo.tga");
			auto pTexPlaneNormal = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Normal.tga");
			
			// meshes
			auto pPlaneModel = Dive::ModelFactory::CreatePlane(100.0f, 100.0f);
			Dive::GetResourceManager()->AddManualResource(pPlaneModel);

			// materials
			auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
			pPlaneMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexPlane);
			pPlaneMaterial->SetTexture(Dive::eTextureUnitType::Normal, pTexPlaneNormal);
			Dive::GetResourceManager()->AddManualResource(pPlaneMaterial);

			// bottom gameobject
			auto pBottom = pActiveScene->CreateGameObject("Bottom");
			auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
			auto pBottomMesh = pPlaneModel->GetMeshAt();
			pBottomRenderableCom->SetGeometry(pBottomMesh, 0, pBottomMesh->GetVertexCount(), 0, pBottomMesh->GetIndexCount());
			pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			pPlaneModel->SetRootObject(pBottom);

			// Models
			auto pTreeModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/tree/GenTree-103_AE3D_03122023-F1.fbx");
			pTreeModel->GetRootObject()->SetPosition(0.0f, 0.0f, 0.0f);
			pTreeModel->GetRootObject()->SetScale(0.02f, 0.02f, 0.02f);

			auto pVampire = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Defeated.fbx");
			pVampire->GetRootObject()->SetPosition(-10.0f, 0.0f, 0.0f);
			pVampire->GetRootObject()->SetScale(0.05f, 0.05f, 0.05f);

			auto pDummy = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Y Bot@Zombie Stumbling.fbx");
			pDummy->GetRootObject()->SetPosition(10.0f, 0.0f, 0.0f);
			pDummy->GetRootObject()->SetScale(0.05f, 0.05f, 0.05f);
		}

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = new Dive::DirectionalLight();
			m_pDirLightA->AddComponent<Dive::DvLight>(pDirLightCom);
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
		
			m_pDummy = pActiveScene->CreateGameObject("Dummy");
			m_pDummy->SetPosition(0.0f, 0.0f, 0.0f);

			// SpotLights
			{
				// MovingSpotLight
				auto pMovingSpotLight = pActiveScene->CreateGameObject("MovingSpotLight");
				pMovingSpotLight->SetPosition(50.0f, 50.0f, -50.0f);
				pMovingSpotLight->LookAt(0.0f, 0.0f, 0.0f);
				pMovingSpotLight->SetParent(m_pDummy);
				pMovingSpotLight->SetActive(true);
				auto pSpotLightCom = new Dive::SpotLight();
				pMovingSpotLight->AddComponent<Dive::DvLight>(pSpotLightCom);
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 0.0f, 1.0f);
				pSpotLightCom->SetAngles(25.0f, 30.0f);
			
				// TopSpotLight
				auto pTopSpotLight = pActiveScene->CreateGameObject("TopSpotLight");
				pTopSpotLight->SetPosition(0.0f, 100.0f, 0.0f);
				pTopSpotLight->SetRotation(90.0f, 0.0f, 0.0f);
				//pTopSpotLight->LookAt(0.0f, 0.0f, 0.0f);
				pTopSpotLight->SetActive(true);
				pSpotLightCom = new Dive::SpotLight();
				pTopSpotLight->AddComponent<Dive::DvLight>(pSpotLightCom);
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
				pSpotLightCom->SetAngles(25.0f, 30.0f);
			}
		}

		m_SceneType = eSceneType::Field;
	}

	void Sandbox::createTextureScene()
	{
		m_pGraphics->SetWindowTitle(L"TextureScene");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Test World");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 0.0f, -10.0f);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);

		// objects
		{
			// textures
			auto pTexDMC = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/dmc.jpg");
			auto pTexDOKEV = Dive::GetResourceManager()->GetResource<Dive::Texture2D>("../../Assets/Textures/dokev.jpeg");

			// models
			auto pDmcPlaneModel = Dive::ModelFactory::CreatePlane(80, 80);
			Dive::GetResourceManager()->AddManualResource(pDmcPlaneModel);

			auto pDokevPlaneModel = Dive::ModelFactory::CreatePlane(80, 80);
			Dive::GetResourceManager()->AddManualResource(pDmcPlaneModel);

			// materials
			auto pDmcMaterial = Dive::CreateMaterial("DmcMaterial");
			pDmcMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexDMC);
			Dive::GetResourceManager()->AddManualResource(pDmcMaterial);

			auto pDokevMaterial = Dive::CreateMaterial("DokevMaterial");
			pDokevMaterial->SetTexture(Dive::eTextureUnitType::Diffuse, pTexDOKEV);
			Dive::GetResourceManager()->AddManualResource(pDokevMaterial);

			// gameobjects
			auto pDmcPlane = pActiveScene->CreateGameObject("DmcPlane");
			pDmcPlane->SetPosition(50, 0.0f, 120.0f);
			pDmcPlane->SetRotation(-90.0f, 0.0f, 0.0f);
			auto pRenderableCom = pDmcPlane->AddComponent<Dive::Renderable>();
			auto pPlaneMesh = pDmcPlaneModel->GetMeshAt();
			pRenderableCom->SetGeometry(pPlaneMesh, 0, pPlaneMesh->GetVertexCount(), 0, pPlaneMesh->GetIndexCount());
			pRenderableCom->SetMaterial(pDokevMaterial);
			pDmcPlaneModel->SetRootObject(pDmcPlane);

			auto pDokevPlane = pActiveScene->CreateGameObject("DokevPlane");
			pDokevPlane->SetPosition(-50, 0.0f, 120.0f);
			pDokevPlane->SetRotation(-90.0f, 0.0f, 0.0f);
			pRenderableCom = pDokevPlane->AddComponent<Dive::Renderable>();
			pPlaneMesh = pDokevPlaneModel->GetMeshAt();
			pRenderableCom->SetGeometry(pPlaneMesh, 0, pPlaneMesh->GetVertexCount(), 0, pPlaneMesh->GetIndexCount());
			pRenderableCom->SetMaterial(pDmcMaterial);
			pDokevPlaneModel->SetRootObject(pDokevPlane);
		}

		// Directional Light
		m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLight");
		m_pDirLightA->SetRotation(0.0f, 0.0f, 0.0f);
		auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
		pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
		pDirLightCom->SetType(Dive::eLightType::Directional);

		m_SceneType = eSceneType::Texture;
	}
}