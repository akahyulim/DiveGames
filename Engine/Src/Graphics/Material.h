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
		Transparent,
	};

	// update
	// material�� shader�� �������̴�.
	// ������ �ؽ��ĵ��� �ش� shader�� ����ϴ� shader resource view�� �Ŵ�.
	// ��, �ش� shader�� constant buffer�� texture�� ���� �������ִ� �Ŵ�.
	// �׷��� ����Ƽ�� standard shader material�� �Ʒ��� ���� ������ ���̴�.
	// �׷��ٸ� �ᱹ shader���� material�� ������ �Ѵٴ� �̾߱��ΰ�?
	// shader�� material, vertex type�� ���� ��Ī�ǹǷ�, rendering mode�� �����Ͽ� �׸��� �ɷ���...
	// renderable�� material�� �߰��ϴ� �����̹Ƿ�, vertex type�� Ȯ���� �� �ش� material�� ���� ���θ� ������ �� �ִ�.
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