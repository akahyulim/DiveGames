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
		
		// test
		{
			saveModelTest();
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

	void Sandbox::saveModelTest()
	{
		// ���� ���� ����(ShadowData�� ���� ����.)
		std::vector<Dive::VertexElement> elements;
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_POSITION);
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_NORMAL);
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_COLOR);

		auto pVb = new Dive::VertexBuffer(m_pContext);
		pVb->SetSize(3, elements);

		// �ε��� ���� ����
		unsigned short indices[] = { 0, 1, 2 };
		auto pIb = new Dive::IndexBuffer(m_pContext);
		pIb->SetSize(3, false);
		{
			auto* pDest = (unsigned short*)pIb->GetShadowData();
			*pDest++ = indices[0];	*pDest++ = indices[1];	*pDest++ = indices[2];
		}

		// �޽� ����
		auto pMesh = new Dive::Mesh(m_pContext);
		pMesh->SetVertexBuffer(0, pVb);
		pMesh->SetIndexBuffer(pIb);

		// �� ����
		auto pModel = new Dive::Model(m_pContext);
		std::vector<Dive::VertexBuffer*> vertexBuffers;
		vertexBuffers.emplace_back(pVb);
		std::vector<Dive::IndexBuffer*> indexBuffers;
		indexBuffers.emplace_back(pIb);
		pModel->SetNumMeshes(1);
		pModel->SetMesh(0, pMesh);
		pModel->SetVertexBuffers(vertexBuffers);
		pModel->SetIndexBuffers(indexBuffers);

		// �� ���̺�
		pModel->Save();
	}
}