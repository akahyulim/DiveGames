#include "stdafx.h"
#include "Sandbox.h"

using namespace Dive;

namespace Sandbox
{
	void Sandbox::Initialize()
	{
		Runtime::Initialize();

		// 중간 처리???

		SetActivePath(&m_renderPath);


		// 역시 위치가 에바
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
	}
}