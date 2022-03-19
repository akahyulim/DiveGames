#include "divepch.h"
#include "RenderPath.h"
#include "PipelineState.h"
#include "CommandList.h"
#include "Renderer.h"

namespace Dive
{
	void RenderPath::Update(float delta)
	{
	}
	
	// active scene�� �־�� �Ѵ�.
	void RenderPath::Render()
	{
		// ���� state �񱳿�
		CommandList cl;

		passDefault(&cl);
	}

	void RenderPath::passDefault(CommandList* pCl)
	{
		// pipeline state
		PipelineState ps;
		ps.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ps.pRasterizerState = Renderer::GetRasterizerState(eRasterizerStateType::CullBackSolid);
		ps.pVertexShader = Renderer::GetShader(eShaderType::Sprite)->pVertexShader;
		ps.pInputLayout = Renderer::GetShader(eShaderType::Sprite)->pInputLayout;
		ps.pPixelShader = Renderer::GetShader(eShaderType::Sprite)->pPixelShader;
		pCl->SetPipelineState(ps);

		// asset data
		{
			// command list�� asset data ����

			// draw
		}
	}
}