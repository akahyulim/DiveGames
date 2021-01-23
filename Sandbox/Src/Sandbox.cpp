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
	// 엔진 초기화 전에 호출된다.
}

void Sandbox::Start()
{
	auto graphics = m_engine->GetSubsystem<Graphics>();
	// 아이콘 변경
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

// 유니티 식으로 하려면 해당 GameObject의 Script에서 직접 구성해야 한다.
bool Sandbox::createTriangle()
{
	// camera 추가
	auto camera = m_scene->CreateGameObject();
	camera->SetName("Camera");
	camera->AddComponent<Camera>();

	// game object 및 Renderable Component 생성
	auto triangle = m_scene->CreateGameObject();
	triangle->SetName("Triangle");
	auto renderable = triangle->AddComponent<Renderable>();

	
	// 삼각형 Geometry 구성
	std::vector<Vertex_PosTexNorTan> vertices;
	vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));

	std::vector<unsigned int> indices;
	indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

	// Mesh에 저장
	auto mesh = new Mesh();
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);

	// Mesh를 MeshFilter에 저장
	auto meshFilter = new MeshFilter(m_engine->GetContext());
	if (!meshFilter->SetMesh("Triangle", mesh))
		return false;

	// Renderable에 MeshFilter 전달
	renderable->SetMeshFilter(meshFilter);

	return true;
}
