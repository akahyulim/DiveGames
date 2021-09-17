#include "RenderPath_Deferred.h"

// Editor의 Camera를 이 곳에서 관리토록 할 수 없을까?
// 그리고 App의 Main Camera에는 RenderTexture를 붙여 구석에서 보이게 하면 될 것 같은데...
// 물론 App 실행시에는 RenderTexture를 적용하지 말아야지.
namespace dive
{
	// 아직도 Render Path와 Pass를 구분하지 못하겠다...
	void RenderPath_Deferred::Render() const
	{
		// deferred shading
		{
			// render pass begin(render targets clear and set)
			// set viewport and bind 
			// draw call
			// render pass end(render targets set null)
		}

		// Lighting shading
		{
			// 위의 pass에서 그려진 texture의 shader resource view를 활용하여
			// render target에 Light가 적용된 최종 출력 화면을 그려낸다.
		}

		// Post processing
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