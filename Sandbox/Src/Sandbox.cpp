#include "Sandbox.h"

DIVE_APPLICATION_MAIN(Sandbox)

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
	SAFE_DELETE(m_scene);
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


	m_scene = new Scene(const_cast<Context*>(m_engine->GetContext()), "Test Scene");
	if(!createTriangle())
		APP_ERROR("Fail to create geometry buffers.");
}

void Sandbox::Stop()
{
}

bool Sandbox::createTriangle()
{
	auto triangle = m_scene->CreateGameObject();
	triangle->SetName("Triangle");
	auto renderable = triangle->AddComponent<Renderable>();
	auto model = renderable->GetModel();

	std::vector<Vertex_PosTexNorTan> vertices;
	vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));
	vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f));

	std::vector<unsigned int> indices;
	indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

	return model->SetAndCreateGeometry(vertices, indices);
}
