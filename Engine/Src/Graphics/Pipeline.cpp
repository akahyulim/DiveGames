#include "divepch.h"
#include "Pipeline.h"
#include "Core/CoreDefs.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GraphicsDefs.h"

namespace Dive
{
	Pipeline::Pipeline(ID3D11DeviceContext* pDeviceContext)
		: m_pDeviceContext(pDeviceContext)
	{
		Clear();
	}

	void Pipeline::Clear()
	{
		for (uint8_t i = 0; i != MAX_NUM_RENDER_VIEWS; ++i)
			m_RenderTargetViews[i] = nullptr;
		m_pDepthStencilView = nullptr;
		m_bRenderTargetsDirty = true;

		m_pDepthStencilState = nullptr;
		m_bDepthStencilStateDirty = true;

		m_pRasterizerState = nullptr;
		m_bRasterizerStateDirty = true;

		m_Viewport = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		m_bViewportDirty = true;

		m_pBlendState = nullptr;
		m_bBlendStateDirty = true;

		m_pInputLayout = nullptr;
		m_bInputLayoutDirty = true;

		m_pVertexShader = nullptr;
		m_bVertexShaderDirty = true;

		for (uint8_t i = 0; i != MAX_NUM_CBUFFERS; ++i)
			m_VertexShaderCBuffers[i] = nullptr;
		m_bVertexShaderCBuffersDirty = true;

		m_pPixelShader = nullptr;
		m_bPixelShaderDirty = true;

		for (uint8_t i = 0; i != MAX_NUM_CBUFFERS; ++i)
			m_PixelShaderCBuffers[i] = nullptr;
		m_bPixelShaderCBuffersDirty = true;

		for (uint8_t i = 0; i != MAX_NUM_SHADER_RESOURCES; ++i)
			m_PixelShaderResourceViews[i] = nullptr;
		m_bPixelShaderResourcesDirty = true;

		for (uint8_t i = 0; i != MAX_NUM_SAMPLERS; ++i)
			m_PixelShaderSamplers[i] = nullptr;
		m_bPixelShaderSamplerDirty = true;

		m_pVertexBuffer = nullptr;
		m_VertexCount = 0;
		m_VertexBufferStride = 0;
		m_VertexBufferOffset = 0;
		m_bVertexBufferDirty = true;

		m_pIndexBuffer = nullptr;
		m_IndexCount = 0;
		m_IndexBufferFormat = DXGI_FORMAT_UNKNOWN;
		m_IndexBufferOffset = 0;
		m_bIndexBufferDirty = true;

		m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		m_bPrimitiveTopologyDirty = true;
	}

	void Pipeline::BindRenderTargetView(ID3D11RenderTargetView* pRtv, size_t index)
	{
		if (index >= MAX_NUM_RENDER_VIEWS)
		{
			DV_ENGINE_ERROR("잘못된 렌더 타겟 뷰의 슬롯 인덱스({:d})를 전달받아 바인딩에 실패하였습니다.", index);
			return;
		}

		if (pRtv != m_RenderTargetViews[index])
		{
			m_RenderTargetViews[index] = pRtv;
			m_bRenderTargetsDirty = true;
		}
	}

	void Pipeline::BindDepthStencilView(ID3D11DepthStencilView* pDsv)
	{
		if (pDsv != m_pDepthStencilView)
		{
			m_pDepthStencilView = pDsv;
			m_bRenderTargetsDirty = true;
		}
	}

	void Pipeline::BindDepthStencilState(ID3D11DepthStencilState* pState)
	{
		if (pState != m_pDepthStencilState)
		{
			m_pDepthStencilState = pState;
			m_bDepthStencilStateDirty = true;
		}
	}
	
	void Pipeline::BindRasterizerState(ID3D11RasterizerState* pState)
	{
		if (pState != m_pRasterizerState)
		{
			m_pRasterizerState = pState;
			m_bRasterizerStateDirty = true;
		}
	}

	void Pipeline::BindViewport(D3D11_VIEWPORT viewport)
	{
		if (m_Viewport.TopLeftX != viewport.TopLeftX || m_Viewport.TopLeftY != viewport.TopLeftY || m_Viewport.Width != viewport.Width
			|| m_Viewport.Height != viewport.Height || m_Viewport.MinDepth != viewport.MinDepth || m_Viewport.MaxDepth != viewport.MaxDepth)
		{
			m_Viewport = viewport;
			m_bViewportDirty = true;
		}
	}

