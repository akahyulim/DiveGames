#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Timer.h"
//#include "ThreadPool.h"

namespace Dive
{
	void RenderPath3D::PreUpdate()
	{
	}

	void RenderPath3D::Update(float deltaTime)
	{
		// 2d update

		// scene update
		{
			auto timeScale = TimeManager::GetInstance().GetTimeScale();
			m_scene->Update(deltaTime * timeScale);
		}

		// visibility ���� �� �ʱ�ȭ �׸��� ����
		// renderer::updatevisibility�� ���� ����

		// renderer::updatePerFrameData: �� ������ ������Ʈ �Ǵ� ������???
		// ���⿡�� �����͸� ���� �� �ٸ� ������ buffer�� ����� �����ϴ� �� ����.

		// camera update
		
	}
	
	void RenderPath3D::Render() const
	{
		// device�� ���� ����ϴ� �� �� ���� ����.
		// �׷��� bind�� ���ؼ� ��¿�� ���µ�?
		auto device = Renderer::GetInstance().GetDevice();

	}

	void RenderPath3D::Compose() const
	{
		// ������� �ؽ��Ŀ� �׸��� �� ����.
	}
}