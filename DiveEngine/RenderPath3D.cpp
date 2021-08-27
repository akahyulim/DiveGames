#include "RenderPath3D.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
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
	
	// ���� Sandbox�� ��Ȱ��ȭ ���״�.
	// ���� Editor�� RenderPath�� �����Ͽ� ��������.
	void RenderPath3D::Render() const
	{
	}

	void RenderPath3D::Compose() const
	{
		CORE_TRACE("RenderPath3D::Compose() - RenderTarget ����");
	}
}