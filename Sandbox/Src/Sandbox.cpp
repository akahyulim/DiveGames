#include "Sandbox.h"

DIVE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

void Sandbox::Setup()
{
	// ���� �ʱ�ȭ ���� ȣ��ȴ�.
}

void Sandbox::Start()
{
	auto graphics = m_engine->GetSubsystem<Graphics>();
	// ������ ����
	graphics->GetWindow()->SetTitle(L"Sandbox");
	graphics->GetWindow()->Show(true);
	APP_TRACE("Sandbox Start~");


	m_scene = m_engine->GetSubsystem<Scene>();
	m_scene->SetName("Test Scene");
	if(!createTriangle())
		APP_ERROR("Fail to create geometry buffers.");
}

void Sandbox::Stop()
{
}

// ����Ƽ ������ �Ϸ��� �ش� GameObject�� Script���� ���� �����ؾ� �Ѵ�.
bool Sandbox::createTriangle()
{
	// camera
	{
		auto camera = m_scene->CreateGameObject();
		camera->SetName("Camera");
		camera->AddComponent<Camera>();
	}

	// Triangle
	{
		// mesh
		std::vector<Vertex_PosTexNorTan> vertices;
		vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
		std::vector<unsigned int> indices;
		indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

		auto triangleMesh = std::make_shared<Mesh>(m_engine->GetContext());
		triangleMesh->CreateBuffer<Vertex_PosTexNorTan>(eBufferType::VertexBuffer, vertices);
		triangleMesh->CreateBuffer<unsigned int>(eBufferType::IndexBuffer, indices);
		triangleMesh->SetResourceName("Triangle");

		auto triangle = m_scene->CreateGameObject();
		triangle->SetName("Triangle");
		auto renderableMesh = triangle->AddComponent<RenderableMesh>();
		renderableMesh->SetMesh(triangleMesh.get());
	}

	// test
	{
		auto rscMng = m_engine->GetSubsystem<ResourceManager>();
		//rscMng->Load<Dive_Texture>("C://Dev//Ref//Unity_UI//LightK.png");
		auto square = std::make_shared<Mesh>(m_engine->GetContext());
		square->SetResourceName("square");
		auto knave = std::make_shared<Mesh>(m_engine->GetContext());
		knave->SetResourceName("knave");
		auto complex = std::make_shared<Mesh>(m_engine->GetContext());
		complex->SetResourceName("complex");
		auto lycoris = std::make_shared<Mesh>(m_engine->GetContext());
		lycoris->SetResourceName("lycoris");

		rscMng->Cache(std::static_pointer_cast<Resource>(square));
		rscMng->Cache(std::static_pointer_cast<Resource>(knave));
		rscMng->Cache(std::static_pointer_cast<Resource>(complex));
		rscMng->Cache(std::static_pointer_cast<Resource>(lycoris));

		APP_TRACE("Resource Count: {:d}", rscMng->GetResourceCount(eResourceType::Mesh));

		rscMng->Remove(std::static_pointer_cast<Resource>(complex));
		auto tmp = rscMng->GetByName(eResourceType::Mesh, "complex");
		if (!tmp)
			APP_TRACE("�������� �ʴ� ��ü�Դϴ�.");

		APP_TRACE("Resource Count: {:d}", rscMng->GetResourceCount(eResourceType::Mesh));
	}

	return true;
}
