#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
#include "Log.h"

/*
* �ٽ� �� �������.
*/
namespace dive
{
	/*
	* ResizeBuffer
	* ��ǻ� �߰� Texture���� ������ �߰� ���ҵ��� �����ϴ� �Լ���.
	* ũ��� GraphicsDevice�� Resolution�� ������� �Ѵ�.
	* ���� �ֱ�� RenderPath2D�� Load, Update ���̸�, WM_SIZE �̺�Ʈ�� �����ϴ� �� ����.
	*/

	void RenderPath3D::PreUpdate()
	{
		if (!m_pScene)	return;

		CORE_TRACE("RenderPath3D::PreUpdate() - Previous Camera ����");

		// GameObject PreUpdate�� ������ �� �ִ�.
	}

	// �׸����� �غ� �Ѵ�.
	void RenderPath3D::Update(float deltaTime)
	{
		if (!m_pScene)	return;

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_pScene->Update(deltaTime * timeScale);
		}

		// �� ������ ������Ʈ �Ǵ� �����͸� ����
		{

		}
	}
	
	// Renderer�� ��� ��������...
	void RenderPath3D::Render() const
	{
		if (!m_pScene)	return;

		auto pImmediateContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// Eidtor���� Backbuffer�� RenderTarget�� ũ�Ⱑ �ٸ��Ƿ�
		// Sandbox���� �̷��� Backbuffer ũ��� ������ �� �־�� �Ѵ�.
		float width = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionWidth();
		float height = (float)Renderer::GetInstance().GetGraphicsDevice()->GetResolutionHeight();
		Renderer::GetInstance().SetViewport(width, height);
		
		Renderer::GetInstance().UpdateCB();
		Renderer::GetInstance().DrawLegacy();
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget ����");
	}
}