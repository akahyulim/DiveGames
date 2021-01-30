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
		Transparent,
	};

	// update
	// material은 shader에 종속적이다.
	// 각각의 텍스쳐들은 해당 shader가 사용하는 shader resource view인 거다.
	// 즉, 해당 shader에 constant buffer로 texture와 값을 전달해주는 거다.
	// 그래서 유니티의 standard shader material이 아래와 같은 형태인 것이다.
	// 그렇다면 결국 shader별로 material을 만들어야 한다는 이야기인가?
	// shader와 material, vertex type은 서로 매칭되므로, rendering mode만 염두하여 그리면 될려나...
	// renderable에 material을 추가하는 형태이므로, vertex type을 확인한 후 해당 material의 지원 여부를 결정할 수 있다.
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