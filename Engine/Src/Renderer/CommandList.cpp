#include "divepch.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "Renderer.h"
#include "CommandList.h"

namespace Dive
{
	bool CommandList::SetPipelineState(PipelineState& ps)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return false;

		if (!ps.IsValid())
			return false;
		m_pPipelineState = &ps;

		// PrimitiveTopology
		{
			D3D11_PRIMITIVE_TOPOLOGY currentTopology;
			pImmediateContext->IAGetPrimitiveTopology(&currentTopology);

			if (m_pPipelineState->primitiveTopology != currentTopology)
			{
				pImmediateContext->IASetPrimitiveTopology(m_pPipelineState->primitiveTopology);
			}
		}

		// InputLayout
		{
			ID3D11InputLayout* pCurrentInputLayout = nullptr;
			pImmediateContext->IAGetInputLayout(&pCurrentInputLayout);

			if (m_pPipelineState->pInputLayout != pCurrentInputLayout)
			{
				pImmediateContext->IASetInputLayout(m_pPipelineState->pInputLayout);
			}
		}

		// VertexShader
		{
			ID3D11VertexShader* pCurrentVertexShader = nullptr;
			ID3D11ClassInstance* pClassInstances[256];
			UINT numClassInstances = 256;
			pImmediateContext->VSGetShader(&pCurrentVertexShader, pClassInstances, &numClassInstances);

			if (m_pPipelineState->pVertexShader != pCurrentVertexShader)
			{
				pImmediateContext->VSSetShader(m_pPipelineState->pVertexShader, nullptr, 0);
			}
		}

		// RasterizerState
		{
			ID3D11RasterizerState* pCurrentState = nullptr;
			pImmediateContext->RSGetState(&pCurrentState);

			if (m_pPipelineState->pRasterizerState != pCurrentState)
			{
				pImmediateContext->RSSetState(m_pPipelineState->pRasterizerState);
			}
		}

		// PixelShader
		{

			ID3D11PixelShader* pCurrentPixelShader = nullptr;
			ID3D11ClassInstance* pClassInstances[256];
			UINT numClassInstances = 256;
			pImmediateContext->PSGetShader(&pCurrentPixelShader, pClassInstances, &numClassInstances);

			if (m_pPipelineState->pPixelShader != pCurrentPixelShader)
			{
				pImmediateContext->PSSetShader(m_pPipelineState->pPixelShader, nullptr, 0);
			}
		}

		// DepthStencilState
		{
			UINT stencilRef = 0;
			ID3D11DepthStencilState* pCurrentState = nullptr;
			pImmediateContext->OMGetDepthStencilState(&pCurrentState, &stencilRef);

			if (m_pPipelineState->pDepthStencilState != pCurrentState)
			{
				pImmediateContext->OMSetDepthStencilState(m_pPipelineState->pDepthStencilState, 0);
			}
		}

		return true;
	}

	// 일단 전부 따로 받았지만
	// Buffer와 stride는 구조체 혹은 클래스화를 생각해보자.
	void CommandList::SetVertexBuffer(ID3D11Buffer* pVertexBuffer, const UINT strides, const UINT offsets)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return;

		ID3D11Buffer* pCurrentBuffer = nullptr;
		UINT currentStrides = 0;
		UINT currentOffsets = 0;
		pImmediateContext->IAGetVertexBuffers(0, 1, &pCurrentBuffer, &currentStrides, &currentOffsets);

		if ((pVertexBuffer == pCurrentBuffer) && (offsets == currentOffsets))
			return;

		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &strides, &offsets);
	}
}