	void Pipeline::BindViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
	{
		if (m_Viewport.TopLeftX != topLeftX || m_Viewport.TopLeftY != topLeftY || m_Viewport.Width != width
			|| m_Viewport.Height != height || m_Viewport.MinDepth != minDepth || m_Viewport.MaxDepth != maxDepth)
		{
			m_Viewport = { topLeftX, topLeftY, width, height, minDepth, maxDepth };
			m_bViewportDirty = true;
		}
	}
	
	void Pipeline::BindBlendState(ID3D11BlendState* pState)
	{
		if (pState != m_pBlendState)
		{
			m_pBlendState = pState;
			m_bBlendStateDirty = true;
		}
	}

	void Pipeline::BindVertexBuffer(ID3D11Buffer* pBuffer, uint32_t stride, uint32_t offset)
	{
		if (pBuffer != m_pVertexBuffer || stride != m_VertexBufferStride || offset != m_VertexBufferOffset)
		{
			m_pVertexBuffer = pBuffer;
			m_VertexBufferStride = stride;
			m_VertexBufferOffset = offset;
			m_bVertexBufferDirty = true;
		}
	}
	
	void Pipeline::BindVertexBuffer(VertexBuffer* pBuffer)
	{
		if (!pBuffer)
			BindVertexBuffer(nullptr, 0, 0);
		else
			BindVertexBuffer(pBuffer->GetBufferNew(), *pBuffer->GetStride(), 0);
	}
	
	void Pipeline::BindIndexBuffer(ID3D11Buffer* pBuffer, DXGI_FORMAT format, uint32_t offset)
	{
		if (pBuffer != m_pIndexBuffer || format != m_IndexBufferFormat || offset != m_IndexBufferOffset)
		{
			m_pIndexBuffer = pBuffer;
			m_IndexBufferFormat = format;
			m_IndexBufferOffset = offset;
			m_bIndexBufferDirty = true;
		}
	}
	
	void Pipeline::BindIndexBuffer(IndexBuffer* pBuffer)
	{
		if (!pBuffer)
			BindIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		else
			BindIndexBuffer(pBuffer->GetBuffer(), pBuffer->GetFormat(), 0);
	}

