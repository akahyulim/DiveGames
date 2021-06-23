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

		// 현재는 Scene이 Mesh 단일 객체를 멤버 변수로 관리하고 있다.
		// 결국엔 GameObject로 다루어야 한다.
		// Camera를 InputSystem으로 제어할 수 있어야 한다.
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

		// 텍스쳐도 여기에서 생성한 후 등록해야 한다.
	}
}