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
	// camera �߰�
	auto camera = m_scene->CreateGameObject();
	camera->SetName("Camera");
	camera->AddComponent<Camera>();

	// game object �� Renderable Component ����
	auto triangle = m_scene->CreateGameObject();
	triangle->SetName("Triangle");
	auto renderable = triangle->AddComponent<Renderable>();

	
	// �ﰢ�� Geometry ����
	std::vector<Vertex_PosTexNorTan> vertices;
	vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));

	std::vector<unsigned int> indices;
	indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

	// Mesh�� ����
	auto mesh = new Mesh();
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	// Mesh�� MeshFilter�� ����
	auto meshFilter = new MeshFilter(m_engine->GetContext());
	if (!meshFilter->SetMesh("Triangle", mesh))
		return false;

	// Renderable�� MeshFilter ����
	renderable->SetMeshFilter(meshFilter);

	return true;
}
