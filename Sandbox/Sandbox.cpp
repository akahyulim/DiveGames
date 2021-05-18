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

		// 현재는 Scene이 Mesh 단일 객체를 멤버 변수로 관리하고 있다.
		// 결국엔 GameObject로 다루어야 한다.
		// Camera를 InputSystem으로 제어할 수 있어야 한다.
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

		// 텍스쳐도 여기에서 생성한 후 등록해야 한다.
	}
}