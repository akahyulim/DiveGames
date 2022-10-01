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

		// camera�� ��� ������ ���� ����

		// create ui content

		// setup viewport
		// 1. viewport�� ������ �� renderer�� set
		// viewport�� scene, camera �׸��� �ʿ��ϴٸ� render path�� �����Ͽ� ����
		// 2. render path�� ���ٸ� cache�κ��� load�� �� viewport�� set
		auto pView = std::make_shared<Dive::View>(m_pContext, m_pScene);
		GetSubsystem<Dive::Renderer>()->SetView(0, pView);

		// subscirbe events
		// update handle�Լ����� camera move �Լ� ȣ��
		// post render update handle �Լ����� debug geometry ȣ��

		// temp test
		{
			auto pResourceCache = GetSubsystem<Dive::ResourceCache>();
			pResourceCache->AddResourceDir("Assets/Models");
			pResourceCache->AddResourceDir("Assets/Textures");
			pResourceCache->AddResourceDir("Assets/Materials");

			{
				// vertex buffer
				std::vector<Dive::VertexElement> elements;
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);
				elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
				Dive::VertexBuffer vb(m_pContext);
				vb.SetSize(3, elements);
				{
					// �����͸� ��� ����־�� �ұ�...
					auto* pDest = vb.GetShadowData();
									
				}
				std::vector<Dive::VertexBuffer*> vertexBuffers;
				vertexBuffers.emplace_back(&vb);

				// index buffer
				std::vector<unsigned short> indices;
				indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
				Dive::IndexBuffer ib(m_pContext);
				ib.SetSize(3, false);
				ib.SetData((const void*)indices.data());
				std::vector<Dive::IndexBuffer*> indexBuffers;
				indexBuffers.emplace_back(&ib);

				// meshes
				Dive::Mesh mesh(m_pContext);
				mesh.SetNumVertexBuffers(1);
				mesh.SetVertexBuffer(0, &vb);
				mesh.SetIndexBuffer(&ib);

				// model
				Dive::Model model(m_pContext);
				model.SetName("Triangle");
				model.SetVertexBuffers(vertexBuffers);
				model.SetIndexBuffers(indexBuffers);
				model.SetNumMeshes(1);
				model.SetMesh(0, &mesh);

				model.SaveToFile("C:/Dev/Projects/DiveGames/Output/Assets/Models/Triangle.md");
			}

			{
				auto* pModel = GetSubsystem<Dive::ResourceCache>()->GetResource<Dive::Model>("Assets/Models/Triangle.md");
			}
		}
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Stop ȣ��");

		// Application Cleanup
		DV_DELETE(m_pScene);
	}
}