#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
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

		bool IsInitialized() const;
		void Destroy();

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
		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);

		void Clear(int flags, const DirectX::XMFLOAT4& color, float depth, int stencil);

		bool IsDeviceLost();

		bool BeginFrame();
		void EndFrame();

		// Draw �Լ���: �� 5��
		void Draw(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int vertexCount, unsigned int vertexStart);
		void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int indexCount, unsigned int indexStart, unsigned int baseVertexIndex = 0);
		void DrawIndexedInstanced(D3D11_PRIMITIVE_TOPOLOGY type, unsigned int indexCount, unsigned int instanceCount, unsigned int indexStart);

		void SetDepthStencil(Texture2D* pTexture);

		ID3D11RenderTargetView* GetRenderTarget(unsigned int index) const;
		void SetRenderTarget(unsigned int index, Texture2D* pTexture);

		VertexBuffer* GetVertexBuffer(unsigned int index) const;
		void SetVertexBuffer(VertexBuffer* pBuffer);
		void SetVertexBuffers(const std::vector<VertexBuffer*>& buffers, unsigned int instanceOffset = 0);

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer);

		Shader* GetShader(eShaderType type, const std::string& name, const std::string& defines = std::string()) const;
		void SetShaders(Shader* pVertexShader, Shader* pPixelShader);

		// ��Ÿ: ��׵��� �̿��� States�� �ǽð����� �����ϴ� �� �ϴ�.
		// SetFillMode		fill_mode
		// SetLineAntiAlias	bool
		// SetClipPlane		bool
		// SetColorWhite?	bool
		// SetDepthBias		float, float
		// SetScissorTest	bool
		// SetStencilTest	bool

		ID3D11Device* GetDevice() { return m_pDevice.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext.Get(); }

		// �ӽ�
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRTV.Get(); }


		// ������ ũ�� ����� ȣ��. �����, ����Ÿ�� ����� �Լ� ȣ��.
		void OnResizeWindow();

	private:
		bool createDeviceAndSwapChain(int width, int height);
		bool updateSwapChain(int width, int height);
		void prepareDraw();

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_WindowTitle;
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		DWORD m_WindowStyles;
		unsigned int m_WindowFlags;
		int m_Width;
		int m_Height;


		// ����Ʈ ��� ������, �ʱ�ȭ �� �����ϴ� �κ��� ã�� ���ߴ�.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// DX11 Device & Default Resources
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pDefaultRTV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDefaultDS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDefaultDSV;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType;

		Texture2D* m_pRenderTargets[MAX_RENDERTARGETS] = { nullptr, };
		Texture2D* m_pDepthStencil = nullptr;

		ID3D11RenderTargetView* m_pCurRenderTargetViews[MAX_RENDERTARGETS] = { nullptr, };
		ID3D11DepthStencilView* m_pCurDepthStencilView = nullptr;

		VertexBuffer* m_pVertexBuffers[MAX_VERTEX_STREAMS] = { nullptr, };
		IndexBuffer* m_pIndexBuffer = nullptr;;

		Shader* m_pVertexShader = nullptr;
		Shader* m_pPixelShader = nullptr;

		// dirty check ���� ������.
		bool m_bRenderTargetsDirty = false;
	};

	void RegisterGraphicsObject(Context* pContext);
}