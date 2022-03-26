#include "divepch.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "Renderer.h"
#include "CommandList.h"

namespace Dive
{
	bool CommandList::BindPipelineState(PipelineState& ps)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		if (!pImmediateContext)
			return false;

		if (!ps.IsValid())
			return false;
		
		// PrimitiveTopology
		{
			if(ps.primitiveTopology != m_OldStates.primitiveTopology)
			{
				pImmediateContext->IASetPrimitiveTopology(ps.primitiveTopology);
			}
		}

		// InputLayout
		{
			if (ps.pInputLayout != m_OldStates.pInputLayout)
			{
				pImmediateContext->IASetInputLayout(ps.pInputLayout);
			}
		}

		// VertexShader
		{
			if (ps.pVertexShader != m_OldStates.pVertexShader)
			{
				pImmediateContext->VSSetShader(ps.pVertexShader, nullptr, 0);
			}
		}

		// RasterizerState
		{
			if (ps.pRasterizerState != m_OldStates.pRasterizerState)
			{
				pImmediateContext->RSSetState(ps.pRasterizerState);
			}
		}

		// Viewports
		{
			if (ps.pViewport != m_OldStates.pViewport)
			{
				pImmediateContext->RSSetViewports(1, ps.pViewport);
			}
		}

		// PixelShader
		{
			if (ps.pPixelShader != m_OldStates.pPixelShader)
			{
				pImmediateContext->PSSetShader(ps.pPixelShader, nullptr, 0);
			}
		}

		// BelndState
		{
			// 추가 데이터가 필요하다.
			if (ps.pBlendState != m_OldStates.pBlendState)
			{
				//pImmediateContext->OMSetBlendState(ps.pBlendState, 0);
			}
		}

		// DepthStencilState
		{
			if (ps.pDepthStencilState != m_OldStates.pDepthStencilState)
			{
				pImmediateContext->OMSetDepthStencilState(ps.pDepthStencilState, 0);
			}
		}

		// RenderTargetViews
		{
			if ((ps.renderTargetViews != m_OldStates.renderTargetViews) || 
				(ps.pDepthStencilView != m_OldStates.pDepthStencilView))
			{
				UINT renderTargetCount = 0;
				for (auto pRenderTargetView : ps.renderTargetViews)
				{
					if (pRenderTargetView)
					{
						renderTargetCount++;
					}
				}

				pImmediateContext->OMSetRenderTargets(
					renderTargetCount,
					ps.renderTargetViews.data(),
					ps.pDepthStencilView
				);
			}
		}

		m_OldStates = ps;
		
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