#include "RenderPath3D_Editor.h"

namespace dive
{
	// view�� ���� dvRenderTexture�� ũ�⵵ ������ �ʿ��ϴ�.
	// �̴� ResizeBuffers()�� override�ϸ鼭 ���� �߰��ϸ� �� �� ����.
	// ������ ���� ���� Ÿ�̹��� �޶��� �� �ִµ�...

	void RenderPath3D_Editor::Render() const
	{
		// �� �κе� �յ��� �ʾƵ� �� �� ����.
		// => �����غ��� �� �κе� �޶��� �� �ִ�.
		// => Editor�� Camera�� App�� Main Camera�� �ٸ��� �����̴�.
	}

	void RenderPath3D_Editor::Compose() const
	{
		// ������ �� ������ ������� ���� dvRenderTexture��
		// View�� ���� dvRenderTexture�� ���ø�? �ؾ� �Ѵٴ� ���̴�.
		// ���� ũ��� ���� �ʱ� �����̴�.
	}
}