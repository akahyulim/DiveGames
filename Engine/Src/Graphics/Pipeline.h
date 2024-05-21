#pragma once

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	
	constexpr uint8_t MAX_NUM_RENDER_VIEWS = 8;
	constexpr uint8_t MAX_NUM_CBUFFERS = 15;
	constexpr uint8_t MAX_NUM_SHADER_RESOURCES = 128;
	constexpr uint8_t MAX_NUM_SAMPLERS = 16;

	class Pipeline
	{
	public:
		Pipeline(ID3D11DeviceContext* pDeviceContext);
		~Pipeline() = default;

		void Clear();

		void BindRenderTargetView(ID3D11RenderTargetView* pRtv, size_t index);
		void BindDepthStencilView(ID3D11DepthStencilView* pDsv);
		void BindDepthStencilState(ID3D11DepthStencilState* pState);
		
		void BindRasterizerState(ID3D11RasterizerState* pState);
		void BindViewport(D3D11_VIEWPORT viewport);
		void BindViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);

		void BindBlendState(ID3D11BlendState* pState);

		void BindVertexBuffer(ID3D11Buffer* pBuffer, uint32_t stride, uint32_t offset);
		void BindVertexBuffer(VertexBuffer* pBuffer);
		void BindIndexBuffer(ID3D11Buffer* pBuffer, DXGI_FORMAT format, uint32_t offset);
		void BindIndexBuffer(IndexBuffer* pBuffer);

		void BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

		void BindInputLayout(ID3D11InputLayout* pInputLayout);

		void BindVertexShader(ID3D11VertexShader* pVertexShader);
		void BindVertexShaderCBuffer(ID3D11Buffer* pBuffer, size_t index);

		void BindPixelShader(ID3D11PixelShader* pPixelShader);
		void BindPixelShaderCBuffer(ID3D11Buffer* pBuffer, size_t index);
		void BindPixelShaderResourceView(ID3D11ShaderResourceView* pView, size_t index);
		void BindPixelShaderSampler(ID3D11SamplerState* pState, size_t index);

		void SetPipeline();

		void ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil);

		void Draw(uint32_t vertexCount, uint32_t vertexStart = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t indexStart = 0);

	private:

	private:
		ID3D11DeviceContext* m_pDeviceContext;

		//render targets
		ID3D11RenderTargetView* m_RenderTargetViews[MAX_NUM_RENDER_VIEWS];
		ID3D11DepthStencilView* m_pDepthStencilView;
		bool m_bRenderTargetsDirty;

		// depth stencil state
		ID3D11DepthStencilState* m_pDepthStencilState;
		bool m_bDepthStencilStateDirty;
		
		// rasterizer state
		ID3D11RasterizerState* m_pRasterizerState;
		bool m_bRasterizerStateDirty;

		D3D11_VIEWPORT m_Viewport;
		bool m_bViewportDirty;
		
		// blender state
		ID3D11BlendState* m_pBlendState;
		bool m_bBlendStateDirty;

		// input layout
		ID3D11InputLayout* m_pInputLayout;
		bool m_bInputLayoutDirty;

		// vertex shader
		ID3D11VertexShader* m_pVertexShader;
		bool m_bVertexShaderDirty;

		// vertex shader constant buffers
		ID3D11Buffer* m_VertexShaderCBuffers[MAX_NUM_CBUFFERS];
		bool m_bVertexShaderCBuffersDirty;

		// pixel shader
		ID3D11PixelShader* m_pPixelShader;
		bool m_bPixelShaderDirty;

		// pixel shader constant buffers
		ID3D11Buffer* m_PixelShaderCBuffers[MAX_NUM_CBUFFERS];
		bool m_bPixelShaderCBuffersDirty;

		// pixl shader resources & sampler states
		ID3D11ShaderResourceView* m_PixelShaderResourceViews[MAX_NUM_SHADER_RESOURCES];
		bool m_bPixelShaderResourcesDirty;

		ID3D11SamplerState* m_PixelShaderSamplers[MAX_NUM_SAMPLERS];
		bool m_bPixelShaderSamplerDirty;

		// vertex buffer
		ID3D11Buffer* m_pVertexBuffer;
		uint32_t m_VertexCount;
		uint32_t m_VertexBufferStride;
		uint32_t m_VertexBufferOffset;
		bool m_bVertexBufferDirty;
		
		// index buffer
		ID3D11Buffer* m_pIndexBuffer;
		uint32_t m_IndexCount;
		DXGI_FORMAT m_IndexBufferFormat;
		uint32_t m_IndexBufferOffset;
		bool m_bIndexBufferDirty;

		// primirive topology
		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
		bool m_bPrimitiveTopologyDirty;
	};
}