#include "Sandbox.h"

namespace sandbox
{
	Sandbox::Sandbox()
	{
		mpTriangle = nullptr;

		m_title = "SandBox";
	}

	Sandbox::~Sandbox()
	{
		if (mpTriangle)
		{
			delete mpTriangle;
			mpTriangle = nullptr;
		}
	}

	bool Sandbox::Initialize()
	{
		if (!Runtime::Initialize())
		{
			return false;
		}

		ActivatePath(&mRenderPath);

		triangle_coloring();
		//triangle_texturing();

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}

	void Sandbox::triangle_coloring()
	{
		mpTriangle = new dive::Mesh;
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		mpTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		mpTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		mpTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		mpTriangle->m_indices.emplace_back(0);
		mpTriangle->m_indices.emplace_back(1);
		mpTriangle->m_indices.emplace_back(2);

		mpTriangle->CreateRenderDate();

		dive::Scene::GetGlobalScene().SetMesh(mpTriangle);

		// ����� Scene�� Mesh ���� ��ü�� ��� ������ �����ϰ� �ִ�.
		// �ᱹ�� GameObject�� �ٷ��� �Ѵ�.
		// Camera�� InputSystem���� ������ �� �־�� �Ѵ�.
	}

	void Sandbox::triangle_texturing()
	{
		mpTriangle = new dive::Mesh;
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		mpTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		mpTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.0f, 1.0f));
		mpTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.5f, 0.0f));
		mpTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(1.0f, 1.0f));
		mpTriangle->m_indices.emplace_back(0);
		mpTriangle->m_indices.emplace_back(1);
		mpTriangle->m_indices.emplace_back(2);

		mpTriangle->CreateRenderDate();

		dive::Scene::GetGlobalScene().SetMesh(mpTriangle);

		// �ؽ��ĵ� ���⿡�� ������ �� ����ؾ� �Ѵ�.
	}
}