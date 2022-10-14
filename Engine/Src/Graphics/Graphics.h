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

	// 윈도우 클래스 이름.
	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	// 윈도우 모드 열거자.
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
		// 생성자.
		explicit Graphics(Context* pContext);
		// 소멸자.
		~Graphics();

		// 초기화 완료 여부 리턴.
		bool IsInitialized() const;

		// 윈도우, D3D11 장치 및 리소스 제거.
		void Destroy();

		// 윈도우 생성.
		bool WindowCreate(int width, int height, unsigned int flags);
		// 윈도우 메시지 핸들러.
		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		// 윈도우 종료.
		void CloseWindow();
		// 윈도우 메시지 처리 루프.
		bool RunWindow();
		// 윈도우 핸들러 리턴.
		HWND GetWindowHandle() const { return m_hWnd; }
		// 윈도우 타이틀 문구 리턴.
		std::wstring GetWindowTitle() const { return m_WindowTitle; }
		// 윈도우 타이틀 문구 설정.
		void SetWindowTitle(const std::wstring& title);
		// 윈도우 위치 리턴.
		void GetWindowPosition(int& outX, int& outY) const;
		// 윈도우 위치 설정.
		void SetWindowPosition(int x, int y);
		// 윈도우 클라이언트 영역 크기 리턴.
		void GetWindowSize(int& outWidth, int&outHeight) const;
		// 윈도우 클라이언트 영역 크기 재설정.
		void ResizeWindow(int width, int height);
		// 윈도우 보이기.
		void ShowWindow();
		// 윈도우 숨김.
		void HideWindow();
		// 전체 화면 전환.
		void SetFullScreenWindow(bool bFullScreen);
		// 보더리스 윈도우로 번환.
		void SetBorderlessWindow(bool bBorderelss);
		// 전체 화면 여부 리턴.
		bool IsFullScreen() const;
		// 보더리스 윈도우 여부 리턴.
		bool IsBoderlessWindow() const;
		// 윈도우 크기 변경 가능 여부 리턴.
		bool IsResizable() const;
		
		// 윈도우, D3D11 디바이스 모드 설정 후 생성.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);

		void Clear(int flags, const DirectX::XMFLOAT4& color, float depth, float stencil);

		bool IsDeviceLost();

		// 렌더링 프레임 시작.
		bool BeginFrame();
		// 렌더링 프레임 종료 후 스왑 체인.
		void EndFrame();

		// Draw 함수들: 총 5개
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

		// 기타: 얘네들을 이용해 States를 실시간으로 생성하는 듯 하다.
		// SetFillMode		fill_mode
		// SetLineAntiAlias	bool
		// SetClipPlane		bool
		// SetColorWhite?	bool
		// SetDepthBias		float, float
		// SetScissorTest	bool
		// SetStencilTest	bool

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		// 임시
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }


		// 윈도우 크기 변경시 호출. 백버퍼, 렌더타겟 재생성 함수 호출.
		void OnResizeWindow();

	private:
		// D3D11 Device, SwapChain 생성.
		bool createDevice(int width, int height);
		// 후면 버퍼 크기 변경에 맞춰 기본 텍스쳐 재생성.
		bool updateSwapChain(int width, int height);
		// Draw 사전 준비.
		void prepareDraw();

	private:
		// 인스턴스.
		HINSTANCE m_hInstance;
		// 윈도우 핸들러.
		HWND m_hWnd;
		// 윈도우 타이틀.
		std::wstring m_WindowTitle;
		// 윈도우 중심 위치.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// 윈도우 스타일.
		DWORD m_WindowStyles;
		unsigned int m_WindowFlags;
		// 화면 너비(==윈도우 클라이언트 영역 너비).
		int m_Width;
		// 화면 높이(==윈도우 클라이언트 영역 높이).
		int m_Height;


		// 디폴트 모드 같은데, 초기화 및 설정하는 부분을 찾지 못했다.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// Device
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		// 후면 버퍼 렌더타겟뷰.
		ID3D11RenderTargetView* m_pDefaultRenderTargetView;
		// 기본 깊이 스텐실 텍스쳐.
		ID3D11Texture2D* m_pDefaultDepthStencilTexture;
		// 기본 깊이 스텐실 뷰.
		ID3D11DepthStencilView* m_pDefaultDepthStencilView;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType;

		Texture2D* m_pDepthStencil;

		Texture2D* m_pRenderTargets[MAX_RENDERTARGETS];

		VertexBuffer* m_pVertexBuffers[MAX_VERTEX_STREAMS];
		IndexBuffer* m_pIndexBuffer;

		Shader* m_pVertexShader;
		Shader* m_pPixelShader;

		// dirty check 전부 모으기.
		bool m_bRenderTargetsDirty;
	};

	void RegisterGraphicsObject(Context* pContext);
}