#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox(Dive::Context* pContext)
		: Dive::Application(pContext),
		m_pScene(nullptr)
	{
	}

	Sandbox::~Sandbox()
	{
		DV_LOG_CLIENT_TRACE("Sandbox 소멸 완료");
	}

	// 엔진 생성 전에 호출
	void Sandbox::Setup()
	{
		// ini 파일 등을 읽어오는 편이 낫다.
		m_EngineParams.Title = L"Sandbox";
		m_EngineParams.Width = 1280;
		m_EngineParams.Height = 760;
		m_EngineParams.PositionX = m_EngineParams.GetCenterPositionX();
		m_EngineParams.PositionY = m_EngineParams.GetCenterPositionY();
		m_EngineParams.bReSizable = true;// false;

		// resource paths?
	}

	// 엔진 초기화 후 호출
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Start 호출");

		m_pScene = new Dive::Scene(m_pContext);
		m_pScene->SetName("Sandbox");
		auto pViewport = new Dive::Viewport(m_pContext);
		pViewport->SetScene(m_pScene);
		GetSubsystem<Dive::Renderer>()->SetViewport(0, pViewport);

		DV_LOG_CLIENT_TRACE("Sandbox Scene을 생성하였습니다.");

		auto pRenderer = GetSubsystem<Dive::Renderer>();

		auto pCache = GetSubsystem<Dive::ResourceCache>();

		// renderPath
		{

		}

		// Add GameObjects
		{
			// Triangle
			{
				auto pTriangle = m_pScene->CreateGameObject("Triangle");
				auto pTransform = pTriangle->GetComponent<Dive::Transform>();
				// 현재 위치를 먼저 변경해 놓았다. Drawable::Update()를 할 수 있는 방법이 없기 때문이다.
				pTransform->SetPosition(DirectX::XMFLOAT3(-5.0f, 0.0f, 5.0f));
				auto pDrawable = pTriangle->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Triangle"));
				pDrawable->SetMaterial(pRenderer->GetDefaultMaterial());

				DV_LOG_CLIENT_TRACE("Triangle GameObject를 생성하였습니다.");
			}

			// Quad
			{
				auto pQuad = m_pScene->CreateGameObject("Quad");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// 현재 위치를 먼저 변경해 놓았다. Drawable::Update()를 할 수 있는 방법이 없기 때문이다.
				pTransform->SetPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f));
				//pTransform->SetRotation(0.0f, 0.0f, 45.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Quad"));
				pDrawable->SetMaterial(pRenderer->GetDefaultMaterial());	// 아직까진 반드시 Material을 가져야 한다.

				DV_LOG_CLIENT_TRACE("Quad GameObject를 생성하였습니다.");
			}
			
			// Cube
			{
				auto pQuad = m_pScene->CreateGameObject("Cube");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// 현재 위치를 먼저 변경해 놓았다. Drawable::Update()를 할 수 있는 방법이 없기 때문이다.
				pTransform->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
				pTransform->SetRotation(45.0f, 45.0f, 0.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Cube"));
				Dive::Material* pMat = new Dive::Material(m_pContext);
				pMat->SetName("CubeMat");
				pCache->AddManualResource(pMat);
				pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
				pDrawable->SetMaterial(pMat);

				DV_LOG_CLIENT_TRACE("Cube GameObject를 생성하였습니다.");
			}
		}

		// test
		{
		}
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Stop 호출");

		DV_DELETE(m_pScene);
	}

	Dive::Model* Sandbox::getModel(const std::string& name)
	{
		auto pCache = GetSubsystem<Dive::ResourceCache>();
		auto pModel = pCache->GetExistingResource<Dive::Model>(name);
		if (pModel)
			return pModel;
		else
		{
			if (name == "Triangle")
			{
				// 결국 이러한 형태는 불가능한 듯 보인다.
				// Geometry Type에 맞춰 Elements는 이미 구성된 상태라 유추할 수 있다.
				std::vector<Dive::VertexElement> elements;
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);

				std::vector<float> vertices =
				{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };

				Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
				pVb->SetSize(3, elements);
				pVb->SetData(static_cast<void*>(vertices.data()));

				std::vector<unsigned short> indices = { 0, 1, 2 };
				Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
				pIb->Create<unsigned short>(indices);

				Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
				pMesh->SetVertexBuffer(pVb);
				pMesh->SetIndexBuffer(pIb);

				Dive::Model* pTriangleModel = new Dive::Model(m_pContext);
				pTriangleModel->SetName("TriangleModel");
				pTriangleModel->SetNumMeshes(1);
				pTriangleModel->SetMesh(0, pMesh);
				pCache->AddManualResource(pTriangleModel);

				DV_LOG_CLIENT_TRACE("Triangle Model을 생성하였습니다.");

				return pTriangleModel;
			}
			else if (name == "Quad")
			{
				std::vector<Dive::VertexElement> elements;
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);

				std::vector<float> vertices =
				{
					-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
					-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
					1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
					1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
				};

				Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
				pVb->SetSize(4, elements);
				pVb->SetData(static_cast<void*>(vertices.data()));

				std::vector<unsigned short> indices = { 0, 1, 2, 0, 2, 3 };
				Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
				pIb->Create<unsigned short>(indices);

				Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
				pMesh->SetVertexBuffer(pVb);
				pMesh->SetIndexBuffer(pIb);

				Dive::Model* pQuadModel = new Dive::Model(m_pContext);
				pQuadModel->SetName("QuadModel");
				pQuadModel->SetNumMeshes(1);
				pQuadModel->SetMesh(0, pMesh);
				pCache->AddManualResource(pQuadModel);

				DV_LOG_CLIENT_TRACE("Quad Model을 생성하였습니다.");

				return pQuadModel;
			}
			else if (name == "Cube")
			{
				std::vector<Dive::VertexElement> elements;
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);

				std::vector<float> vertices =
				{
					// front
					-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,

					// bottom
					-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,

					// back
					-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
					-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,

					// top
					-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
					-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,

					// right
					0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,

					// left
					-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
					-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f
				};

				Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
				pVb->SetSize(24, elements);
				pVb->SetData(static_cast<void*>(vertices.data()));

				std::vector<unsigned short> indices = { 
					0, 2, 3, 0, 1, 2,
					4, 6, 7, 4, 5, 6,
					9, 11, 10, 9, 8, 11,
					12, 14, 15, 12, 13, 14, 
					16, 18, 19, 16, 17, 18,
					20, 22, 23, 20, 21, 22
				};
				Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
				pIb->Create<unsigned short>(indices);

				Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
				pMesh->SetVertexBuffer(pVb);
				pMesh->SetIndexBuffer(pIb);

				Dive::Model* pCubeModel = new Dive::Model(m_pContext);
				pCubeModel->SetName("CubeModel");
				pCubeModel->SetNumMeshes(1);
				pCubeModel->SetMesh(0, pMesh);
				pCache->AddManualResource(pCubeModel);

				DV_LOG_CLIENT_TRACE("Cube Model을 생성하였습니다.");

				return pCubeModel;
			}
		}

		return nullptr;
	}

	Dive::Technique* Sandbox::getTechnique(const std::string& name)
	{
		auto pCache = GetSubsystem<Dive::ResourceCache>();
		Dive::Technique* pTechnique = pCache->GetExistingResource<Dive::Technique>(name);
		if (pTechnique)
			return pTechnique;
		else
		{
			if (name == "BasicVColorUnlitAlpha")
			{
				pTechnique = new Dive::Technique(m_pContext);
				pTechnique->SetName("BasicVColorUnlitAlpha");
				Dive::Pass* pPass = pTechnique->CreatePass("alpha");
				// global은 pass마다 저장하는 듯 하다.
				// 개별 셰이더가 있다면 그때는 따로 저장한다.
				pPass->SetVertexShader("CoreData/Shaders/Basic.hlsl");
				pPass->SetVertexShaderDefines("VERTEXCOLOR");	// + DIFFMAP
				pPass->SetPixelShader("CoreData/Shaders/Basic.hlsl");
				pPass->SetPixelShaderDefines("VERTEXCOLOR");	// + DIFFMAP
				// depthwrite = false
				// blend = alpha
				pCache->AddManualResource(pTechnique);

				return pTechnique;
			}
		}

		return nullptr;
	}
}