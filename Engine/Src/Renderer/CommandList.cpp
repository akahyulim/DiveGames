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

		// Viewports
		{
			UINT numViewports = 1;
			D3D11_VIEWPORT* pCurrentViewports = nullptr;
			pImmediateContext->RSGetViewports(&numViewports, pCurrentViewports);

			if (m_pPipelineState->pViewport != pCurrentViewports)
			{
				pImmediateContext->RSSetViewports(1, m_pPipelineState->pViewport);
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

		// RenderTargetViews
		{
			UINT numViews = 8;
			std::array<ID3D11RenderTargetView*, 8> currentRenderTargetViews = { nullptr };
			ID3D11DepthStencilView* pCurrentDepthStencilView = nullptr;
			pImmediateContext->OMGetRenderTargets(numViews, currentRenderTargetViews.data(), &pCurrentDepthStencilView);

			if ((currentRenderTargetViews != m_pPipelineState->renderTargetViews) || (pCurrentDepthStencilView != m_pPipelineState->pDepthStencilView))
			{
				UINT renderTargetCount = 0;
				for (auto pRenderTargetView : m_pPipelineState->renderTargetViews)
				{
					if (pRenderTargetView)
					{
						renderTargetCount++;
					}
				}

				// 이렇게까지 해도 리사이즈가 안되는건 이해가 안된다.
				// Scene을 로드하지 않았을 땐 문제가 발생하지 않는 걸 보니
				// 역시 RenderTarget의 Resize와 문제가 있는 것 같다...
				auto pRTV = m_pPipelineState->renderTargetViews[0];

				pImmediateContext->OMSetRenderTargets(
					//renderTargetCount,
					//m_pPipelineState->renderTargetViews.data(),
					//m_pPipelineState->pDepthStencilView
					1,
					&pRTV,
					nullptr
				);
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