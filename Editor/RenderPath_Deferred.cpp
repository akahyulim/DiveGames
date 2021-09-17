#include "RenderPath_Deferred.h"

// Editor�� Camera�� �� ������ ������� �� �� ������?
// �׸��� App�� Main Camera���� RenderTexture�� �ٿ� �������� ���̰� �ϸ� �� �� ������...
// ���� App ����ÿ��� RenderTexture�� �������� ���ƾ���.
namespace dive
{
	// ������ Render Path�� Pass�� �������� ���ϰڴ�...
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
			// ���� pass���� �׷��� texture�� shader resource view�� Ȱ���Ͽ�
			// render target�� Light�� ����� ���� ��� ȭ���� �׷�����.
		}

		// Post processing
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