#include "stdafx.h"
#include "Sandbox.h"

using namespace Dive;

namespace Sandbox
{
	void Sandbox::Initialize()
	{
		Runtime::Initialize();

		// 1. ���� App���� ����ϴ� ���� ��� �������� �ʱ�ȭ�Ѵ�.

		// 2. �⺻ RenderPath�� �ٷ� �����ų �� �ִ�.
		// Wicked�� Loader(LoadingScreen Ȯ��) Path�� �⺻������ ����ߴ�.

		// �׳� �ٷ� ������״�.
		ActivatePath(&m_renderPath);

		// ======================================================================================
		// ������ ���� ��ũ��Ʈ�� ������� �ʴ� ��� �� ������ �����ϴ°� �´�.
		// �� �� ��Ȯ�ϰ� �Ϸ��� Scene ���� ���� �����ϴ°� ���� �� �ϴ�.
		// Wicked�� '�� Scene ���� -> Scene ������ �ε� -> GlobalScene�� �߰�'�� ������ �����ش�.

		// ������ ��ũ��Ʈ�� ����� �� ��� �����ϴ����̴�.
		// ����Ƽ�� ��� Scene�� ������ Editor�� ���� �����ǰ�
		// ��ũ��Ʈ�� GameObject�� �߰��Ͽ� �����ϴ� ó���� �����ϴµ� ����ߴ�.
		// �ݸ� �Ϲ����� �ǹ̿��� ��ũ��Ʈ�� �Ʒ��� �κ��� �ܺο��� ������� ó����� ����� ���̸�
		// Wicked�� ���� ���� �̷��� ������ �����ش�.

		// ��ũ���� ��� ������ ���� �۵��� ���� �ϴ��� �մ��� �ʴ� ���� ���� �� ����.
		// �ᱹ�� �����α׷��Ӹ� ���� ���α׷����� ������ ���̶�� ������ ��κ��̴�.
		// �ٸ� ������ ��� ���ư����ϴ°�. ��, ��� �����Ǿ�� �ϴ°��� ���� ������ ����� �ʿ��ϴ�.

		m_triangle = new Mesh;
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_triangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.0f, 1.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.5f, 0.0f));
		m_triangle->m_uv.emplace_back(DirectX::XMFLOAT2(1.0f, 1.0f));
		//m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		//m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		//m_triangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		m_triangle->m_indices.emplace_back(0);
		m_triangle->m_indices.emplace_back(1);
		m_triangle->m_indices.emplace_back(2);

		m_triangle->CreateRenderDate();

		Scene::GetGlobalScene().SetMesh(m_triangle);
	}
}