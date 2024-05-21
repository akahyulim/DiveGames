#pragma once
#include "GraphicsDefs.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class Shader;
	class Pipeline;

	constexpr size_t MAX_NUM_RENDERTARGETS = 4;

	class Graphics
	{
	public:
		Graphics(const Graphics&) = delete;
		void operator=(const Graphics&) = delete;

		static Graphics* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Graphics;

			return s_pInstance;
		}

		bool Initialize(uint32_t width, uint32_t height, bool fullScreen, bool borderless);
		void Shutdown();

		bool RunWindow();

		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void AdjustWindow(uint32_t width, uint32_t height, bool borderless);

		bool BeginFrame();
		void EndFrame();

		void SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pRenderTargetView);
		void SetDepthStencilView(ID3D11DepthStencilView* pDepthStencilView);
		void ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth = 1.0f, uint8_t stencil = 0);

		void SetViewport(const RECT& rt);

		void SetDepthStencilState(ID3D11DepthStencilState* pState);
		void SetRasterizerState(ID3D11RasterizerState* pState);
		void SetBlendState(ID3D11BlendState* pState);

		void SetTexture(eTextureUnit index, Texture* pTexture);

		void SetShader(eShaderType type, Shader* pShader);

		void SetVertexBuffer(VertexBuffer* pBuffer);
		void SetIndexBuffer(IndexBuffer* pBuffer);

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t vertexCount, uint32_t vertexStart);
		void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t indexCount, uint32_t indexStart);

		bool IsInitialized();

		HINSTANCE GetWindowInstance() { return m_hInstance; }
		HWND GetWindowHandle() { return m_hWnd; }

		std::wstring GetWindowTitle() { return m_WindowTitle; }
		void SetWindowTitle(const std::wstring& title);

		bool IsFullScreen() { return m_bFullScreen; }
		void SetFullScreen(bool enabled);

		uint32_t GetResolutionWidth() { return m_ResolutionWidth; };
		uint32_t GetResolutionHeight() { return m_ResolutionHeight; }
		DirectX::XMFLOAT2 GetResolution() { return { (float)m_ResolutionWidth, (float)m_ResolutionHeight }; }
		void ResizeResolution(uint32_t width, uint32_t height);

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }
		ID3D11DepthStencilView* GetDefaultDepthStencilView() { return m_pDefaultDepthStencilView; }
		ID3D11Texture2D* GetDefaultDepthTexture() { return m_pDefaultDepthTexture; }

		bool IsVSync() { return m_bVSync; }
		void SetVSync(bool enabled) { m_bVSync = enabled; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return m_PrimitiveTopology; }

	private:
		Graphics();
		~Graphics();

		bool createWindow(uint32_t width, uint32_t height, bool borderless);
		bool createDevice(uint32_t width, uint32_t height);
		bool updateSwapChain(uint32_t width, uint32_t height);

		void prepareDraw();
		void resetViews();


	private:
		static Graphics* s_pInstance;

		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_WindowTitle;
		bool m_bFullScreen;
		uint32_t m_ResolutionWidth;
		uint32_t m_ResolutionHeight;

		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11RenderTargetView* m_pDefaultRenderTargetView;
		ID3D11DepthStencilView* m_pDefaultDepthStencilView;
		ID3D11Texture2D* m_pDefaultDepthTexture;

		bool m_bVSync;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

		ID3D11RenderTargetView* m_RenderTargetViews[MAX_NUM_RENDERTARGETS];
		ID3D11DepthStencilView* m_pDepthStencilView;

		bool m_bRenderTargetViewsDirty;

		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;

		Shader* m_pVertexShader;
		Shader* m_pHullShader;
		Shader* m_pDomainShader;
		Shader* m_pComputeShader;
		Shader* m_pPixelShader;

		// ConstantBuffer를 vs, ps별 배열 즉, 이차원 배열로 관리한다.

		ID3D11DepthStencilState* m_pDepthStencilState;
		ID3D11RasterizerState* m_pRasterizerState;
		ID3D11BlendState* m_pBlendState;

		ID3D11ShaderResourceView* m_ShaderResourceViews[(uint32_t)eTextureUnit::Count];
		ID3D11SamplerState* m_SamplerStates[(uint32_t)eTextureUnit::Count];

		bool m_bTextureDirty;
	};

	Graphics* GetGraphics();
}