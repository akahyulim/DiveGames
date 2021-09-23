#include "RenderPath3D_Editor.h"

namespace dive
{
	// view에 붙일 dvRenderTexture의 크기도 갱신이 필요하다.
	// 이는 ResizeBuffers()를 override하면서 끝에 추가하면 될 것 같다.
	// 하지만 둘의 갱신 타이밍이 달라질 수 있는데...

	void RenderPath3D_Editor::Render() const
	{
		// 이 부분도 손데지 않아도 될 것 같다.
		// => 생각해보니 이 부분도 달라질 수 있다.
		// => Editor의 Camera와 App의 Main Camera가 다르기 때문이다.
	}

	void RenderPath3D_Editor::Compose() const
	{
		// 문제는 이 곳에서 결과물을 가진 dvRenderTexture를
		// View에 붙일 dvRenderTexture로 샘플링? 해야 한다는 것이다.
		// 둘의 크기는 같지 않기 때문이다.
	}
}