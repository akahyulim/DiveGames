#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
	class InputLayout;
	class Texture2D;

	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	enum eWindowFlags : unsigned int
	{
		WINDOW_FULLSCREEN = 1,
		WINDOW_BORDERLESS = 1 << 1,
		WINDOW_RESIZABLE = 1 << 2
	};

	struct ScreenModeParams
	{
		bool operator==(const ScreenModeParams& rhs) const
		{
			return bFullScreen == rhs.bFullScreen &&
				bBorderless == rhs.bBorderless &&
				bResizable == rhs.bResizable &&
				bVSync == rhs.bVSync;
		}

		bool operator!=(const ScreenModeParams& rhs) const
		{
			return !(*this == rhs);
		}

		bool bFullScreen = false;
		bool bBorderless = false;
		bool bResizable = false;
		bool bVSync = false;
		//bool bTripleBuffer;
		//int multiSample
		//int refreshRate;
	};

	struct WindowModeParams
	{
		int width = 0;
		int height = 0;
		ScreenModeParams screenModeParams;
	};

	class Graphics : public Object
	{ 
		DIVE_OBJECT(Graphics, Object)

	public:
		explicit Graphics(Context* pContext);
		~Graphics() override;

		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);
		void Destroy();
		bool IsInitialized() const;

		// Window
		bool WindowCreate(int width, int height, unsigned int flags);
		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CloseWindow();
		
		bool RunWindow();
		
		HWND GetWindowHandle() const { return m_hWnd; }
		std::wstring GetWindowTitle() const { return m_WindowTitle; }
		
		void SetWindowTitle(const std::wstring& title);
		
		void GetWindowPosition(int& outX, int& outY) const;
		void SetWindowPosition(int x, int y);
		
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		
		DirectX::XMINT2 GetSize() const { return DirectX::XMINT2(m_Width, m_Height); }
		void ResizeWindow(int width, int height);
		
		void ShowWindow();
		void HideWindow();
		
		void SetFullScreenWindow(bool bFullScreen);
		void SetBorderlessWindow(bool bBorderelss);
		
		bool IsFullScreen() const;
		bool IsBoderlessWindow() const;
		bool IsResizable() const;
		
		// DX11 Graphics & Resources
		bool CreateDeviceAndSwapChain(unsigned int width = 0, unsigned int height = 0);

		bool UpdateSwapChain(unsigned int width = 0, unsigned int height = 0);

		bool IsDeviceLost();

		bool BeginFrame();
		void EndFrame();

		void Clear(int flags, const DirectX::XMFLOAT4& color, float depth, int stencil);

		// Draw 함수들: 총 5개
		void Draw(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int vertexCount, unsigned int vertexStart);
		void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int indexCount, unsigned int indexStart, unsigned int baseVertexIndex = 0);
		void DrawIndexedInstanced(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int indexCount, unsigned int instanceCount, unsigned int indexStart);

		void SetDepthStencil(Texture2D* pTexture);

		ID3D11RenderTargetView* GetRenderTarget(unsigned int index) const;
		void SetRenderTarget(unsigned int index, Texture2D* pTexture);
		void ResetRenderTargets();

		D3D11_VIEWPORT GetViewport() const { return m_Viewport; }
		RECT GetViewportRect() const;
		void SetViewport(const D3D11_VIEWPORT& viewport);
		void SetViewportRect(const RECT& rect);

		VertexBuffer* GetVertexBuffer(unsigned int index) const;
		void SetVertexBuffer(VertexBuffer* pBuffer);
		void SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, unsigned int instanceOffset = 0);

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer);

		Shader* GetShader(eShaderType type, const std::string& name, const std::string& defines = std::string()) const;
		void SetShaders(Shader* pVertexShader, Shader* pPixelShader);

		// 기타: 얘네들을 이용해 States를 실시간으로 생성하는 듯 하다.
		// SetFillMode		fill_mode
		// SetLineAntiAlias	bool
		// SetClipPlane		bool
		// SetColorWhite?	bool
		// SetDepthBias		float, float
		// SetScissorTest	bool
		// SetStencilTest	bool

		ID3D11Device* GetDevice() { return m_pDevice.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext.Get(); }

		// 임시
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRTV.Get(); }

		bool LoadShaders();
		void SetDefaultShader();

		Shader* GetDefaultVS() const { return m_pDefaultVS; }

		// 윈도우 크기 변경시 호출. 백버퍼, 렌더타겟 재생성 함수 호출.
		void OnResizeWindow();

	private:
		void prepareDraw();

	private:
		HINSTANCE m_hInstance = NULL;
		HWND m_hWnd = NULL;
		std::wstring m_WindowTitle = L"DIVE";
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		DWORD m_WindowStyles = 0;
		unsigned int m_WindowFlags = 0;
		int m_Width = 800;
		int m_Height = 600;


		// 디폴트 모드 같은데, 초기화 및 설정하는 부분을 찾지 못했다.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// DX11 Device & Default Resources
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pDefaultRTV = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDefaultDS = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDefaultDSV = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		D3D11_VIEWPORT m_Viewport;

		Texture2D* m_pRenderTargets[MAX_RENDERTARGETS] = { nullptr, };
		Texture2D* m_pDepthStencil = nullptr;

		ID3D11RenderTargetView* m_pCurRenderTargetViews[MAX_RENDERTARGETS] = { nullptr, };
		ID3D11DepthStencilView* m_pCurDepthStencilView = nullptr;

		VertexBuffer* m_pVertexBuffers[MAX_VERTEX_STREAMS] = { nullptr, };
		IndexBuffer* m_pIndexBuffer = nullptr;;

		Shader* m_pVertexShader = nullptr;
		Shader* m_pPixelShader = nullptr;

		// dirty check 전부 모으기.
		bool m_bRenderTargetsDirty = false;

		// temp
		Shader* m_pDefaultVS = nullptr;
		Shader* m_pDefaultPS = nullptr;
		InputLayout* m_pDefaultIL = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pMatrixBuffer;

		struct MatrixBufferType
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
		};

	};

	void RegisterGraphicsObject(Context* pContext);
}