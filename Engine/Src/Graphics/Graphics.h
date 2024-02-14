#pragma once
#include "GraphicsDefs.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class Shader;

	class Graphics
	{
	public:
		static bool Initialize(HWND hWnd, uint32_t width, uint32_t height, bool fullScreen);
		static void Shutdown();

		static bool UpdateSwapChain(uint32_t width = 0, uint32_t height = 0);

		static IDXGISwapChain* GetSwapChain() { return s_pSwapChain; }
		static ID3D11Device* GetDevice() { return s_pDevice; }
		static ID3D11DeviceContext* GetDeviceContext() { return s_pDeviceContext; }

		static ID3D11RenderTargetView* GetDefaultRenderTargetView() { return s_pDefaultRenderTargetView; }
		static ID3D11DepthStencilView* GetDefaultDepthStencilView() { return s_pDefaultDepthStencilView; }
		static ID3D11Texture2D* GetDefaultDepthTexture() { return s_pDefaultDepthTexture; }

		static uint32_t GetBackbufferWidth() { return s_BackbufferWidth; }
		static uint32_t GetBackbufferHeight() { return s_BackbufferHeight; }
		static DirectX::XMFLOAT2 GetBackbufferSize() { return { (float)s_BackbufferWidth, (float)s_BackbufferHeight }; }

		static bool IsVSync() { return s_bVSync; }
		static void SetVSync(bool enabled) { s_bVSync = enabled; }

		static bool IsInitialized();

		static bool BeginFrame();
		static void EndFrame();

		static void ClearViews(uint8_t flags, const float* pColor, float depth, uint8_t stencil);
		static void SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pRenderTargetView);
		static void SetDepthStencilView(ID3D11DepthStencilView* pDepthStencilView);
		static void SetViewport(const RECT& rt);
		static void SetDepthStencilState(ID3D11DepthStencilState* pState);
		static void SetRasterizerState(ID3D11RasterizerState* pState);
		static void SetBlendState(ID3D11BlendState* pState);
		static void SetTexture(eTextureUnit index, Texture* pTexture);
		static void SetShader(Shader* pShader);
		static void SetVertexBuffer(VertexBuffer* pBuffer);
		static void SetIndexBuffer(IndexBuffer* pBuffer);
		static void Draw(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t vertexCount, uint32_t vertexStart);
		static void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t indexCount, uint32_t indexStart);

		static D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return s_PrimitiveTopology; }

	private:
		static void prepareDraw();
		static void resetViews();


	private:
		static IDXGISwapChain* s_pSwapChain;
		static ID3D11Device* s_pDevice;
		static ID3D11DeviceContext* s_pDeviceContext;

		static ID3D11RenderTargetView* s_pDefaultRenderTargetView;
		static ID3D11DepthStencilView* s_pDefaultDepthStencilView;
		static ID3D11Texture2D* s_pDefaultDepthTexture;

		static uint32_t s_BackbufferWidth;
		static uint32_t s_BackbufferHeight;

		static bool s_bVSync;

		static D3D11_PRIMITIVE_TOPOLOGY s_PrimitiveTopology;

		static ID3D11RenderTargetView* s_RenderTargetViews[4];
		static ID3D11DepthStencilView* s_pDepthStencilView;

		static bool s_bRenderTargetViewsDirty;

		static VertexBuffer* s_pVertexBuffer;
		static IndexBuffer* s_pIndexBuffer;

		static Shader* s_pShader;
		
		// ConstantBuffer를 vs, ps별 배열 즉, 이차원 배열로 관리한다.

		static ID3D11DepthStencilState* s_pDepthStencilState;
		static ID3D11RasterizerState* s_pRasterizerState;
		static ID3D11BlendState* s_pBlendState;

		static ID3D11ShaderResourceView* s_ShaderResourceViews[(uint32_t)eTextureUnit::Max_Num];
		static ID3D11SamplerState* s_SamplerStates[(uint32_t)eTextureUnit::Max_Num];

		static bool s_bTextureDirty;
	};
}