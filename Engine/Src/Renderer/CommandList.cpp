#include "divepch.h"
#include "CommandList.h"
#include "PipelineState.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/ConstantBuffer.h"

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
			// 두 번째 인자가 좀 애매하다.
			if (ps.pBlendState != m_OldStates.pBlendState)
			{
				pImmediateContext->OMSetBlendState(ps.pBlendState, NULL, 0xffffffff);
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

	void CommandList::SetVertexBuffer(VertexBuffer* pVertexBuffer, unsigned int offsets)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(pVertexBuffer != nullptr);
		DV_ASSERT(pVertexBuffer->GetBuffer() != nullptr);

		// current buffer
		ID3D11Buffer* pCurrentBuffer = nullptr;
		UINT currentStrides = 0;
		UINT currentOffsets = 0;
		pImmediateContext->IAGetVertexBuffers(0, 1, &pCurrentBuffer, &currentStrides, &currentOffsets);

		// input buffer
		ID3D11Buffer* pBuffer = pVertexBuffer->GetBuffer();
		UINT strides = pVertexBuffer->GetStride();

		if ((pBuffer == pCurrentBuffer) && (offsets == currentOffsets))
			return;

		// offset가 현재 MeshRenderer의 시작 부분이라 생각했는데, 아닌 듯 하다.
		// 실제로 좀 더 생각해보니, 시작부분이라 해도 현재 버퍼에서 어디까지 그려야 하는지 알 수 없다.
		// offset은 draw에 사용되네...?
		pImmediateContext->IASetVertexBuffers(0, 1, &pBuffer, &strides, &offsets);
	}

	void CommandList::SetIndexBuffer(IndexBuffer* pIndexBuffer, unsigned int offsets)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		DV_ASSERT(pIndexBuffer != nullptr);
		DV_ASSERT(pIndexBuffer->GetBuffer() != nullptr);
		
		// current buffer
		ID3D11Buffer* pCurrentBuffer	= nullptr;
		DXGI_FORMAT currentFormat		= DXGI_FORMAT_UNKNOWN;
		UINT currentOffsets				= 0;
		pImmediateContext->IAGetIndexBuffer(&pCurrentBuffer, &currentFormat, &currentOffsets);

		// input buffer
		ID3D11Buffer* pBuffer = pIndexBuffer->GetBuffer();
		UINT strides = pIndexBuffer->GetStride();

		if ((pBuffer == pCurrentBuffer) && (offsets == currentOffsets))
			return;

		pImmediateContext->IASetIndexBuffer(pBuffer, pIndexBuffer->GetFormat(), 0);// offsets);
	}

	void CommandList::SetConstantBuffer(uint8_t type, eConstantBufferSlot slot, ConstantBuffer* pConstantBuffer)
	{
		auto pImmediateContext = Renderer::GetGraphicsDevice().GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);
		// 이것들은 매개 변수를 잘못 전달받았다고 경고로 띄우는 게...?
		DV_ASSERT(pConstantBuffer != nullptr);
		DV_ASSERT(pConstantBuffer->GetBuffer() != nullptr);

		ID3D11Buffer* pBuffer = pConstantBuffer->GetBuffer();

		if (type & Scope_Vertex)
		{
			ID3D11Buffer* pCurrentBuffer = nullptr;
			pImmediateContext->VSGetConstantBuffers(static_cast<unsigned int>(slot), 1, &pCurrentBuffer);
			if (pBuffer != pCurrentBuffer)
			{
				pImmediateContext->VSSetConstantBuffers(static_cast<unsigned int>(slot), 1, &pBuffer);
			}
		}

		if (type & Scope_Pixel)
		{
			ID3D11Buffer* pCurrentBuffer = nullptr;
			pImmediateContext->PSGetConstantBuffers(static_cast<unsigned int>(slot), 1, &pCurrentBuffer);
			if (pBuffer != pCurrentBuffer)
			{
				pImmediateContext->PSSetConstantBuffers(static_cast<unsigned int>(slot), 1, &pBuffer);
			}
		}

		if (type & Scope_Compute)
		{
			ID3D11Buffer* pCurrentBuffer = nullptr;
			pImmediateContext->CSGetConstantBuffers(static_cast<unsigned int>(slot), 1, &pCurrentBuffer);
			if (pBuffer != pCurrentBuffer)
			{
				pImmediateContext->CSSetConstantBuffers(static_cast<unsigned int>(slot), 1, &pBuffer);
			}
		}
	}
}