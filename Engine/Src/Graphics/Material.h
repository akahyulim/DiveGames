#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;

	// ���� Renderer�� ������ �ǹ��� enum�� ����
	// �׷��� Camera�� ���ԵǾ� �ִ�.
	enum class eRenderingMode
	{
		Opaque,
		Cutout,
		Transparent,
	};

	// ���ҽ��̱��ѵ� ���� ������ �Ǵ°� �ƴѰ�?
	// �� �� �����غ��ڸ� Default�� �����ؿ� �� Custermizing �ϴ� ����?
	class Material : public Resource
	{
	public:
		Material(Context* context);
		~Material();

		// serialize

		// get & set

	private:
	private:
		// shader - shader ������ �ƴ϶� Ÿ���̴�.
		// rendering mode - opaque, transparent(���� ���� ����), cutout(������ 0 or 100%)
		// color
		// texture
		// tiling
		// offset
	};
}