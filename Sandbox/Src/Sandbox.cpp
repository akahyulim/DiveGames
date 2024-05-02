#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

// forward lights 적용
// material, mesh, shader를 resourceCache에서 관리
// gameObjects update - rotate
// input 적용
namespace Sandbox
{
	Sandbox::Sandbox()
		: Dive::Application()
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
		// create scene
		{
			//createBaseScene();
			//createCarScene();
			createSponzaScene();
			//createShadowScene();
		}

		// setup renderLayer
		{
			// ViewScreen에서 Forward, Deferred를 설정한다.
			auto pViewScreen = new Dive::ViewScreen(m_pMainCam->GetComponent<Dive::Camera>());//, Dive::eRenderPath::Deferred);
			Dive::Renderer::SetViewScreen(0, pViewScreen);
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
		using namespace Dive;
		float deltaTime = static_cast<float>(Timer::GetDeltaTimeSec());

		static bool bMove = false;

		static float bSlow = 0;
		if (Input::KeyPress(DIK_LSHIFT))
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

				const auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf");
				if (pBallModel)
					pBallModel->GetRootObject()->Rotate(0.0f, 5.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);

				const auto pBoxModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/Cube/Cube_fbx.fbx");
				if (pBoxModel)
					pBoxModel->GetRootObject()->Rotate(0.0f, -25.00f * deltaTime * bSlow, 0.0f);
			}
			else if (m_SceneType == eSceneType::Car)
			{
				m_pCar->Rotate(0.0f, 10.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);
			}
			else if (m_SceneType == eSceneType::Shadow)
			{
				if(m_pDirLightA)
					m_pDirLightA->Rotate(0.0f, 30.0f * deltaTime * bSlow, 0.0f, Dive::eSpace::World);

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
			static float moveSpeed = (Dive::SceneManager::GetActiveScene()->GetName() == "Test World") ? 10.0f : 100.0f;

			// move controll
			{
				if (Input::KeyPress(DIK_UP))
				{
					m_pMainCam->Rotate(-rotateSpeed * deltaTime, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_DOWN))
				{
					m_pMainCam->Rotate(rotateSpeed * deltaTime, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_LEFT))
				{
					m_pMainCam->Rotate(0.0f, -rotateSpeed * deltaTime, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_RIGHT))
				{
					m_pMainCam->Rotate(0.0f, rotateSpeed * deltaTime, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_W))
				{
					m_pMainCam->Translate(0.0f, 0.0f, moveSpeed * deltaTime * bSlow, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_S))
				{
					m_pMainCam->Translate(0.0f, 0.0f, -moveSpeed * deltaTime * bSlow, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_A))
				{
					m_pMainCam->Translate(-moveSpeed * deltaTime * bSlow, 0.0f, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_D))
				{
					m_pMainCam->Translate(moveSpeed * deltaTime * bSlow, 0.0f, 0.0f, Dive::eSpace::Self);
				}

				if (Input::KeyPress(DIK_Q))
				{
					m_pMainCam->Translate(0.0f, moveSpeed * deltaTime * bSlow, 0.0f, Dive::eSpace::Self);
				}
				if (Input::KeyPress(DIK_E))
				{
					m_pMainCam->Translate(0.0f, -moveSpeed * deltaTime * bSlow, 0.0f, Dive::eSpace::Self);
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
				if (Input::KeyDown(DIK_1) && m_pDirLightA)
				{
					m_pDirLightA->SetActive(m_pDirLightA->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_2) && m_pPointLightA)
				{
					m_pPointLightA->SetActive(m_pPointLightA->IsActive() ? false : true);
					return;
				}
		
				if (Input::KeyDown(DIK_3) && m_pPointLightB)
				{
					m_pPointLightB->SetActive(m_pPointLightB->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_4) && m_pPointLightC)
				{
					m_pPointLightC->SetActive(m_pPointLightC->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_5) && m_pSpotLightA)
				{
					m_pSpotLightA->SetActive(m_pSpotLightA->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_6) && m_pSpotLightB)
				{
					m_pSpotLightB->SetActive(m_pSpotLightB->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_7) && m_pSpotLightC)
				{
					m_pSpotLightC->SetActive(m_pSpotLightC->IsActive() ? false : true);
					return;
				}
				
				if (Input::KeyDown(DIK_8) && m_pFlashLight)
				{
					m_pFlashLight->SetActive(m_pFlashLight->IsActive() ? false : true);
					return;
				}		
			}

			// etc
			{
				if (Input::KeyDown(DIK_SPACE))
				{
					bMove = !bMove;
					return;
				}
			}
		}
	}

