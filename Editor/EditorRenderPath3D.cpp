#include "EditorRenderPath3D.h"

// 이걸 굳이 만들 필요가 있나 싶다.
// 어차피 후처리를 생각한다면 RenderTarget 하나쯤은 기본 Renderer에도 존재해야 할텐데...
// => 문제는 1) 기본적인 렌더링도 이 RenderTarget을 사용토록 해야하나? 2) 이 RenderTarget을 BackBuffer에 다시 그려야 하나? 등이 있다.
// => 이걸 구분하려면 Core쪽에서 App과 Editor를 구분해야 하는데... 그러한 구현이 마음에 들지 않는다.
// 이보다 Wicked가 RenderPath를 어떻게 구성하고 스왑하는지 확인해야 할 것 같다.

namespace Editor
{
	EditorRenderPath3D::EditorRenderPath3D()
	{
		m_pScene = &Dive::Scene::GetGlobalScene();
		m_pTexture = nullptr;
	}
	
	EditorRenderPath3D::~EditorRenderPath3D()
	{
	}
	
	void EditorRenderPath3D::PreUpdate()
	{
		APP_TRACE("RenderPath3D::PreUpdate() - Previous Camera 저장");
	}
	
	// 이 부분까지는 RenderPath3D()를 상속한 후 그냥 써도 될 듯 하다.
	void EditorRenderPath3D::Update(float deltaTime)
	{
		// 2d update
		APP_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = Dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// visibility 생성 및 초기화 그리고 설정
		// renderer::updatevisibility로 마저 설정
		APP_TRACE("RenderPath3D::Update() - Visibility 구성");

		// renderer::updatePerFrameData: 매 프레임 업데이트 되는 데이터???
		// 여기에서 데이터를 얻어온 후 다른 곳에서 buffer를 만들어 전달하는 것 같다.
		APP_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		APP_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	// Editor의 수정 사항에 맞춰 후처리는 선택하여 적용할 수 있도록?
	// 그런데 이 역시 RenderPath3D()를 그냥 사용해도 되지 않을까?
	// 실제로 Wicked는 RenderPath3D()를 상속조차 하지 않고 있다.
	void EditorRenderPath3D::Render() const
	{
		auto pImmediateContext = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// 이렇게 구현하려면 RenderTarget을 직접 전달해야 한다. 
		// 디폴트는 Backbuffer, Texture 전달시에는 해당 RenderTarget을 사용토록 하면 된다.
		// 아니면 여기 앞 부분에다가 RenderTarget을 변경하면 된다.
		Dive::Renderer::GetInstance().UpdateCB();
		Dive::Renderer::GetInstance().DrawColor();
	}
	
	void EditorRenderPath3D::Compose() const
	{
	}
}