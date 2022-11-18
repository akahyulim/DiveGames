#include "divepch.h"
#include "BatchRenderer.h"
#include "Graphics/Graphics.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "IO/Log.h"

namespace Dive
{
	// shader와 같이 개별 mesh에 적용되어야 할 리소스를 gpu로 보낸다.
	void BatchRenderer::Prepare(View* pView)
	{
		// 이건 임시다. Shader를 가지고 있어야 한다.
		pView->GetGraphics()->SetDefaultShader();

		// Pass를 이용해
		// graphics에 blend, cull, fill mode등을 전달한다.

		// shaderParameter들을 graphics에 보낸다.

		// material의 텍스쳐를 graphics를 통해 set한다.
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