	void Sandbox::createBaseScene()
	{
		Dive::Scene* pActiveScene = Dive::SceneManager::CreateScene("Test World");
		Dive::SceneManager::SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 50.0f, -100.0f);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);

		// objects
		{
			// textures
			auto pTexDMC = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/dmc.jpg");
			auto pTexDOKEV = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/dokev.jpeg");
			auto pTexPlane = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Albedo.tga");
			auto pTexPlaneNormal = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Normal.tga");
			auto pTexStone = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/stone01.tga");
			auto pTexNormal = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/normal01.tga");
			auto pTexNoTexture = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/bright-squares.png");

			// shader
			auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("../../Assets/Shaders/ForwardLight.hlsl");
			//auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("../../Assets/Shaders/Deferred.hlsl");
			pShader->CreateInputLayout(Dive::eVertexLayout::Static_Model);

			// meshes
			auto pTriangleModel = Dive::ModelFactory::CreateTriangle(5.0f);
			Dive::ResourceManager::AddManualResource(pTriangleModel);

			auto pCubeModel = Dive::ModelFactory::CreateCube(5.0f);
			Dive::ResourceManager::AddManualResource(pCubeModel);

			auto pPlaneModel = Dive::ModelFactory::CreatePlane(80, 80);
			Dive::ResourceManager::AddManualResource(pPlaneModel);

			// materials
			// 셰이더를 직접 전달하지 않아도 RenderPath에 맞는 Default가 적용되어 있어야 한다.
			auto pTriangleMaterial = Dive::CreateMaterial("TriangleMaterial");
			pTriangleMaterial->SetShader(pShader);
			pTriangleMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDMC);
			Dive::ResourceManager::AddManualResource(pTriangleMaterial);

			auto pCubeMaterial = Dive::CreateMaterial("CubeMaterial");
			pCubeMaterial->SetShader(pShader);
			pCubeMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDOKEV);
			Dive::ResourceManager::AddManualResource(pCubeMaterial);

			auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
			pPlaneMaterial->SetShader(pShader);
			pPlaneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexPlane);
			pPlaneMaterial->SetTexture(Dive::eTextureUnit::Normal, pTexPlaneNormal);
			Dive::ResourceManager::AddManualResource(pPlaneMaterial);

			auto pStoneMaterial = Dive::CreateMaterial("StoneMaterial");
			pStoneMaterial->SetShader(pShader);
			pStoneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexStone);
			pStoneMaterial->SetTexture(Dive::eTextureUnit::Normal, pTexNormal);
			Dive::ResourceManager::AddManualResource(pStoneMaterial);

			auto pTransparentMaterial = Dive::CreateMaterial("TransparentMaterial");
			pTransparentMaterial->SetShader(pShader);
			pTransparentMaterial->SetDiffuseColor(1.0f, 1.0f, 0.0f, 0.75f);
			//pTransparentMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexNoTexture);
			Dive::ResourceManager::AddManualResource(pTransparentMaterial);

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
			pCubeRenderableCom->SetMaterial(pTransparentMaterial);//pStoneMaterial);
			pCubeModel->SetRootObject(m_pCube);

			// bottom gameobject
			auto pBottom = pActiveScene->CreateGameObject("Bottom");
			auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
			auto pBottomMesh = pPlaneModel->GetMeshAt();
			pBottomRenderableCom->SetGeometry(pBottomMesh, 0, pBottomMesh->GetVertexCount(), 0, pBottomMesh->GetIndexCount());
			pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			pPlaneModel->SetRootObject(pBottom);

			auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf");
			pBallModel->GetRootObject()->SetPosition(20.0f, 7.5f, 0.0f);
					
			auto pSphereModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
			pSphereModel->GetRootObject()->SetPosition(20.0f, 15.0f, -30.0f);
			pSphereModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
			pSphereModel->GetRootObject()->SetParent(pBallModel->GetRootObject());

			auto pBoxModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/Cube/Cube_fbx.fbx");
			pBoxModel->GetRootObject()->SetScale(0.1f, 0.1f, 0.1f);
			pBoxModel->GetRootObject()->SetPosition(-20.0f, 0.0f, 20.0f);
			pBoxModel->GetRootObject()->SetRotation(0.0f, -45.0f, 0.0f);
		
			auto pHelmetModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/damaged_helmet/DamagedHelmet.gltf");
			pHelmetModel->GetRootObject()->SetPosition(-20.0f, 20.0f, 20.0f);
			pHelmetModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
			pHelmetModel->GetRootObject()->SetParent(pBoxModel->GetRootObject());

			auto pFlightHelmetModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/flight_helmet/FlightHelmet.gltf");
			pFlightHelmetModel->GetRootObject()->SetScale(30.0f, 30.0f, 30.0f);
		}

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pDirLightCom->SetType(Dive::eLightType::Directional);

			auto pSphereModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/Sphere.obj")->GetRootObject();
			auto pBallModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/material_ball_in_3d-coat/Scene.gltf")->GetRootObject();
			auto pHelmetModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/damaged_helmet/DamagedHelmet.gltf")->GetRootObject();

			// PointLights
			{
				m_pPointLightA = pActiveScene->CreateGameObject("PointLightA");
				m_pPointLightA->SetPosition(0.0f, 5.0f, -20.0f);
				auto pPointLightCom = m_pPointLightA->AddComponent<Dive::Light>();
				pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(100.0f);
				pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);

				m_pPointLightB = pActiveScene->CreateGameObject("PointLightB");
				m_pPointLightB->SetPosition(-20.0f, 5.0f, 20.0f);
				pPointLightCom = m_pPointLightB->AddComponent<Dive::Light>();
				pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(30.0f);
				pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);

				m_pPointLightC = pActiveScene->CreateGameObject("PointLightC");
				m_pPointLightC->SetPosition(20.0f, 5.0f, 20.0f);
				pPointLightCom = m_pPointLightC->AddComponent<Dive::Light>();
				pPointLightCom->SetType(Dive::eLightType::Point);
				pPointLightCom->SetRange(30.0f);
				pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
			}

			// SpotLights
			{
				m_pSpotLightA = pActiveScene->CreateGameObject("SpotLightA");
				m_pSpotLightA->SetPosition(pBallModel->GetPosition().x, pBallModel->GetPosition().y + 10.0f, pBallModel->GetPosition().z);
				m_pSpotLightA->LookAt(pSphereModel);
				m_pSpotLightA->SetParent(pBallModel);
				auto pSpotLightCom = m_pSpotLightA->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(1000.0f);
				pSpotLightCom->SetColor(1.0f, 0.0f, 0.0f);
				pSpotLightCom->SetSpotLightAngles(45.0f, 35.0f);

				m_pSpotLightB = pActiveScene->CreateGameObject("SpotLightB");
				m_pSpotLightB->SetPosition(m_pCube->GetPosition());
				m_pSpotLightB->SetRotation(90.0f, 0.0f, 0.0f);
				m_pSpotLightB->SetParent(m_pCube);
				pSpotLightCom = m_pSpotLightB->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(60.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
				pSpotLightCom->SetSpotLightAngles(65.0f, 55.0f);

				m_pSpotLightC = pActiveScene->CreateGameObject("SpotLightC");
				m_pSpotLightC->SetPosition(pHelmetModel->GetPosition().x, pHelmetModel->GetPosition().y + 10.0f, pHelmetModel->GetPosition().z);
				m_pSpotLightC->LookAt(pHelmetModel->GetPosition().x, 0.0f, -100.0f);
				pSpotLightCom = m_pSpotLightC->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(1000.0f);
				pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
				pSpotLightCom->SetSpotLightAngles(30.0f, 20.0f);
				//pSpotLightCom->SetLightShadows(Dive::eLightShadows::None);

				m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
				m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
				m_pFlashLight->LookAt(m_pMainCam->GetForward(), m_pMainCam->GetUpward());
				m_pFlashLight->SetParent(m_pMainCam);
				m_pFlashLight->SetActive(false);
				pSpotLightCom = m_pFlashLight->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(1000.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f); 
				pSpotLightCom->SetSpotLightAngles(30.0f, 15.0f);
			}
		}

		m_SceneType = eSceneType::Base;
	}

	void Sandbox::createCarScene()
	{
		Dive::Scene* pActiveScene = Dive::SceneManager::CreateScene("Car World");
		Dive::SceneManager::SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 100.0f, -400.0f);

		// shader
		auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("../../Assets/Shaders/ForwardLight.hlsl");
		pShader->CreateInputLayout(Dive::eVertexLayout::Static_Model);

		// car sceneauto 
		auto pCarModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/toyota_ae86_sprinter_trueno_zenki/scene.gltf");
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
		Dive::Scene* pActiveScene = Dive::SceneManager::CreateScene("Sponza World");
		Dive::SceneManager::SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		m_pMainCam->SetPosition(0.0f, 10.0f, 0.0f);
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		pCamCom->SetFarClipPlane(5000.0f);

		// shader
		auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("../../Assets/Shaders/ForwardLight.hlsl");
		pShader->CreateInputLayout(Dive::eVertexLayout::Static_Model);

		// sponza scene
		Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/sponza/sponza.obj");

		// lights
		{
			// Directional Light
			m_pDirLightA = pActiveScene->CreateGameObject("DirectionalLightA");
			m_pDirLightA->SetRotation(45.0f, -45.0f, 45.0f);
			auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
			pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pDirLightCom->SetType(Dive::eLightType::Directional);

			// Flash Light
			m_pFlashLight = pActiveScene->CreateGameObject("FlashLight");
			m_pFlashLight->SetPosition(m_pMainCam->GetPosition());
			m_pFlashLight->SetParent(m_pMainCam);
			m_pFlashLight->SetActive(false);
			auto pSpotLightCom = m_pFlashLight->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(1000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 0.7f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 10.0f);

			// SpotLight left
			auto pLeftSpotLight = pActiveScene->CreateGameObject("LeftSpotLight");
			pLeftSpotLight->SetPosition(-50.0f, 100.0f, 0.0f);
			pLeftSpotLight->LookAt(50.0f, 100.0f, 0.0f);
			pLeftSpotLight->SetActive(true);
			pSpotLightCom = pLeftSpotLight->AddComponent<Dive::Light>();
			pSpotLightCom->SetType(Dive::eLightType::Spot);
			pSpotLightCom->SetRange(10000.0f);
			pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
			pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
		}

		m_SceneType = eSceneType::Sponza;
	}

	void Sandbox::createShadowScene()
	{
		Dive::Scene* pActiveScene = Dive::SceneManager::CreateScene("Shadow Test World");
		Dive::SceneManager::SetActiveScene(pActiveScene);

		// main camera
		m_pMainCam = pActiveScene->CreateGameObject("MainCam");
		auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
		pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pMainCam->SetPosition(0.0f, 30.0f, -50.0f);
		m_pMainCam->LookAt(0.0f, 0.0f, 0.0f);

		// objects
		{
			// shader
			auto pShader = Dive::ResourceManager::GetResource<Dive::Shader>("../../Assets/Shaders/ForwardLight.hlsl");
			pShader->CreateInputLayout(Dive::eVertexLayout::Static_Model);

			// texture
			auto pTexPlane = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Albedo.tga");
			auto pTexPlaneNormal = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/WornWood/WornWood_Normal.tga");
			auto pTexStone = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/stone01.tga");
			auto pTexNormal = Dive::ResourceManager::GetResource<Dive::Texture2D>("../../Assets/Textures/normal01.tga");

			// meshes
			auto pCubeModel = Dive::ModelFactory::CreateCube(10.0f);
			Dive::ResourceManager::AddManualResource(pCubeModel);

			auto pPlaneModel = Dive::ModelFactory::CreatePlane(50.0f, 50.0f);
			Dive::ResourceManager::AddManualResource(pPlaneModel);

			// materials
			auto pCubeMaterial = Dive::CreateMaterial("CubeMaterial");
			pCubeMaterial->SetShader(pShader);
			pCubeMaterial->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			Dive::ResourceManager::AddManualResource(pCubeMaterial);

			auto pPlaneMaterial = Dive::CreateMaterial("PlaneMaterial");
			pPlaneMaterial->SetShader(pShader);
			pPlaneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexPlane);
			pPlaneMaterial->SetTexture(Dive::eTextureUnit::Normal, pTexPlaneNormal);
			Dive::ResourceManager::AddManualResource(pPlaneMaterial);

			auto pStoneMaterial = Dive::CreateMaterial("StoneMaterial");
			pStoneMaterial->SetShader(pShader);
			pStoneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexStone);
			pStoneMaterial->SetTexture(Dive::eTextureUnit::Normal, pTexNormal);
			Dive::ResourceManager::AddManualResource(pStoneMaterial);

			// cube gameobject
			m_pCube = pActiveScene->CreateGameObject("Cube");
			m_pCube->SetPosition(-10.0f, 5.0f, 0.0f);
			auto pCubeRenderableCom = m_pCube->AddComponent<Dive::Renderable>();
			auto pMesh = pCubeModel->GetMeshAt();
			pCubeRenderableCom->SetGeometry(pMesh, 0, pMesh->GetVertexCount(), 0, pMesh->GetIndexCount());
			pCubeRenderableCom->SetMaterial(pStoneMaterial);
			pCubeModel->SetRootObject(m_pCube);

			// bottom gameobject
			auto pBottom = pActiveScene->CreateGameObject("Bottom");
			auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
			auto pBottomMesh = pPlaneModel->GetMeshAt();
			pBottomRenderableCom->SetGeometry(pBottomMesh, 0, pBottomMesh->GetVertexCount(), 0, pBottomMesh->GetIndexCount());
			pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			pPlaneModel->SetRootObject(pBottom);

			// Sphere
			auto pSphereModel = Dive::ResourceManager::GetResource<Dive::Model>("../../Assets/Models/Sphere.obj");
			pSphereModel->GetRootObject()->SetPosition(10.0f, 5.0f, 0.0f);
			pSphereModel->GetRootObject()->SetScale(5.0f, 5.0f, 5.0f);
		}

		// lights
		{
			m_pDummy = pActiveScene->CreateGameObject("Dummy");
			m_pDummy->SetPosition(0.0f, 0.0f, 0.0f);

			// SpotLights
			{
				// MovingSpotLight
				auto pMovingSpotLight = pActiveScene->CreateGameObject("MovingSpotLight");
				pMovingSpotLight->SetPosition(30.0f, 30.0f, -30.0f);
				pMovingSpotLight->LookAt(0.0f, 0.0f, 0.0f);
				pMovingSpotLight->SetParent(m_pDummy);
				pMovingSpotLight->SetActive(true);
				auto pSpotLightCom = pMovingSpotLight->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(5000.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
				pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
				//pSpotLightCom->SetLightShadows(Dive::eLightShadows::None);

				// TopSpotLight
				auto pTopSpotLight = pActiveScene->CreateGameObject("TopSpotLight");
				pTopSpotLight->SetPosition(0.0f, 100.0f, 0.0f);
				pTopSpotLight->LookAt(0.0f, 0.0f, 0.0f);
				pTopSpotLight->SetActive(true);
				pSpotLightCom = pTopSpotLight->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(1000.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
				pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

				// left
				auto pLeftSpotLight = pActiveScene->CreateGameObject("LeftSpotLight");
				pLeftSpotLight->SetPosition(-50.0f, 30.0f, 50.0f);
				pLeftSpotLight->LookAt(0.0f, 0.0f, 0.0f);
				pLeftSpotLight->SetActive(true);
				pSpotLightCom = pLeftSpotLight->AddComponent<Dive::Light>();
				pSpotLightCom->SetType(Dive::eLightType::Spot);
				pSpotLightCom->SetRange(1000.0f);
				pSpotLightCom->SetColor(1.0f, 1.0f, 1.0f);
				pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
			}
		}

		m_SceneType = eSceneType::Shadow;
	}
}