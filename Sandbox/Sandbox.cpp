#include "stdafx.h"
#include "Sandbox.h"

using namespace Dive;

namespace Sandbox
{
	void Sandbox::Initialize()
	{
		Runtime::Initialize();

		ActivatePath(&m_renderPath);

		triangle_coloring();
		//triangle_texturing();
	}

	void Sandbox::triangle_coloring()
	{
		m_triangle = new Mesh;
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		m_triangle->m_indices.emplace_back(0);
		m_triangle->m_indices.emplace_back(1);
		m_triangle->m_indices.emplace_back(2);

		m_triangle->CreateRenderDate();

		Scene::GetGlobalScene().SetMesh(m_triangle);

		// ����� Scene�� Mesh ���� ��ü�� ��� ������ �����ϰ� �ִ�.
		// �ᱹ�� GameObject�� �ٷ��� �Ѵ�.
		// Camera�� InputSystem���� ������ �� �־�� �Ѵ�.
	}

	void Sandbox::triangle_texturing()
	{
		m_triangle = new Mesh;
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.0f, 1.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.5f, 0.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(1.0f, 1.0f));
		m_triangle->m_indices.emplace_back(0);
		m_triangle->m_indices.emplace_back(1);
		m_triangle->m_indices.emplace_back(2);

		m_triangle->CreateRenderDate();

		Scene::GetGlobalScene().SetMesh(m_triangle);

		// �ؽ��ĵ� ���⿡�� ������ �� ����ؾ� �Ѵ�.
	}
}