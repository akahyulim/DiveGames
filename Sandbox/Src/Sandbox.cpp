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
				pDrawable->SetModel(GetModel("Triangle"));
				pDrawable->SetMaterial(GetMaterial("Default"));

				DV_LOG_CLIENT_TRACE("Triangle GameObject를 생성하였습니다.");
			}

			// Quad
			{
				auto pQuad = m_pScene->CreateGameObject("Quad");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// 현재 위치를 먼저 변경해 놓았다. Drawable::Update()를 할 수 있는 방법이 없기 때문이다.
				pTransform->SetPosition(DirectX::XMFLOAT3(2.0f, 0.0f, 3.0f));
				pTransform->SetRotation(0.0f, 0.0f, 45.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(GetModel("Quad"));
				pDrawable->SetMaterial(GetMaterial("Default"));

				DV_LOG_CLIENT_TRACE("Quad GameObject를 생성하였습니다.");
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

	Dive::Model* Sandbox::GetModel(const std::string& name)
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

	// Technique의 ShaderDefine을 어떻게 설정해야 의도에 부합할지,
	// Material은 언제 Technique을 Set하는지 생각하고 알아보자.
	Dive::Material* Sandbox::GetMaterial(const std::string& name)
	{
		auto pCache = GetSubsystem<Dive::ResourceCache>();
		auto pMaterial = pCache->GetExistingResource<Dive::Material>(name);
		if (pMaterial)
			return pMaterial;
		else
		{
			if (name == "Default")
			{
				auto pDefaultTech = new Dive::Technique(m_pContext);
				pDefaultTech->SetName("DefaultTech");
				auto pPass = pDefaultTech->CreatePass("base");
				// 실제로는 이름만 저장되어야 한다. urho3d는 Graphics에서 Load할 때 경로를 직접 추가한다.
				pPass->SetVertexShader("CoreData/Shaders/color.hlsl");
				pPass->SetPixelShader("CoreData/Shaders/color.hlsl");
				// 이 부분은 잘못되었다.
				// 실제로 Input Define는 ShaderVariation의 SetDefine으로 전달해야 한다.
				// Path에서 추가하는 Define은 Main 내부에서 사용하는 것들이다.

				// 버퍼에는 존재하지만 InputLayout에는 포함하지 않으면 어떻게 될까?
				// 현재 VertexBuffer와 Shader가 동일 hash여부를 확인하기 때문에
				// 테스트가 불가능하다.
				// => 실제로 urho는 VERTEXCOLOR를 Tech에 기술해 놓았다.
				// 즉, VertexBuffer에 존재한다 해도 사용하지 않을 수 있어야 한다...
				// => hash 비교구문을 주석처리하니 잘 적용된다.
				// 흐음.. 이 부분부터 좀 더 알아보자.
				// InputLayout은 VertexType에 맞춰 생성되었음을 확인하였다.
				pPass->SetVertexShaderDefines("VERTEXCOLOR");
				pPass->SetPixelShaderDefines("VERTEXCOLOR");
				pCache->AddManualResource(pDefaultTech);

				// Material 구성
				auto pDefaultMat = new Dive::Material(m_pContext);
				pDefaultMat->SetName("DefaultMat");
				pDefaultMat->SetTechnique(pDefaultTech);
				pCache->AddManualResource(pDefaultMat);

				DV_LOG_CLIENT_TRACE("Default Material을 생성하였습니다.");

				return pDefaultMat;
			}
		}

		return nullptr;
	}
}