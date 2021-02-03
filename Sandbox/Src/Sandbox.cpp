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

		// 사용 Mesh에 따라 구체화된 객체를 생성시켜야 한다.
		// Rigid, Skinned, Line, Particle 등으로 나뉜다.
		auto renderable = triangle->AddComponent<Renderable>();

		// 삼각형 Geometry 구성 => VertexType을 바꿔야 한다.
		// 아니면 MeshFilter를 Renderable에 추가할 때 타입을 변경한 후 빈 곳의 값을 계산하여 넣어야 한다.
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

		// 일단 하드코딩으로 추가하자.
		renderable->SetMaterial((Material*)new Legacy(m_engine->GetContext()));
	}

	return true;
}
