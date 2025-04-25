#pragma once
#include "core/EventDispatcher.h"

namespace Dive
{
	class Shader;
	class RenderTexture;
	class VertexBuffer;
	class IndexBuffer;

	class Graphics
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void ChangeResolution(UINT32 width, UINT32 height);

		static void OnWindowResized(EventData data);

		static void Present();

		static void SetRenderTarget(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView);
		static void ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, const DirectX::XMFLOAT4& color);
		static void ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, uint8_t flags, float depth = 1.0f, uint8_t stencil = 0);


		static void SetViewport(const RECT& rt);
		static void SetViewport(float width, float height, float x = 0.0f, float y = 0.0f);

		static void SetDepthStencilState(ID3D11DepthStencilState* state);
		static void SetRasterizerState(ID3D11RasterizerState* state);
		static void SetBlendState(ID3D11BlendState* state);

		//static void SetTexture(eTextureUnit index, Texture* pTexture);

		static void SetVertexShader(Shader* shader);
		static void SetHullShader(Shader* shader);
		static void SetDomainShader(Shader* shader);
		static void SetGeometryShader(Shader* shader);
		static void SetPixelShader(Shader* shader);
		static void SetComputeShader(Shader* shader);

		static void SetVertexBuffer(VertexBuffer* buffer);
		static void SetIndexBuffer(IndexBuffer* buffer);

		static void Draw(D3D11_PRIMITIVE_TOPOLOGY topology, UINT32 vertexCount, UINT32 vertexStart);
		static void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, UINT32 indexCount, UINT32 indexStart);

		static IDXGISwapChain* GetSwapChain();
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetRenderTargetView();
		static ID3D11DepthStencilView* GetDepthStencilView();

		static UINT32 GetResolutionWidth() { return s_ResolutionWidth; }
		static UINT32 GetResolutionHeight() { return s_ResolutionHeight; }

		static bool IsVSyncEnabled() { return s_UseVSync; }
		static void SetVSyncEnabled(bool enabled) { s_UseVSync = enabled; }

	private:
		static void updateBackbuffer();

	private:
		static UINT32 s_ResolutionWidth;
		static UINT32 s_ResolutionHeight;
		static bool s_UseVSync;

		static IDXGISwapChain* s_SwapChain;
		static ID3D11Device* s_Device;
		static ID3D11DeviceContext* s_DeviceContext;

		static ID3D11RenderTargetView* s_BackbufferRenderTarget;
		static ID3D11Texture2D* s_BackbufferTexture;
		static ID3D11DepthStencilView* s_BackbufferDepthStencil;

		static ID3D11DepthStencilView* s_CurrentDepthStencil;

		static ID3D11DepthStencilState* s_CurrentDepthStencilState;
		static ID3D11RasterizerState* s_CurrentRasterizerState;
		static ID3D11BlendState* s_CurrentBlendState;

		static Shader* s_CurrentVertexShader;
		static Shader* s_CurrentHullShader;
		static Shader* s_CurrentDomainShader;
		static Shader* s_CurrentGeometryShader;
		static Shader* s_CurrentPixelShader;
		static Shader* s_CurrentComputeShader;

		static VertexBuffer* s_CurrentVertexBuffer;
		static IndexBuffer* s_CurrentIndexBuffer;

		static D3D11_PRIMITIVE_TOPOLOGY s_PrimitiveTopology;
	};
}
