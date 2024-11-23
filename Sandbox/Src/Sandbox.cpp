#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox()
	{
	}

	Sandbox::~Sandbox()
	{
	}

	// 실제로는 직접 설정 구조체를 채우거나 파일로 읽어야 한다.
	// 엔진의 초기화는 이 다음에 자동으로 호출되는 게 맞다.
	void Sandbox::OnSetup()
	{
		Dive::GEngine->Initialize(L"SANDBOX", 800, 600);
	}

	void Sandbox::OnStart()
	{
	}

	void Sandbox::OnTick()
	{
	}

	void Sandbox::OnStop()
	{
	}

	void Sandbox::HandleUpdate(const Dive::Event& e)
	{
	}

	/*
	Sandbox::Sandbox()
		: m_pGraphics(nullptr)
		, m_pInput(nullptr)
		, m_pMainCam(nullptr)
		, m_CamDefaultPos(0.0f, 10.0f, -10.0f)
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
		, m_SceneType(eSceneType::Base)		// World을 런타임에서 바꾸고 싶다.
	{
		//DV_REGISTER_LOGGER(Sandbox);
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
				DV_LOG(Sandbox, critical, "잘못된 씬을 선택하였습니다. 시스템을 종료합니다.");
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

	void Sandbox::OnTick()
	{
		m_pEngine->RunFrame();
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

				if (m_pInput->KeyDown(DIK_9))
				{
					m_pMainCam->SetPosition(m_CamDefaultPos);
					m_pMainCam->LookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), m_pMainCam->GetUpward());
					return;
				}
				if (m_pInput->KeyDown(DIK_0))
				{
					m_pMainCam->SetPosition(0.0f, 160.0f, 0.0f);
					m_pMainCam->LookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
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
		m_pGraphics->SetWindowTitle(L"Base: Opaque Objects");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Base");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_CamDefaultPos = DirectX::XMFLOAT3(0.0f, 50.0f, -160.0f);
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
			//(0.65f, 0.65f, 1.0f, 1.0f);
		m_pMainCam->SetPosition(m_CamDefaultPos);
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

			auto pPlaneModel = Dive::ModelFactory::CreatePlane(160, 160);
			Dive::GetResourceManager()->AddManualResource(pPlaneModel);

			// materials
			// 셰이더를 직접 전달하지 않아도 RenderPath에 맞는 Default가 적용되어 있어야 한다.
			auto pTriangleMaterial = Dive::CreateMaterial("TriangleMaterial");
			pTriangleMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexDMC);
			Dive::GetResourceManager()->AddManualResource(pTriangleMaterial);

			auto pCubeMaterial = Dive::CreateMaterial("CubeMaterial");
			pCubeMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexDOKEV);
			Dive::GetResourceManager()->AddManualResource(pCubeMaterial);

			auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
			pPlaneMaterial->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			//pPlaneMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexPlane);
			pPlaneMaterial->SetDvTexture(Dive::eTextureUnitType::Normal, pTexPlaneNormal);
			Dive::GetResourceManager()->AddManualResource(pPlaneMaterial);

			auto pStoneMaterial = Dive::CreateMaterial("StoneMaterial");
			pStoneMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexStone);
			pStoneMaterial->SetDvTexture(Dive::eTextureUnitType::Normal, pTexNormal);
			Dive::GetResourceManager()->AddManualResource(pStoneMaterial);

			auto pTransparentMaterial = Dive::CreateMaterial("TransparentMaterial");
			pTransparentMaterial->SetDiffuseColor(1.0f, 1.0f, 0.0f, 0.75f);
			//pTransparentMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexNoTexture);
			Dive::GetResourceManager()->AddManualResource(pTransparentMaterial);

			// tirangle gameobject
			m_pTriangle = pActiveScene->CreateGameObject("Triangle");
			m_pTriangle->SetPosition(15.0f, 5.0f, 30.0f);
			auto pTriangleRenderableCom = m_pTriangle->AddComponent<Dive::Renderable>();
			// 모델로 받으면 이 부분이 애매해진다.
			Dive::Mesh* pMesh = pTriangleModel->GetMeshAt();
			pTriangleRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
			pTriangleRenderableCom->SetMaterial(pTriangleMaterial);
			pTriangleModel->SetRootObject(m_pTriangle);

			// cube gameobject
			m_pCube = pActiveScene->CreateGameObject("Cube");
			m_pCube->SetPosition(-50.0f, 2.5f, 25.0f);
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
			//pPlaneModel->SetRootObject(pBottom);	// 이게 필요할까?

			// wall gameobject
			auto pWall = pActiveScene->CreateGameObject("Wall");
			auto pWallRenderableCom = pWall->AddComponent<Dive::Renderable>();
			auto pWallMesh = pPlaneModel->GetMeshAt();
			pWallRenderableCom->SetGeometry(pWallMesh, 0, pWallMesh->GetVertexCount(), 0, pWallMesh->GetIndexCount());
			//pWallRenderableCom->SetMaterial(pPlaneMaterial);
			//pPlaneModel->SetRootObject(pWall);
			pWall->SetRotation(-90.0f, 0.0f, 0.0f);
			pWall->SetPosition(0.0f, 80.0f, 80.0f);

			// left wall object
			auto pLeftWall = pActiveScene->CreateGameObject("Bottom");
			auto pLeftWallRenderableCom = pLeftWall->AddComponent<Dive::Renderable>();
			auto pLeftWallMesh = pPlaneModel->GetMeshAt();
			pLeftWallRenderableCom->SetGeometry(pLeftWallMesh, 0, pLeftWallMesh->GetVertexCount(), 0, pLeftWallMesh->GetIndexCount());
			//pLeftWallRenderableCom->SetMaterial(pPlaneMaterial);
			pLeftWall->SetPosition(-80.0f, 80.0f, 0.0f);
			pLeftWall->SetRotation(0.0f, 0.0f, -90.0f);		

			// right wall object
			auto pRightWall = pActiveScene->CreateGameObject("Bottom");
			auto pRightWallRenderableCom = pRightWall->AddComponent<Dive::Renderable>();
			auto pRightWallMesh = pPlaneModel->GetMeshAt();
			pRightWallRenderableCom->SetGeometry(pRightWallMesh, 0, pRightWallMesh->GetVertexCount(), 0, pRightWallMesh->GetIndexCount());
			//pRightWallRenderableCom->SetMaterial(pPlaneMaterial);
			pRightWall->SetPosition(80.0f, 80.0f, 0.0f);
			pRightWall->SetRotation(0.0f, 0.0f, 90.0f);

			auto pBallModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf");
			pBallModel->GetRootObject()->SetPosition(0.0f, 7.5f, 0.0f);
					
			auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
			pSphereModel->GetRootObject()->SetPosition(0.0f, 20.0f, -50.0f);
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
			pFlightHelmetModel->GetRootObject()->SetPosition(40.0f, 0.0f, 0.0f);
		}

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, 45.0f, 0.0f);
			auto pDirLight = new Dive::DirectionalLight();
			pDirLight->SetColor(0.3f, 0.3f, 0.3f);
				//1.0f, 1.0f, 1.0f);
			m_pDirLightA->AddComponent<Dive::Light>(pDirLight);
			//m_pDirLightA->SetActive(false);
			pDirLight->InitializeCascadedMatrixSet(m_pMainCam);	// 좀 애매허다.
			pDirLight->SetShadowEnabled(true);

			auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj")->GetRootObject();
			auto pBallModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf")->GetRootObject();
			auto pHelmetModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/damaged_helmet/DamagedHelmet.gltf")->GetRootObject();

			// PointLights
			{
				m_pPointLightA = pActiveScene->CreateGameObject("PointLightA");
				m_pPointLightA->SetPosition(-60.0f, 25.0f, 30.0f);
				auto pPointLightCom = new Dive::PointLight();
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
				pPointLightCom->SetShadowEnabled(true);
				m_pPointLightA->AddComponent<Dive::Light>(pPointLightCom);
				m_pPointLightA->SetActive(false);
				
				m_pPointLightB = pActiveScene->CreateGameObject("PointLightB");
				m_pPointLightB->SetPosition(60.0f, 25.0f, 30.0f);
				pPointLightCom = new Dive::PointLight();
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);
				pPointLightCom->SetShadowEnabled(true);
				m_pPointLightB->AddComponent<Dive::Light>(pPointLightCom);
				m_pPointLightB->SetActive(false);

				m_pPointLightC = pActiveScene->CreateGameObject("PointLightC");
				//m_pPointLightC->SetPosition(60.0f, 50.0f, 60.0f);
				auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
				m_pPointLightC->SetPosition(pSphereModel->GetRootObject()->GetPosition());
				m_pPointLightC->SetParent(pSphereModel->GetRootObject());
				pPointLightCom = new Dive::PointLight();
				pPointLightCom->SetRange(50.0f);
				pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);
				pPointLightCom->SetShadowEnabled(true);
				m_pPointLightC->AddComponent<Dive::Light>(pPointLightCom);
				m_pPointLightC->SetActive(false);
			}

			// SpotLights
			{
				// red
				m_pSpotLightA = pActiveScene->CreateGameObject("SpotLightA");
				m_pSpotLightA->SetPosition(pBallModel->GetPosition().x, pBallModel->GetPosition().y, pBallModel->GetPosition().z);
				m_pSpotLightA->LookAt(pSphereModel);
				m_pSpotLightA->SetParent(pBallModel);
				auto pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(150.0f);
				pSpotLightCom->SetColor(1.0f, 0.0f, 0.0f);
				pSpotLightCom->SetAngles(20.0f, 30.0f);
				pSpotLightCom->SetShadowEnabled(true);
				m_pSpotLightA->AddComponent<Dive::Light>(pSpotLightCom);
				m_pSpotLightA->SetActive(false);
				
				// yellow
				m_pSpotLightB = pActiveScene->CreateGameObject("SpotLightB");
				//m_pSpotLightB->SetPosition(m_pCube->GetPosition());
				//m_pSpotLightB->SetRotation(90.0f, 0.0f, 0.0f);
				//m_pSpotLightB->LookAt(m_pSpotLightB->GetPosition().x, 0.0f, m_pSpotLightB->GetPosition().z, 0.0f, 0.0f, 1.0f);
				//m_pSpotLightB->SetParent(m_pCube);
				m_pSpotLightB->SetPosition(-50.0f, 0.0f, -40.0f);
				m_pSpotLightB->LookAt(-50.0f, 40.0f, 80.0f);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(200.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
				pSpotLightCom->SetAngles(20.0f, 30.0f);
				pSpotLightCom->SetShadowEnabled(true);
				m_pSpotLightB->AddComponent<Dive::Light>(pSpotLightCom);
				m_pSpotLightB->SetActive(false);

				// green-blue
				m_pSpotLightC = pActiveScene->CreateGameObject("SpotLightC");
				m_pSpotLightC->SetPosition(50.0f, 0.0f, -40.0f);
				m_pSpotLightC->LookAt(50.0f, 40.0f, 80.0f);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(200.0f);
				pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
				pSpotLightCom->SetAngles(20.0f, 30.0f);
				pSpotLightCom->SetShadowEnabled(true);
				m_pSpotLightC->AddComponent<Dive::Light>(pSpotLightCom);
				m_pSpotLightC->SetActive(false);

				m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
				m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
				m_pFlashLight->LookAt(m_pMainCam->GetForward(), m_pMainCam->GetUpward());
				m_pFlashLight->SetParent(m_pMainCam);
				m_pFlashLight->SetActive(false);
				pSpotLightCom = new Dive::SpotLight();
				pSpotLightCom->SetRange(500.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f); 
				pSpotLightCom->SetAngles(10.0f, 15.0f);
				m_pFlashLight->AddComponent<Dive::Light>(pSpotLightCom);
				pSpotLightCom->SetShadowEnabled(true);
			}
		}

		m_SceneType = eSceneType::Base;

		// test
		//auto pRenderTex = new Dive::RenderTexture();
		//pRenderTex->SetSize(1024, 1024);
		//pRenderTex->SetArraySize(3);
		//pRenderTex->SetDepthFormat(DXGI_FORMAT_R32_TYPELESS);
		//pRenderTex->Create();
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
			auto pDirLightCom = new Dive::DirectionalLight(); 
			m_pDirLightA->AddComponent<Dive::Light>(pDirLightCom);
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);

			m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
			m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
			m_pFlashLight->SetParent(m_pMainCam);
			m_pFlashLight->SetActive(false);
			auto pSpotLightCom = new Dive::SpotLight(); 
			m_pFlashLight->AddComponent<Dive::Light>(pSpotLightCom);
			pSpotLightCom->SetRange(1000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pSpotLightCom->SetAngles(60.0f, 45.0f);
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
		pCamCom->SetBackgroundColor//(0.0f, 0.0f, 0.0f, 1.0f);
		(0.65f, 0.65f, 1.0f, 1.0f);
		pCamCom->SetFarClipPlane(5000.0f);

		// sponza scene
		Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/sponza/sponza.obj");

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = new Dive::DirectionalLight();
			m_pDirLightA->AddComponent<Dive::Light>(pDirLightCom);
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);

			// SpotLight
			{
				m_pSpotLightA = pActiveScene->CreateGameObject("SpotLight");
				m_pSpotLightA->SetPosition(0.0f, 55.0f, 20.0f);
				//pSpotLightObj->LookAt(100.0f, 250.0f, 0.0f);
				m_pSpotLightA->SetRotation(0.0f, 10.0f, 0.0f);
				auto pSpotLightCom = new Dive::SpotLight();
				m_pSpotLightA->AddComponent<Dive::Light>(pSpotLightCom);
				pSpotLightCom->SetRange(95.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
				pSpotLightCom->SetAngles(35.0f, 45.0f);
				pSpotLightCom->SetShadowEnabled(true);
			}
			// Flash Light
			{
				m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
				m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
				m_pFlashLight->SetParent(m_pMainCam);
				m_pFlashLight->SetActive(false);
				auto pSpotLightCom = new Dive::SpotLight();
				m_pFlashLight->AddComponent<Dive::Light>(pSpotLightCom);
				pSpotLightCom->SetRange(95.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 0.5f);
				pSpotLightCom->SetAngles(35.0f, 45.0f);
			}
		}

		m_SceneType = eSceneType::Sponza;
	}

	void Sandbox::createFieldScene()
	{
		m_pGraphics->SetWindowTitle(L"Field - Shadow Test");

		Dive::Scene* pActiveScene = Dive::GetSceneManager()->CreateScene("Field");
		Dive::GetSceneManager()->SetActiveScene(pActiveScene);

		// main camera
		m_CamDefaultPos = { 71.0f, 41.0f, 71.0f };
		//= { 0.0f, 20.0f, -50.0f };
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		 //0.65f, 0.65f, 1.0f, 1.0f);
		pCamCom->SetFarClipPlane(500.0f);
		m_pMainCam->SetPosition(m_CamDefaultPos);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);

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
			pPlaneMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexPlane);
			pPlaneMaterial->SetDvTexture(Dive::eTextureUnitType::Normal, pTexPlaneNormal);
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

			m_pDummy = pActiveScene->CreateGameObject("Dummy");
			m_pDummy->SetPosition(0.0f, 0.0f, 0.0f);

		}

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = new Dive::DirectionalLight();
			m_pDirLightA->AddComponent<Dive::Light>(pDirLightCom);
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);

			// SpotLights
			{
				// MovingSpotLight
				{
					m_pSpotLightA = pActiveScene->CreateGameObject("MovingSpotLight");
					m_pSpotLightA->SetPosition(-30.0f, 10.0f, -30.0f);
					m_pSpotLightA->LookAt(0.0f, 0.0f, 0.0f);
					m_pSpotLightA->SetParent(m_pDummy);
					m_pSpotLightA->SetActive(false);
					auto pSpotLightCom = new Dive::SpotLight();
					m_pSpotLightA->AddComponent<Dive::Light>(pSpotLightCom);
					pSpotLightCom->SetRange(100.0f);
					pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
					pSpotLightCom->SetAngles(20.0f, 35.0f);
					pSpotLightCom->SetShadowEnabled(true);

					auto pSphereModel = Dive::GetResourceManager()->GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
					pSphereModel->GetRootObject()->SetPosition(m_pSpotLightA->GetPosition());
					pSphereModel->GetRootObject()->SetParent(m_pDummy);
				}

				// TopSpotLight
				{
					m_pSpotLightB = pActiveScene->CreateGameObject("TopSpotLight");
					m_pSpotLightB->SetPosition(0.0f, 55.0f, 20.0f);
					//m_pSpotLightB->SetRotation(90.0f, 0.0f, 0.0f);
					m_pSpotLightB->LookAt(0.0f, 10.0f, 0.0f);
					m_pSpotLightB->SetActive(true);
					auto pSpotLightCom = new Dive::SpotLight();
					m_pSpotLightB->AddComponent<Dive::Light>(pSpotLightCom);
					pSpotLightCom->SetRange(95.0f);
					pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
					pSpotLightCom->SetAngles(35.0f, 45.0f);
					pSpotLightCom->SetShadowEnabled(true);
				}

				// flash light
				{
					m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
					//m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
					m_pFlashLight->SetTransform(m_pMainCam->GetWorldTransform());
					m_pFlashLight->SetParent(m_pMainCam);
					m_pFlashLight->SetActive(false);
					auto pSpotLightCom = new Dive::SpotLight();
					pSpotLightCom->SetRange(500.0f);
					pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
					pSpotLightCom->SetAngles(10.0f, 20.0f);
					m_pFlashLight->AddComponent<Dive::Light>(pSpotLightCom);
				}
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
			pDmcMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexDMC);
			Dive::GetResourceManager()->AddManualResource(pDmcMaterial);

			auto pDokevMaterial = Dive::CreateMaterial("DokevMaterial");
			pDokevMaterial->SetDvTexture(Dive::eTextureUnitType::Diffuse, pTexDOKEV);
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
		auto pDirLightCom = new Dive::DirectionalLight();
		m_pDirLightA->AddComponent<Dive::Light>(pDirLightCom);
		pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);

		m_SceneType = eSceneType::Texture;
	}
	*/
}