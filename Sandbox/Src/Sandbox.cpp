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
		DV_LOG_CLIENT_TRACE("Sandbox �Ҹ� �Ϸ�");
	}

	// ���� ���� ���� ȣ��
	void Sandbox::Setup()
	{
		// ini ���� ���� �о���� ���� ����.
		m_EngineParams.Title = L"Sandbox";
		m_EngineParams.Width = 1280;
		m_EngineParams.Height = 760;
		m_EngineParams.PositionX = m_EngineParams.GetCenterPositionX();
		m_EngineParams.PositionY = m_EngineParams.GetCenterPositionY();
		m_EngineParams.bReSizable = true;// false;

		// resource paths?
	}

	// ���� �ʱ�ȭ �� ȣ��
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Start ȣ��");

		m_pScene = new Dive::Scene(m_pContext);
		m_pScene->SetName("Sandbox");
		auto pViewport = new Dive::Viewport(m_pContext);
		pViewport->SetScene(m_pScene);
		GetSubsystem<Dive::Renderer>()->SetViewport(0, pViewport);

		DV_LOG_CLIENT_TRACE("Sandbox Scene�� �����Ͽ����ϴ�.");

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
				// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
				pTransform->SetPosition(DirectX::XMFLOAT3(-5.0f, 0.0f, 5.0f));
				auto pDrawable = pTriangle->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Triangle"));
				pDrawable->SetMaterial(pRenderer->GetDefaultMaterial());

				DV_LOG_CLIENT_TRACE("Triangle GameObject�� �����Ͽ����ϴ�.");
			}

			// Quad
			{
				auto pQuad = m_pScene->CreateGameObject("Quad");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
				pTransform->SetPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f));
				//pTransform->SetRotation(0.0f, 0.0f, 45.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Quad"));
				pDrawable->SetMaterial(pRenderer->GetDefaultMaterial());	// �������� �ݵ�� Material�� ������ �Ѵ�.

				DV_LOG_CLIENT_TRACE("Quad GameObject�� �����Ͽ����ϴ�.");
			}
			
			// Cube
			{
				auto pQuad = m_pScene->CreateGameObject("Cube");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
				pTransform->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
				pTransform->SetRotation(45.0f, 45.0f, 0.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(getModel("Cube"));
				Dive::Material* pMat = new Dive::Material(m_pContext);
				pMat->SetName("CubeMat");
				pCache->AddManualResource(pMat);
				pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
				pDrawable->SetMaterial(pMat);

				DV_LOG_CLIENT_TRACE("Cube GameObject�� �����Ͽ����ϴ�.");
			}
		}

		// test
		{
		}
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Stop ȣ��");

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
				// �ᱹ �̷��� ���´� �Ұ����� �� ���δ�.
				// Geometry Type�� ���� Elements�� �̹� ������ ���¶� ������ �� �ִ�.
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

				DV_LOG_CLIENT_TRACE("Triangle Model�� �����Ͽ����ϴ�.");

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

				DV_LOG_CLIENT_TRACE("Quad Model�� �����Ͽ����ϴ�.");

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

				DV_LOG_CLIENT_TRACE("Cube Model�� �����Ͽ����ϴ�.");

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
				// global�� pass���� �����ϴ� �� �ϴ�.
				// ���� ���̴��� �ִٸ� �׶��� ���� �����Ѵ�.
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