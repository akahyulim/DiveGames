#include "RenderPath_Deferred.h"

namespace dive
{
	void RenderPath_Deferred::Render() const
	{
		// depth
		{
			// render pass begin(render targets clear and set)
			// set viewport and bind 
			// draw call
			// render pass end(render targets set null)
		}

		// Lighting
		{
			// 위의 pass에서 그려진 texture의 shader resource view를 활용하여
			// render target에 Light가 적용된 최종 출력 화면을 그려낸다.
		}
	}

	void RenderPath_Deferred::Compose() const
	{
		// editor는 결과물을 하나의 Texture에 그려내야 한다.
		// 따로 만들 필요는 없고 Lighting의 RenderTarget을 그대로 사용하면 될 것 같다.
	}

	void RenderPath_Deferred::ResizeBuffers(unsigned int width, unsigned int height)
	{
		// texture와 renderpass를 만든다.
		// texture의 경우 크기가 달라질 때 마다 새로 만들어져야 하므로
		// 이름이 ResizeBuffers인 것 같다.
		
		// Wicked의 경우 Resolution과 ResolutionScale의 곱으로 크기를 설정했지만
		// 나는 Editor의 크기를 받아와야만 한다.
	}
}