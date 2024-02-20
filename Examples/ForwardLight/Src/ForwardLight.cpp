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
		, m_pScene(nullptr)
		, m_pMainCam(nullptr)
		, m_pCube(nullptr)
		, m_pTriangle(nullptr)
		, m_pSphere(nullptr)
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

		// Title�� ����ü�� ���Խ��Ѿ� �ϳ�...?
		Dive::Graphics::SetWindowTitle(L"Forward Light");
		m_WindowWidth = 1600;
		m_WindowHeight = 900;
	}

	void ForwardLight::OnStart()
	{
		// create scene
		{
			m_pScene = new Dive::Scene;

			// main camera
			m_pMainCam = m_pScene->CreateGameObject("MainCam");
			auto pCamCom = m_pMainCam->AddComponent<Dive::Camera>();
			pCamCom->SetBackgroundColor(0.1f, 0.1f, 0.1f, 1.0f);
			m_pMainCam->GetTransform()->SetPosition(0.0f, 5.0f, -30.0f);

			// game objects
			{
				// textures
				auto pTexDMC = Dive::ResourceCache::LoadFromFile<Dive::Texture2D>("Assets/Textures/dmc.jpg");
				auto pTexDOKEV = Dive::ResourceCache::LoadFromFile<Dive::Texture2D>("Assets/Textures/dokev.jpeg");
				auto pTexPlane = Dive::ResourceCache::LoadFromFile<Dive::Texture2D>("Assets/Textures/WornWood/WornWood_Albedo.tga");
				auto pTexStone = Dive::ResourceCache::LoadFromFile<Dive::Texture2D>("Assets/Textures/Stone01.tga");

				// shader, material, meshes
				// �ϴ� AddManualResource�� ĳ�ÿ� �����ߴ�.
				// ���� �ε尡 �ƴϱ⿡ LoadFromFile()�� ����� �� ���� �����̴�.
				auto pShader = Dive::Shader::CreateFromFile("ForwardLight", L"CoreData/Shaders/ForwardLight.hlsl", Dive::eVertexLayout::Static_Model);
				Dive::ResourceCache::AddManualResource<Dive::Shader>(pShader);
				auto pTriangleMesh = Dive::MeshFactory::CreateTriangle(5.0f);
				Dive::ResourceCache::AddManualResource<Dive::Mesh>(pTriangleMesh);
				// Material�� �̸��� �� �ָ��ϴ�.
				// Default(Legacy)�� Pbs�� �����ϴ°� �´� �� �ѵ�...
				// �� �ʿ��ϴٸ� Renderable���� Mesh�� �̸��� ������ �� �ִ�.
				// ������ AddMenualResource() ������ �ָ�������.
				auto pTriangleMaterial = Dive::Material::Create("TriangleMaterial", pShader);
				pTriangleMaterial->SetDiffuseColor(1.0f, 1.0f, 0.0f, 1.0f);
				pTriangleMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDMC);
				Dive::ResourceCache::AddManualResource<Dive::Material>(pTriangleMaterial);
				auto pCubeMesh = Dive::MeshFactory::CreateCube(5.0f);
				Dive::ResourceCache::AddManualResource<Dive::Mesh>(pCubeMesh);
				auto pCubeMaterial = Dive::Material::Create("CubeMaterial", pShader);
				pCubeMaterial->SetDiffuseColor(0.0f, 1.0f, 1.0f, 1.0f);
				pCubeMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexDOKEV);
				Dive::ResourceCache::AddManualResource<Dive::Material>(pCubeMaterial);
				auto pSphereMesh = Dive::MeshFactory::CreateSphere(5.0f);
				Dive::ResourceCache::AddManualResource<Dive::Mesh>(pSphereMesh);
				auto pSphereMaterial = Dive::Material::Create("SphereMaterial", pShader);
				pSphereMaterial->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
				//pSphereMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexStone);
				Dive::ResourceCache::AddManualResource<Dive::Material>(pSphereMaterial);
				auto pPlaneMesh = Dive::MeshFactory::CreatePlane(50, 50);
				Dive::ResourceCache::AddManualResource<Dive::Mesh>(pPlaneMesh);
				auto pPlaneMaterial = Dive::Material::Create("PlaneMaterial", pShader);
				pPlaneMaterial->SetDiffuseColor(0.1f, 0.1f, 0.1f, 1.0f);
				pPlaneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexPlane);
				Dive::ResourceCache::AddManualResource<Dive::Material>(pPlaneMaterial);

				// tirangle gameobject
				m_pTriangle = m_pScene->CreateGameObject("Triangle");
				m_pTriangle->GetTransform()->SetPosition(5.0f, 5.0f, -10.0f);
				auto pTriangleRenderableCom = m_pTriangle->AddComponent<Dive::Renderable>();
				pTriangleRenderableCom->SetMesh(pTriangleMesh);
				pTriangleRenderableCom->SetMaterial(pTriangleMaterial);

				// cube gameobject
				m_pCube = m_pScene->CreateGameObject("Cube");
				m_pCube->GetTransform()->SetPosition(-15.0f, 2.5f, 0.0f);
				auto pCubeRenderableCom = m_pCube->AddComponent<Dive::Renderable>();
				pCubeRenderableCom->SetMesh(pCubeMesh);
				pCubeRenderableCom->SetMaterial(pCubeMaterial);

				// sphere gameObject
				m_pSphere = m_pScene->CreateGameObject("Sphere");
				m_pSphere->GetTransform()->SetPosition(0.0f, 5.0f, 10.0f);
				auto pCSphereRenderableCom = m_pSphere->AddComponent<Dive::Renderable>();
				pCSphereRenderableCom->SetMesh(pSphereMesh);
				pCSphereRenderableCom->SetMaterial(pSphereMaterial);

				// bottom gameobject
				auto pBottom = m_pScene->CreateGameObject("Bottom");
				auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
				pBottomRenderableCom->SetMesh(pPlaneMesh);
				pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			}

			// lights
			{
				// Directional Light
				m_pDirLightA = m_pScene->CreateGameObject("DirectionalLightA");
				auto pDirLightCom = m_pDirLightA->AddComponent<Dive::Light>();
				pDirLightCom->SetColor(0.9f, 0.9f, 0.9f);
				pDirLightCom->SetType(Dive::eLightType::Directional);
				pDirLightCom->SetLookAt(1.0f, -1.0f, 1.0f);
				
				// PointLights
				{
					m_pPointLightA = m_pScene->CreateGameObject("PointLightA");
					m_pPointLightA->GetTransform()->SetPosition(0.0f, 5.0f, -15.0f);
					auto pPointLightCom = m_pPointLightA->AddComponent<Dive::Light>();
					pPointLightCom->SetType(Dive::eLightType::Point);
					pPointLightCom->SetRange(30.0f);
					pPointLightCom->SetColor(1.0f, 0.0f, 0.0f);

					m_pPointLightB = m_pScene->CreateGameObject("PointLightB");
					m_pPointLightB->GetTransform()->SetPosition(-10.0f, 5.0f, 15.0f);
					pPointLightCom = m_pPointLightB->AddComponent<Dive::Light>();
					pPointLightCom->SetType(Dive::eLightType::Point);
					pPointLightCom->SetRange(30.0f);
					pPointLightCom->SetColor(0.0f, 1.0f, 0.0f);

					m_pPointLightC = m_pScene->CreateGameObject("PointLightC");
					m_pPointLightC->GetTransform()->SetPosition(10.0f, 5.0f, 15.0f);
					pPointLightCom = m_pPointLightC->AddComponent<Dive::Light>();
					pPointLightCom->SetType(Dive::eLightType::Point);
					pPointLightCom->SetRange(30.0f);
					pPointLightCom->SetColor(0.0f, 0.0f, 1.0f);
				}
				
				// SpotLights
				{
					m_pSpotLightA = m_pScene->CreateGameObject("SpotLightA");
					m_pSpotLightA->GetTransform()->SetPosition(0.0f, 20.0f, -30.0f);
					auto pSpotLightCom = m_pSpotLightA->AddComponent<Dive::Light>();
					pSpotLightCom->SetType(Dive::eLightType::Spot);
					pSpotLightCom->SetRange(60.0f);
					pSpotLightCom->SetColor(1.0f, 1.0f, 0.0f);
					pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
					pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

					m_pSpotLightB = m_pScene->CreateGameObject("SpotLightB");
					m_pSpotLightB->GetTransform()->SetPosition(30.0f, 20.0f, 0.0f);
					pSpotLightCom = m_pSpotLightB->AddComponent<Dive::Light>();
					pSpotLightCom->SetType(Dive::eLightType::Spot);
					pSpotLightCom->SetRange(60.0f);
					pSpotLightCom->SetColor(0.0f, 1.0f, 1.0f);
					pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
					pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);

					m_pSpotLightC = m_pScene->CreateGameObject("SpotLightC");
					m_pSpotLightC->GetTransform()->SetPosition(-30.0f, 20.0f, 0.0f);
					pSpotLightCom = m_pSpotLightC->AddComponent<Dive::Light>();
					pSpotLightCom->SetType(Dive::eLightType::Spot);
					pSpotLightCom->SetRange(60.0f);
					pSpotLightCom->SetColor(1.0f, 0.0f, 1.0f);
					pSpotLightCom->SetLookAt(0.0f, 10.0f, 0.0f);
					pSpotLightCom->SetSpotLightAngles(30.0f, 25.0f);
				}
			}
		}

		// setup renderLayer
		{
			auto pViewScreen = new Dive::ViewScreen(m_pScene, m_pMainCam->GetComponent<Dive::Camera>());
			Dive::Renderer::SetViewScreen(0, pViewScreen);
		}

		// subscribe events
		{
			SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(HandleUpdate));
		}
	}
	
	void ForwardLight::OnStop()
	{
		DV_DELETE(m_pScene);
	}

	void ForwardLight::HandleUpdate(const Dive::Event& e)
	{
		static float angle = 0.0f;
		angle += 0.1f;
		if (angle >= 360.0f)
			angle = 0.0f;

		m_pCube->GetTransform()->SetRotation(DirectX::XMFLOAT3(0.0f, -angle, 0.0f));
		m_pTriangle->GetTransform()->SetRotation(DirectX::XMFLOAT3(0.0f, 0.0f, angle));
		m_pSphere->GetTransform()->SetRotation(DirectX::XMFLOAT3(0.0f, angle, 0.0f));

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
}