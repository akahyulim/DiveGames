#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;

	// 현재 Renderer에 동일한 의미의 enum이 존재
	// 그런데 Camera가 포함되어 있다.
	enum class eRenderingMode
	{
		Opaque,
		Cutout,
		Transparent,
	};

	// 리소스이긴한데 개별 설정도 되는거 아닌가?
	// 좀 더 생각해보자면 Default를 복사해온 후 Custermizing 하는 느낌?
	class Material : public Resource
	{
	public:
		Material(Context* context);
		~Material();

		// serialize

		// get & set

	private:
	private:
		// shader - shader 파일이 아니라 타입이다.
		// rendering mode - opaque, transparent(투명도 조절 가능), cutout(투명도는 0 or 100%)
		// color
		// texture
		// tiling
		// offset
	};
}