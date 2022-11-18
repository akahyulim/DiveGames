#include "divepch.h"
#include "BatchRenderer.h"
#include "Graphics/Graphics.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "IO/Log.h"

namespace Dive
{
	// shader�� ���� ���� mesh�� ����Ǿ�� �� ���ҽ��� gpu�� ������.
	void BatchRenderer::Prepare(View* pView)
	{
		// �̰� �ӽô�. Shader�� ������ �־�� �Ѵ�.
		pView->GetGraphics()->SetDefaultShader();

		// Pass�� �̿���
		// graphics�� blend, cull, fill mode���� �����Ѵ�.

		// shaderParameter���� graphics�� ������.

		// material�� �ؽ��ĸ� graphics�� ���� set�Ѵ�.
		if (m_pMaterial)
		{

		}
	}

	void BatchRenderer::Draw(View* pView)
	{
		if (!m_pMesh)
			return;

		Prepare(pView);
		m_pMesh->Draw(pView->GetGraphics());
	}
}