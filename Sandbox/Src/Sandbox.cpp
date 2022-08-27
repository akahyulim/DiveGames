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
		m_EngineParameters.width = 800;
		m_EngineParameters.height = 600;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = true;//false;

		// resource paths?
	}

	// 엔진 초기화 후 호출
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Start 호출");

		// create scene content
		// scene을 직접 생성한 후 cache를 이용하여 object를 구성
		m_pScene = new Dive::Scene(m_pContext);
		
		// buffer test
		{
			// vertices, indices 생성
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);

			Dive::VertexBuffer buffer(GetContext());
			buffer.SetBuffer(3, elements, true);

			std::vector<DirectX::XMFLOAT3> triangle;
			triangle.emplace_back(-0.5f, -0.5f, 0.0f);
			triangle.emplace_back(0.5f, -0.5f, 0.0f);
			triangle.emplace_back(0.5f, 0.5f, 0.0f);

			buffer.SetData(triangle.data());

			Dive::IndexBuffer indexBuffer(GetContext());
			indexBuffer.CreateBuffer(3, false);

			std::vector<unsigned int> indices;
			indices.emplace_back(0);
			indices.emplace_back(1);
			indices.emplace_back(2);
			indexBuffer.SetData(indices.data());

			buffer.Release();
			indexBuffer.Release();
		}

		// camera는 멤버 변수로 별도 관리

		// create ui content

		// setup viewport
		// 1. viewport를 생성한 후 renderer에 set
		// viewport는 scene, camera 그리고 필요하다면 render path를 전달하여 생성
		// 2. render path가 없다면 cache로부터 load한 후 viewport에 set
		auto pView = std::make_shared<Dive::View>(m_pContext);
		GetSubsystem<Dive::Renderer>()->SetView(0, pView);

		// subscirbe events
		// update handle함수에서 camera move 함수 호출
		// post render update handle 함수에서 debug geometry 호출
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Stop 호출");

		// Application Cleanup
		DV_DELETE(m_pScene);
	}
}