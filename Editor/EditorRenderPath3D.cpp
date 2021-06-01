#include "EditorRenderPath3D.h"

// �̰� ���� ���� �ʿ䰡 �ֳ� �ʹ�.
// ������ ��ó���� �����Ѵٸ� RenderTarget �ϳ����� �⺻ Renderer���� �����ؾ� ���ٵ�...
// => ������ 1) �⺻���� �������� �� RenderTarget�� ������ �ؾ��ϳ�? 2) �� RenderTarget�� BackBuffer�� �ٽ� �׷��� �ϳ�? ���� �ִ�.
// => �̰� �����Ϸ��� Core�ʿ��� App�� Editor�� �����ؾ� �ϴµ�... �׷��� ������ ������ ���� �ʴ´�.
// �̺��� Wicked�� RenderPath�� ��� �����ϰ� �����ϴ��� Ȯ���ؾ� �� �� ����.

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
		APP_TRACE("RenderPath3D::PreUpdate() - Previous Camera ����");
	}
	
	// �� �κб����� RenderPath3D()�� ����� �� �׳� �ᵵ �� �� �ϴ�.
	void EditorRenderPath3D::Update(float deltaTime)
	{
		// 2d update
		APP_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = Dive::TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// visibility ���� �� �ʱ�ȭ �׸��� ����
		// renderer::updatevisibility�� ���� ����
		APP_TRACE("RenderPath3D::Update() - Visibility ����");

		// renderer::updatePerFrameData: �� ������ ������Ʈ �Ǵ� ������???
		// ���⿡�� �����͸� ���� �� �ٸ� ������ buffer�� ����� �����ϴ� �� ����.
		APP_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		APP_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	// Editor�� ���� ���׿� ���� ��ó���� �����Ͽ� ������ �� �ֵ���?
	// �׷��� �� ���� RenderPath3D()�� �׳� ����ص� ���� ������?
	// ������ Wicked�� RenderPath3D()�� ������� ���� �ʰ� �ִ�.
	void EditorRenderPath3D::Render() const
	{
		auto pImmediateContext = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// �̷��� �����Ϸ��� RenderTarget�� ���� �����ؾ� �Ѵ�. 
		// ����Ʈ�� Backbuffer, Texture ���޽ÿ��� �ش� RenderTarget�� ������ �ϸ� �ȴ�.
		// �ƴϸ� ���� �� �κп��ٰ� RenderTarget�� �����ϸ� �ȴ�.
		Dive::Renderer::GetInstance().UpdateCB();
		Dive::Renderer::GetInstance().DrawColor();
	}
	
	void EditorRenderPath3D::Compose() const
	{
	}
}