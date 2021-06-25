#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
//#include "ThreadPool.h"
#include "Log.h"

namespace dive
{
	void RenderPath3D::PreUpdate()
	{
		CORE_TRACE("RenderPath3D::PreUpdate() - Previous Camera ����");
	}

	// �׸����� �غ� �Ѵ�.
	void RenderPath3D::Update(float deltaTime)
	{
		// 2d update
		CORE_TRACE("RenderPath3D::Update() - RenderPath2D::Update()");

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			mScene->Update(deltaTime * timeScale);
		}

		// visibility ���� �� �ʱ�ȭ �׸��� ����
		// renderer::updatevisibility�� ���� ����
		CORE_TRACE("RenderPath3D::Update() - Visibility ����");

		// renderer::updatePerFrameData: �� ������ ������Ʈ �Ǵ� ������???
		// ���⿡�� �����͸� ���� �� �ٸ� ������ buffer�� ����� �����ϴ� �� ����.
		CORE_TRACE("RenderPath3D::Update() - Update Per Frame Data(Constant Buffer)");

		// camera update
		CORE_TRACE("RenderPath3D::Update() - Update Camera Component");
	}
	
	// Renderer�� ��� ��������...
	void RenderPath3D::Render() const
	{
		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// Eidtor���� Backbuffer�� RenderTarget�� ũ�Ⱑ �ٸ��Ƿ�
		// Sandbox���� �̷��� Backbuffer ũ��� ������ �� �־�� �Ѵ�.
		float width = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionWidth();
		float height = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionHeight();
		Renderer::GetInstance().SetViewport(width, height);
		
		Renderer::GetInstance().UpdateCB();
		//Renderer::GetInstance().DrawColor();
		//Renderer::GetInstance().DrawTexturing();
		Renderer::GetInstance().DrawLegacy();

		// TextMesh�� GameObject ���� �� �ٽ� �׽�Ʈ
		//Renderer::GetInstance().DrawText();
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget ����");
	}
}