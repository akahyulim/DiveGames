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
		m_EngineParameters.width = 800;
		m_EngineParameters.height = 600;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = true;//false;

		// resource paths?
	}

	// ���� �ʱ�ȭ �� ȣ��
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Start ȣ��");

		// create scene content
		// scene�� ���� ������ �� cache�� �̿��Ͽ� object�� ����
		m_pScene = new Dive::Scene(m_pContext);
		
		// buffer test
		{
			// vertices, indices ����
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

		// camera�� ��� ������ ���� ����

		// create ui content

		// setup viewport
		// 1. viewport�� ������ �� renderer�� set
		// viewport�� scene, camera �׸��� �ʿ��ϴٸ� render path�� �����Ͽ� ����
		// 2. render path�� ���ٸ� cache�κ��� load�� �� viewport�� set
		auto pView = std::make_shared<Dive::View>(m_pContext);
		GetSubsystem<Dive::Renderer>()->SetView(0, pView);

		// subscirbe events
		// update handle�Լ����� camera move �Լ� ȣ��
		// post render update handle �Լ����� debug geometry ȣ��
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Stop ȣ��");

		// Application Cleanup
		DV_DELETE(m_pScene);
	}
}