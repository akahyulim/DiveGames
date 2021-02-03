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
		auto triangle = m_scene->CreateGameObject();
		triangle->SetName("Triangle");

		// ��� Mesh�� ���� ��üȭ�� ��ü�� �������Ѿ� �Ѵ�.
		// Rigid, Skinned, Line, Particle ������ ������.
		auto renderable = triangle->AddComponent<Renderable>();

		// �ﰢ�� Geometry ���� => VertexType�� �ٲ�� �Ѵ�.
		// �ƴϸ� MeshFilter�� Renderable�� �߰��� �� Ÿ���� ������ �� �� ���� ���� ����Ͽ� �־�� �Ѵ�.
		std::vector<Vertex_PosCol> vertices;
		vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		std::vector<unsigned int> indices;
		indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

		auto mesh = std::make_unique<Mesh<Vertex_PosCol>>();
		mesh->SetVertices(vertices);
		mesh->SetIndices(indices);

		auto meshFilter = new MeshFilter(m_engine->GetContext());
		if (!meshFilter->SetMesh<Vertex_PosCol>(*mesh.get()))
			return false;

		renderable->SetMeshFilter(meshFilter);

		// �ϴ� �ϵ��ڵ����� �߰�����.
		renderable->SetMaterial((Material*)new Legacy(m_engine->GetContext()));
	}

	return true;
}
