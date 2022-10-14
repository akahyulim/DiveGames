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

	// ������ Ŭ���� �̸�.
	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	// ������ ��� ������.
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
		// ������.
		explicit Graphics(Context* pContext);
		// �Ҹ���.
		~Graphics();

		// �ʱ�ȭ �Ϸ� ���� ����.
		bool IsInitialized() const;

		// ������, D3D11 ��ġ �� ���ҽ� ����.
		void Destroy();

		// ������ ����.
		bool WindowCreate(int width, int height, unsigned int flags);
		// ������ �޽��� �ڵ鷯.
		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		// ������ ����.
		void CloseWindow();
		// ������ �޽��� ó�� ����.
		bool RunWindow();
		// ������ �ڵ鷯 ����.
		HWND GetWindowHandle() const { return m_hWnd; }
		// ������ Ÿ��Ʋ ���� ����.
		std::wstring GetWindowTitle() const { return m_WindowTitle; }
		// ������ Ÿ��Ʋ ���� ����.
		void SetWindowTitle(const std::wstring& title);
		// ������ ��ġ ����.
		void GetWindowPosition(int& outX, int& outY) const;
		// ������ ��ġ ����.
		void SetWindowPosition(int x, int y);
		// ������ Ŭ���̾�Ʈ ���� ũ�� ����.
		void GetWindowSize(int& outWidth, int&outHeight) const;
		// ������ Ŭ���̾�Ʈ ���� ũ�� �缳��.
		void ResizeWindow(int width, int height);
		// ������ ���̱�.
		void ShowWindow();
		// ������ ����.
		void HideWindow();
		// ��ü ȭ�� ��ȯ.
		void SetFullScreenWindow(bool bFullScreen);
		// �������� ������� ��ȯ.
		void SetBorderlessWindow(bool bBorderelss);
		// ��ü ȭ�� ���� ����.
		bool IsFullScreen() const;
		// �������� ������ ���� ����.
		bool IsBoderlessWindow() const;
		// ������ ũ�� ���� ���� ���� ����.
		bool IsResizable() const;
		
		// ������, D3D11 ����̽� ��� ���� �� ����.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);

		void Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil);

		bool IsDeviceLost();

		// ������ ������ ����.
		bool BeginFrame();
		// ������ ������ ���� �� ���� ü��.
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

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		// �ӽ�
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }


		// ������ ũ�� ����� ȣ��. �����, ����Ÿ�� ����� �Լ� ȣ��.
		void OnResizeWindow();

	private:
		// D3D11 Device, SwapChain ����.
		bool createDevice(int width, int height);
		// �ĸ� ���� ũ�� ���濡 ���� �⺻ �ؽ��� �����.
		bool updateSwapChain(int width, int height);
		// Draw ���� �غ�.
		void prepareDraw();

	private:
		// �ν��Ͻ�.
		HINSTANCE m_hInstance;
		// ������ �ڵ鷯.
		HWND m_hWnd;
		// ������ Ÿ��Ʋ.
		std::wstring m_WindowTitle;
		// ������ �߽� ��ġ.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// ������ ��Ÿ��.
		DWORD m_WindowStyles;
		unsigned int m_WindowFlags;
		// ȭ�� �ʺ�(==������ Ŭ���̾�Ʈ ���� �ʺ�).
		int m_Width;
		// ȭ�� ����(==������ Ŭ���̾�Ʈ ���� ����).
		int m_Height;


		// ����Ʈ ��� ������, �ʱ�ȭ �� �����ϴ� �κ��� ã�� ���ߴ�.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// Device
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		// �ĸ� ���� ����Ÿ�ٺ�.
		ID3D11RenderTargetView* m_pDefaultRenderTargetView;
		// �⺻ ���� ���ٽ� �ؽ���.
		ID3D11Texture2D* m_pDefaultDepthStencilTexture;
		// �⺻ ���� ���ٽ� ��.
		ID3D11DepthStencilView* m_pDefaultDepthStencilView;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType;

		Texture2D* m_pDepthStencil;

		Texture2D* m_pRenderTargets[MAX_RENDERTARGETS];

		VertexBuffer* m_pVertexBuffers[MAX_VERTEX_STREAMS];
		IndexBuffer* m_pIndexBuffer;

		Shader* m_pVertexShader;
		Shader* m_pPixelShader;

		// dirty check ���� ������.
		bool m_bRenderTargetsDirty;
	};

	void RegisterGraphicsObject(Context* pContext);
}