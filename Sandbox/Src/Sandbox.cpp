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

	// 엔진 생성 전에 호출
	void Sandbox::Setup()
	{
		// ini 파일 등을 읽어오는 편이 낫다.
		m_EngineParameters.title = L"Sandbox";
		m_EngineParameters.width = 1280;
		m_EngineParameters.height = 760;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = true;// false;

		// resource paths?
	}

	// 엔진 초기화 후 호출
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Start 호출");

		m_pScene = new Dive::Scene(m_pContext);
		auto pViewport = new Dive::Viewport(m_pContext);
		pViewport->SetScene(m_pScene);
		GetSubsystem<Dive::Renderer>()->SetViewport(0, pViewport);

		// renderPath
		{

		}

		// model: 버퍼 생성 확인!
		{
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);

			std::vector<float> vertices =
				{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };

			Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
			pVb->SetSize(3, elements);
			pVb->SetData(static_cast<void*>(vertices.data()));

			std::vector<unsigned short> indices = { 0, 1, 2 };
			Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
			pIb->SetSize(3, false);
			pIb->SetData(static_cast<void*>(indices.data()));

			Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
			pMesh->SetNumVertexBuffers(1);
			pMesh->SetVertexBuffer(0, pVb);
			pMesh->SetIndexBuffer(pIb);

			static Dive::Model* pTriangleModel = new Dive::Model(m_pContext);
			pTriangleModel->SetName("Triangle");
			pTriangleModel->SetNumMeshes(1);
			pTriangleModel->SetMesh(0, pMesh);

			auto pTriangle = m_pScene->CreateGameObject("Triangle");
			auto pModel = pTriangle->CreateComponent<Dive::StaticModel>();
			pModel->SetModel(pTriangleModel);
		}
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_TRACE("Sandbox::Stop 호출");

		DV_DELETE(m_pScene);
	}
}