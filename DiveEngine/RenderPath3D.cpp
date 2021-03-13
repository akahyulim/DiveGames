#include "RenderPath3D.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "ThreadPool.h"

namespace Dive
{
	void RenderPath3D::PreUpdate()
	{
	}

	void RenderPath3D::Update(float deltaTime)
	{
	}
	
	void RenderPath3D::Render() const
	{
		auto device = Renderer::GetInstance().GetDevice();

		{
			auto index = device->BeginCommandList();
			// thread�� CommandList�� �����.
			// �׷��� ������ ��� ���ϴ��� �𸣰ڴ�.
			// pass�ʹ� �����̴�.
		}
	}

	void RenderPath3D::Compose(CommandList cmd) const
	{
		// ������� �ؽ��Ŀ� �׸��� �� ����.
	}
}