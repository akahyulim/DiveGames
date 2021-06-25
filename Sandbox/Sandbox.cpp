#include "Sandbox.h"

namespace sandbox
{
	Sandbox::Sandbox()
	{
		mTriangle = nullptr;

		mAppTitle = "SandBox";
	}

	Sandbox::~Sandbox()
	{
		if (mTriangle)
		{
			delete mTriangle;
			mTriangle = nullptr;
		}
	}

	bool Sandbox::Initialize()
	{
		if (!Runtime::Initialize())
		{
			return false;
		}

		ActivatePath(&mRenderPath);

		//triangle_coloring();
		//triangle_texturing();
		{
			auto gameObject = dive::Scene::GetGlobalScene().CreateGameObject();
			auto meshRenderer = gameObject->AddComponent<dive::MeshRenderer>();

			static dive::dvMesh mesh;
			mesh.SetName("Cube");
			dive::utility::geometry::CreateCube(mesh.GetVertices(), mesh.GetIndices());

			meshRenderer->SetMesh(mesh);
		}

		APP_TRACE("Sandbox::Initialize()");
		
		return true;
	}

	void Sandbox::triangle_coloring()
	{
		mTriangle = new dive::Mesh;
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		mTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		mTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		mTriangle->m_colors.emplace_back(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		mTriangle->m_indices.emplace_back(0);
		mTriangle->m_indices.emplace_back(1);
		mTriangle->m_indices.emplace_back(2);

		mTriangle->CreateRenderDate();

		dive::Scene::GetGlobalScene().SetMesh(mTriangle);

		// 현재는 Scene이 Mesh 단일 객체를 멤버 변수로 관리하고 있다.
		// 결국엔 GameObject로 다루어야 한다.
		// Camera를 InputSystem으로 제어할 수 있어야 한다.
	}

	void Sandbox::triangle_texturing()
	{
		mTriangle = new dive::Mesh;
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f));
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		mTriangle->m_positions.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f));
		mTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.0f, 1.0f));
		mTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(0.5f, 0.0f));
		mTriangle->m_uv.emplace_back(DirectX::XMFLOAT2(1.0f, 1.0f));
		mTriangle->m_indices.emplace_back(0);
		mTriangle->m_indices.emplace_back(1);
		mTriangle->m_indices.emplace_back(2);

		mTriangle->CreateRenderDate();

		dive::Scene::GetGlobalScene().SetMesh(mTriangle);

		// 텍스쳐도 여기에서 생성한 후 등록해야 한다.
	}
}