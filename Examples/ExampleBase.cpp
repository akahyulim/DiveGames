#include "LoadStaticModel.h"

DEFINE_APPLICATION_MAIN(LoadStaticModel::LoadStaticModel)

namespace LoadStaticModel
{
	LoadStaticModel::LoadStaticModel()
		: Dive::DvApplication(L"LoadStaticModel", 1600, 900, false)
		, m_pScene(nullptr)
		, m_pMainCam(nullptr)
		, m_pStaticModel(nullptr)
	{
	}

	LoadStaticModel::~LoadStaticModel()
	{
	}
	
	void LoadStaticModel::OnSetup()
	{
	}
	
	void LoadStaticModel::OnStart()
	{// create scene
		{
			m_pScene = new Dive::DvScene;

			// main camera
			m_pMainCam = m_pScene->CreateGameObject("MainCam");
			auto pCamCom = m_pMainCam->AddComponent<Dive::DvCamera>();
			pCamCom->SetBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
			m_pMainCam->GetTransform()->SetPosition(0.0f, 5.0f, -30.0f);

			// game objects
			{
				auto pTexPlane = Dive::ResourceCache::LoadFromFile<Dive::DvTexture2D>("Assets/Textures/WornWood/WornWood_Albedo.tga");
				
				// shader, material, meshes
				// 현재 관리주체가 없다. ResourceCache가 적절해보인다.
				auto pShader = Dive::DvShader::CreateFromFile("ForwardLight", L"CoreData/Shaders/ForwardLight.hlsl", Dive::eVertexLayout::Static_Model);
				auto pPlaneMesh = Dive::MeshFactory::CreatePlane(50, 50);
				auto pPlaneMaterial = Dive::DvMaterial::Create(pShader);
				pPlaneMaterial->SetDiffuseColor(0.1f, 0.1f, 0.1f, 1.0f);
				pPlaneMaterial->SetTexture(Dive::eTextureUnit::Diffuse, pTexPlane);

				// bottom gameobject
				auto pBottom = m_pScene->CreateGameObject("Bottom");
				auto pBottomRenderableCom = pBottom->AddComponent<Dive::Renderable>();
				pBottomRenderableCom->SetMesh(pPlaneMesh);
				pBottomRenderableCom->SetMaterial(pPlaneMaterial);
			}

			// lights
			{
				auto pDirLight = m_pScene->CreateGameObject("DirectionalLight");
				auto pDirLightCom = pDirLight->AddComponent<Dive::DvLight>();
				pDirLightCom->SetColor(1.0f, 1.0f, 1.0f);
				pDirLightCom->SetType(Dive::DvLight::eLightType::Directional);
				pDirLightCom->SetDir(1.0f, -1.0f, 1.0f);
			}
		}

		// setup renderLayer
		{
			auto pViewScreen = new Dive::ViewScreen(m_pScene, m_pMainCam->GetComponent<Dive::DvCamera>());
			Dive::DvRenderer::SetViewScreen(0, pViewScreen);
		}

		// subscribe events
		{
			SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(HandleUpdate));
		}
	}
	
	void LoadStaticModel::OnStop()
	{
		DV_DELETE(m_pScene);
	}
	
	void LoadStaticModel::HandleUpdate(const Dive::Event& e)
	{
		static float angle = 0.0f;
		angle += 0.1f;
		if (angle >= 360.0f)
			angle = 0.0f;

		if(m_pStaticModel)
			m_pStaticModel->GetTransform()->SetRotation(DirectX::XMFLOAT3(0.0f, -angle, 0.0f));
		
		{
			using namespace Dive;

			float deltaTime = static_cast<float>(Timer::GetDeltaTimeSec());

			auto pTransform = m_pMainCam->GetTransform();

			static float m_MoveSpeed = 10.0f;
			static float boost = 1.0f;
			if (DvInput::KeyPress(DIK_LSHIFT))
				boost = 5.0f;
			else
				boost = 1.0f;

			if (DvInput::KeyPress(DIK_UP))
			{
				pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
			}
			if (DvInput::KeyPress(DIK_DOWN))
			{
				pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
			}
			if (DvInput::KeyPress(DIK_LEFT))
			{
				pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
			}
			if (DvInput::KeyPress(DIK_RIGHT))
			{
				pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
			}

			if (DvInput::MouseButtonPress(1))
			{
				// 현재 키 입력과 마우스 이동이 동시에 작동하지 않는다.
				const DirectX::XMFLOAT2& delta = DvInput::GetMouseMoveDelta();
				pTransform->Rotate(delta.y * 0.1f, delta.x * 0.1f, 0.0f);

				if (DvInput::KeyPress(DIK_W))
				{
					pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
				}
				if (DvInput::KeyPress(DIK_S))
				{
					pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
				}
				if (DvInput::KeyPress(DIK_A))
				{
					pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
				}
				if (DvInput::KeyPress(DIK_D))
				{
					pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
				}

				if (DvInput::KeyPress(DIK_Q))
				{
					pTransform->Translate(0.0f, m_MoveSpeed * deltaTime * boost, 0.0f);
				}
				if (DvInput::KeyPress(DIK_E))
				{
					pTransform->Translate(0.0f, -m_MoveSpeed * deltaTime * boost, 0.0f);
				}
			}
		}
	}
}