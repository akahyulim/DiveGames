#include "stdafx.h"
#include "Sandbox.h"

using namespace Dive;

namespace Sandbox
{
	void Sandbox::Initialize()
	{
		Runtime::Initialize();

		// 1. 현재 App에서 사용하는 각종 멤버 변수들을 초기화한다.

		// 2. 기본 RenderPath를 바로 적용시킬 수 있다.
		// Wicked는 Loader(LoadingScreen 확장) Path를 기본적으로 사용했다.

		// 그냥 바로 적용시켰다.
		ActivatePath(&m_renderPath);

		// ======================================================================================
		// 따지고 보면 스크립트를 사용하지 않는 경우 이 곳에서 생성하는게 맞다.
		// 좀 더 정확하게 하려면 Scene 역시 직접 생성하는게 맞을 듯 하다.
		// Wicked는 '빈 Scene 생성 -> Scene 파일을 로드 -> GlobalScene에 추가'의 과정을 보여준다.

		// 문제는 스크립트를 사용할 때 어떻게 구성하느냐이다.
		// 유니티의 경우 Scene의 구성은 Editor를 통해 관리되고
		// 스크립트는 GameObject에 추가하여 제어하는 처리를 구현하는데 사용했다.
		// 반면 일반적인 의미에서 스크립트는 아래의 부분을 외부에서 순서대로 처리토록 만드는 것이며
		// Wicked의 예제 또한 이러한 구현을 보여준다.

		// 스크립의 사용 유무에 관한 글들을 보니 일단은 손대지 않는 편이 나은 것 같다.
		// 결국엔 비프로그래머를 위한 프로그래머의 수고일 뿐이라는 내용이 대부분이다.
		// 다만 게임이 어떻게 돌아가야하는가. 즉, 어떻게 구성되어야 하는가에 대한 여전히 고민은 필요하다.

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