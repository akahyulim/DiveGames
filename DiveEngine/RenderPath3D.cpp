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
			// thread로 CommandList를 만든다.
			// 그런데 범위를 어떻게 정하는지 모르겠다.
			// pass와는 별개이다.
		}
	}

	void RenderPath3D::Compose(CommandList cmd) const
	{
		// 결과물을 텍스쳐에 그리는 것 같다.
	}
}