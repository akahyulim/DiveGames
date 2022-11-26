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
	}

	// ���� ���� ���� ȣ��
	void Sandbox::Setup()
	{
		// ini ���� ���� �о���� ���� ����.
		m_EngineParameters.title = L"Sandbox";
		m_EngineParameters.width = 1280;
		m_EngineParameters.height = 760;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = true;// false;

		// resource paths?
	}

	// ���� �ʱ�ȭ �� ȣ��
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Start ȣ��");

		m_pScene = new Dive::Scene(m_pContext);
		auto pViewport = new Dive::Viewport(m_pContext);
		pViewport->SetScene(m_pScene);
		GetSubsystem<Dive::Renderer>()->SetViewport(0, pViewport);

		auto pCache = GetSubsystem<Dive::ResourceCache>();

		// renderPath
		{

		}

		// model: ���� ���� Ȯ��!
		{
			// Model ����
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
			
			// �̰� ����Ϸ��� Shader�� Define�� �����ؾ� �Ѵ�.
			std::vector<float> posVertices =
			{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f };
			
			std::vector<float> vertices =
				{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };
			
			Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
			pVb->SetSize(3, elements);
			pVb->SetData(static_cast<void*>(vertices.data()));
			
			std::vector<unsigned short> indices = { 0, 1, 2 };
			Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
			pIb->Create<unsigned short>(indices);

			Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
			pMesh->SetNumVertexBuffers(1);
			pMesh->SetVertexBuffer(0, pVb);
			pMesh->SetIndexBuffer(pIb);
			pMesh->SetDrawRange(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 0, 3);

			Dive::Model* pTriangleModel = new Dive::Model(m_pContext);
			pTriangleModel->SetName("TriangleModel");
			pTriangleModel->SetNumMeshes(1);
			pTriangleModel->SetMesh(0, pMesh);
			pCache->AddManualResource(pTriangleModel);

			DV_LOG_CLIENT_TRACE("Triangle Model ������ �����Ͽ����ϴ�.");

			// Technique ����
			auto pDefaultTech = new Dive::Technique(m_pContext);
			pDefaultTech->SetName("DefaultTech");
			auto pPass = pDefaultTech->CreatePass("first");
			pPass->SetVertexShader("CoreData/Shaders/color.hlsl");
			pPass->SetPixelShader("CoreData/Shaders/color.hlsl");
			pCache->AddManualResource(pDefaultTech);

			DV_LOG_CLIENT_TRACE("Default Technique ������ �����Ͽ����ϴ�.");

			// Material ����
			auto pDefaultMat = new Dive::Material(m_pContext);
			pDefaultMat->SetName("DefaultMat");
			pDefaultMat->SetTechnique(pDefaultTech);
			pCache->AddManualResource(pDefaultMat);

			DV_LOG_CLIENT_TRACE("Default Material ������ �����Ͽ����ϴ�.");

			// Drawable�� Model�� Material�� �߰�
			auto pTriangle = m_pScene->CreateGameObject("Triangle");
			auto pDrawable = pTriangle->CreateComponent<Dive::Drawable>();
			pDrawable->SetModel(pTriangleModel);
			pDrawable->SetMaterial(pDefaultMat);

			DV_LOG_CLIENT_TRACE("Triangle Drawable ������ �����Ͽ����ϴ�.");
		}

		// test create shader
		{
			auto pShader = pCache->GetResource<Dive::Shader>("CoreData/Shaders/color.hlsl");

			auto pVSVariation = pShader->GetVariation(Dive::eShaderType::Vertex, std::string());
			auto pPSVariation = pShader->GetVariation(Dive::eShaderType::Pixel, std::string());

			pPSVariation = pShader->GetVariation(Dive::eShaderType::Pixel, std::string());
		}
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Stop ȣ��");

		DV_DELETE(m_pScene);
	}
}