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
				pDrawable->SetModel(GetModel("Triangle"));
				pDrawable->SetMaterial(GetMaterial("Default"));

				DV_LOG_CLIENT_TRACE("Triangle GameObject�� �����Ͽ����ϴ�.");
			}

			// Quad
			{
				auto pQuad = m_pScene->CreateGameObject("Quad");
				auto pTransform = pQuad->GetComponent<Dive::Transform>();
				// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
				pTransform->SetPosition(DirectX::XMFLOAT3(2.0f, 0.0f, 3.0f));
				pTransform->SetRotation(0.0f, 0.0f, 45.0f);
				auto pDrawable = pQuad->AddComponent<Dive::Drawable>();
				pDrawable->SetModel(GetModel("Quad"));
				pDrawable->SetMaterial(GetMaterial("Default"));

				DV_LOG_CLIENT_TRACE("Quad GameObject�� �����Ͽ����ϴ�.");
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

				DV_LOG_CLIENT_TRACE("Cube Model�� �����Ͽ����ϴ�.");

				return pCubeModel;
			}
		}

		return nullptr;
	}

	// Technique�� ShaderDefine�� ��� �����ؾ� �ǵ��� ��������,
	// Material�� ���� Technique�� Set�ϴ��� �����ϰ� �˾ƺ���.
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
				// �����δ� �̸��� ����Ǿ�� �Ѵ�. urho3d�� Graphics���� Load�� �� ��θ� ���� �߰��Ѵ�.
				pPass->SetVertexShader("CoreData/Shaders/color.hlsl");
				pPass->SetPixelShader("CoreData/Shaders/color.hlsl");
				// �� �κ��� �߸��Ǿ���.
				// ������ Input Define�� ShaderVariation�� SetDefine���� �����ؾ� �Ѵ�.
				// Path���� �߰��ϴ� Define�� Main ���ο��� ����ϴ� �͵��̴�.

				// ���ۿ��� ���������� InputLayout���� �������� ������ ��� �ɱ�?
				// ���� VertexBuffer�� Shader�� ���� hash���θ� Ȯ���ϱ� ������
				// �׽�Ʈ�� �Ұ����ϴ�.
				// => ������ urho�� VERTEXCOLOR�� Tech�� ����� ���Ҵ�.
				// ��, VertexBuffer�� �����Ѵ� �ص� ������� ���� �� �־�� �Ѵ�...
				// => hash �񱳱����� �ּ�ó���ϴ� �� ����ȴ�.
				// ����.. �� �κк��� �� �� �˾ƺ���.
				// InputLayout�� VertexType�� ���� �����Ǿ����� Ȯ���Ͽ���.
				pPass->SetVertexShaderDefines("VERTEXCOLOR");
				pPass->SetPixelShaderDefines("VERTEXCOLOR");
				pCache->AddManualResource(pDefaultTech);

				// Material ����
				auto pDefaultMat = new Dive::Material(m_pContext);
				pDefaultMat->SetName("DefaultMat");
				pDefaultMat->SetTechnique(pDefaultTech);
				pCache->AddManualResource(pDefaultMat);

				DV_LOG_CLIENT_TRACE("Default Material�� �����Ͽ����ϴ�.");

				return pDefaultMat;
			}
		}

		return nullptr;
	}
}