	void Pipeline::BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (topology != m_PrimitiveTopology)
		{
			m_PrimitiveTopology = topology;
			m_bPrimitiveTopologyDirty = true;
		}
	}

	void Pipeline::BindInputLayout(ID3D11InputLayout* pInputLayout)
	{
		if (pInputLayout != m_pInputLayout)
		{
			m_pInputLayout = pInputLayout;
			m_bInputLayoutDirty = true;
		}
	}

	void Pipeline::BindVertexShader(ID3D11VertexShader* pVertexShader)
	{
		if(pVertexShader != m_pVertexShader)
		{
			m_pVertexShader = pVertexShader;
			m_bVertexShaderDirty = true;
		}
	}

	void Pipeline::BindVertexShaderCBuffer(ID3D11Buffer* pBuffer, size_t index)
	{
		if (index >= MAX_NUM_CBUFFERS)
		{
			DV_ENGINE_ERROR("잘못된 정점 셰이더 상수 버퍼 슬롯 인덱스({:d})를 전달받아 바인딩에 실패하였습니다.", index);
			return;
		}

		if (pBuffer != m_VertexShaderCBuffers[index])
		{
			m_VertexShaderCBuffers[index] = pBuffer;
			m_bVertexShaderCBuffersDirty = true;
		}
	}

	void Pipeline::BindPixelShader(ID3D11PixelShader* pPixelShader)
	{
		if (pPixelShader != m_pPixelShader)
		{
			m_pPixelShader = pPixelShader;
			m_bPixelShaderDirty = true;
		}
	}

	void Pipeline::BindPixelShaderCBuffer(ID3D11Buffer* pBuffer, size_t index)
	{
		if (index >= MAX_NUM_CBUFFERS)
		{
			DV_ENGINE_ERROR("잘못된 픽셀 셰이더 상수 버퍼 슬롯 인덱스({:d})를 전달받아 바인딩에 실패하였습니다.", index);
			return;

		}
		if (pBuffer != m_PixelShaderCBuffers[index])
		{
			m_PixelShaderCBuffers[index] = pBuffer;
			m_bPixelShaderCBuffersDirty = true;
		}
	}

	void Pipeline::BindPixelShaderResourceView(ID3D11ShaderResourceView* pView, size_t index)
	{
		if (pView != m_PixelShaderResourceViews[index])
		{
			m_PixelShaderResourceViews[index] = pView;
			m_bPixelShaderResourcesDirty = true;
		}
	}

	void Pipeline::BindPixelShaderSampler(ID3D11SamplerState* pSampler, size_t index)
	{
		if (pSampler != m_PixelShaderSamplers[index])
		{
			m_PixelShaderSamplers[index] = pSampler;
			m_bPixelShaderResourcesDirty = true;
		}
	}

	void Pipeline::SetPipeline()
	{
		DV_ENGINE_ASSERT(m_pDeviceContext);

		// input assembler stage
		if (m_bPrimitiveTopologyDirty)
		{
			m_pDeviceContext->IASetPrimitiveTopology(m_PrimitiveTopology);
			m_bPrimitiveTopologyDirty = false;
		}

		if (m_bInputLayoutDirty)
		{
			m_pDeviceContext->IASetInputLayout(m_pInputLayout);
			m_bInputLayoutDirty = false;
		}

		if (m_bVertexBufferDirty)
		{
			m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
			m_bVertexBufferDirty = false;
		}

		if (m_bIndexBufferDirty)
		{
			m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, m_IndexBufferFormat, m_IndexBufferOffset);
		}

		// vertex shader stage
		if (m_bVertexShaderDirty)
		{
			m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
			m_bVertexShaderDirty = false;
		}

		if (m_bVertexShaderCBuffersDirty)
		{
			m_pDeviceContext->VSSetConstantBuffers(0, MAX_NUM_CBUFFERS, m_VertexShaderCBuffers);
			m_bVertexShaderCBuffersDirty = false;
		}

		// rasterizer stage
		if (m_bRasterizerStateDirty)
		{
			m_pDeviceContext->RSSetState(m_pRasterizerState);
			m_bRasterizerStateDirty = false;
		}

		if (m_bViewportDirty)
		{
			m_pDeviceContext->RSSetViewports(1, &m_Viewport);
			m_bViewportDirty = false;
		}

		// pixel shader stage
		if (m_bPixelShaderDirty)
		{
			m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
			m_bPixelShaderDirty = false;
		}

		if (m_bPixelShaderCBuffersDirty)
		{
			m_pDeviceContext->PSSetConstantBuffers(0, MAX_NUM_CBUFFERS, m_PixelShaderCBuffers);
			m_bPixelShaderCBuffersDirty = false;
		}

		if (m_bPixelShaderResourcesDirty)
		{
			m_pDeviceContext->PSSetShaderResources(0, MAX_NUM_SHADER_RESOURCES, m_PixelShaderResourceViews);
			m_bPixelShaderResourcesDirty = false;
		}

		if (m_bPixelShaderSamplerDirty)
		{
			m_pDeviceContext->PSSetSamplers(0, MAX_NUM_SAMPLERS, m_PixelShaderSamplers);
			m_bPixelShaderSamplerDirty = false;
		}
		
		// out merge stage
		if (m_bDepthStencilStateDirty)
		{
			m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
			m_bDepthStencilStateDirty = false;
		}

		if (m_bBlendStateDirty)
		{
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pDeviceContext->OMSetBlendState(m_pBlendState, blendFactor, 0xffffffff);
			m_bBlendStateDirty = false;
		}

		if (m_bRenderTargetsDirty)
		{
			m_pDeviceContext->OMSetRenderTargets(MAX_NUM_RENDER_VIEWS, m_RenderTargetViews, m_pDepthStencilView);
			m_bRenderTargetsDirty = false;
		}
	}

	void Pipeline::ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		//SetPipeline();
		if (m_bRenderTargetsDirty)
		{
			m_pDeviceContext->OMSetRenderTargets(MAX_NUM_RENDER_VIEWS, m_RenderTargetViews, m_pDepthStencilView);
			m_bRenderTargetsDirty = false;
		}

		if (flags & eClearFlags::Color)
		{
			float clearColor[4] = { color.x, color.y, color.z, color.w };

			for (uint32_t i = 0; i < MAX_NUM_RENDER_VIEWS; ++i)
			{
				if (m_RenderTargetViews[i])
				{
					m_pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[i], clearColor);
				}
			}
		}

		if ((flags & eClearFlags::Depth) || (flags & eClearFlags::Stencil))
		{
			if (m_pDepthStencilView)
			{
				uint32_t clearFlags = 0;
				clearFlags |= (flags & eClearFlags::Depth) ? D3D11_CLEAR_DEPTH : 0;
				clearFlags |= (flags & eClearFlags::Stencil) ? D3D11_CLEAR_STENCIL : 0;


				m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, clearFlags, depth, stencil);
			}
		}
	}
	
	void Pipeline::Draw(uint32_t vertexCount, uint32_t vertexStart)
	{
		SetPipeline();

		m_pDeviceContext->Draw(vertexCount, vertexStart);
	}
	
	void Pipeline::DrawIndexed(uint32_t indexCount, uint32_t indexStart)
	{
		SetPipeline();

		m_pDeviceContext->DrawIndexed(indexCount, indexStart, 0);
	}
}