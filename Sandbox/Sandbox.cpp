#include "Sandbox.h"

namespace Sandbox
{
	Sandbox::Sandbox()
	{
		m_pTriangle = nullptr;
	}

	Sandbox::~Sandbox()
	{
		if (m_pTriangle)
		{
			delete m_pTriangle;
			m_pTriangle = nullptr;
		}
	}

	bool Sandbox::Initialize()
	{
		if (!Runtime::Initialize())
		{
			return false;
		}

		ActivatePath(&m_renderPath);

		triangle_coloring();
		//triangle_texturing();

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}

	void Sandbox::triangle_coloring()
	{
		m_pTriangle = new Dive::Mesh;
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		m_pTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		m_pTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		m_pTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		m_pTriangle->m_indices.emplace_back(0);
		m_pTriangle->m_indices.emplace_back(1);
		m_pTriangle->m_indices.emplace_back(2);

		m_pTriangle->CreateRenderDate();

		Dive::Scene::GetGlobalScene().SetMesh(m_pTriangle);

		// ����� Scene�� Mesh ���� ��ü�� ��� ������ �����ϰ� �ִ�.
		// �ᱹ�� GameObject�� �ٷ��� �Ѵ�.
		// Camera�� InputSystem���� ������ �� �־�� �Ѵ�.
	}

	void Sandbox::triangle_texturing()
	{
		m_pTriangle = new Dive::Mesh;
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_pTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		m_pTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.0f, 1.0f));
		m_pTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.5f, 0.0f));
		m_pTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(1.0f, 1.0f));
		m_pTriangle->m_indices.emplace_back(0);
		m_pTriangle->m_indices.emplace_back(1);
		m_pTriangle->m_indices.emplace_back(2);

		m_pTriangle->CreateRenderDate();

		Dive::Scene::GetGlobalScene().SetMesh(m_pTriangle);

		// �ؽ��ĵ� ���⿡�� ������ �� ����ؾ� �Ѵ�.
	}
}