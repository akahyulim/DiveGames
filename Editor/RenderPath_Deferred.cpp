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
			// ���� pass���� �׷��� texture�� shader resource view�� Ȱ���Ͽ�
			// render target�� Light�� ����� ���� ��� ȭ���� �׷�����.
		}
	}

	void RenderPath_Deferred::Compose() const
	{
		// editor�� ������� �ϳ��� Texture�� �׷����� �Ѵ�.
		// ���� ���� �ʿ�� ���� Lighting�� RenderTarget�� �״�� ����ϸ� �� �� ����.
	}

	void RenderPath_Deferred::ResizeBuffers(unsigned int width, unsigned int height)
	{
		// texture�� renderpass�� �����.
		// texture�� ��� ũ�Ⱑ �޶��� �� ���� ���� ��������� �ϹǷ�
		// �̸��� ResizeBuffers�� �� ����.
		
		// Wicked�� ��� Resolution�� ResolutionScale�� ������ ũ�⸦ ����������
		// ���� Editor�� ũ�⸦ �޾ƿ;߸� �Ѵ�.
	}
}