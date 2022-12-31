#pragma once
#include "Core/Object.h"
#include "GraphicsDefs.h"
#include "ShaderVariation.h"
#include "ShaderProgram.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class InputLayout;
	class Texture;
	class Texture2D;
	class ConstantBuffer;

	const LPCWSTR WND_CLASS_NAME = L"AppWnd";

	enum eWindowFlags : uint32_t
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
		//int32_t multiSample
		//int32_t refreshRate;
	};

	struct WindowModeParams
	{
		int32_t width = 0;
		int32_t height = 0;
		ScreenModeParams screenModeParams;
	};

	class Graphics : public Object
	{ 
		DIVE_OBJECT(Graphics, Object)

	public:
		explicit Graphics(Context* pContext);
		~Graphics() override;

		bool SetMode(int32_t width, int32_t height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int32_t multiSample, int32_t refreshRate);
		void Destroy();

		bool IsInitialized() const;

		// Window
		bool WindowCreate(int32_t width, int32_t height, uint32_t flags);
		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void CloseWindow();
		
		bool RunWindow();
		
		HWND GetWindowHandle() const { return m_WindowHandle; }
		std::wstring GetWindowTitle() const { return m_WindowTitle; }
		
		void SetWindowTitle(const std::wstring& title);
		
		void GetWindowPosition(int32_t& outX, int32_t& outY) const;
		void SetWindowPosition(int32_t x, int32_t y);
		
		int32_t GetWidth() const { return m_Width; }
		int32_t GetHeight() const { return m_Height; }
		DirectX::XMINT2 GetSize() const { return DirectX::XMINT2(m_Width, m_Height); }
		
		void ResizeWindow(int32_t width, int32_t height);
		
		void ShowWindow();
		void HideWindow();
		
		void SetFullScreenWindow(bool bFullScreen);
		void SetBorderlessWindow(bool bBorderelss);
		
		bool IsFullScreen() const;
		bool IsBoderlessWindow() const;
		bool IsResizable() const;
		
		// DX11 Graphics & Resources
		bool CreateDeviceAndSwapChain(uint32_t width = 0, uint32_t height = 0);

		bool UpdateSwapChain(uint32_t width = 0, uint32_t height = 0);

		bool IsDeviceLost();

		bool BeginFrame();
		void EndFrame();

		void Clear(int32_t flags, const DirectX::XMFLOAT4& color, float depth, int32_t stencil);

		// Draw 함수들: 총 5개
		void Draw(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t vertexCount, uint32_t vertexStart);
		void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart);
		void DrawIndexedInstanced(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t instanceCount, uint32_t indexStart);

		void SetDepthStencil(Texture2D* pTexture);

		ID3D11RenderTargetView* GetRenderTarget(uint32_t index) const;
		void SetRenderTarget(uint32_t index, Texture2D* pTexture);
		void ResetRenderTargets();

		D3D11_VIEWPORT GetViewport() const { return m_Viewport; }
		RECT GetViewportRect() const;
		void SetViewport(const D3D11_VIEWPORT& viewport);
		void SetViewportRect(const RECT& rect);

		VertexBuffer* GetVertexBuffer(uint32_t index) const;
		void SetVertexBuffer(VertexBuffer* pBuffer);

		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }
		void SetIndexBuffer(IndexBuffer* pBuffer);

		ShaderVariation* GetShader(eShaderType type, const std::string& name, const std::string& defines = std::string());
		void SetShaders(ShaderVariation* pVertexShader, ShaderVariation* pPixelShader);

		void SetShaderParameter(const std::string& param, bool value);
		void SetShaderParameter(const std::string& param, float value);
		void SetShaderParameter(const std::string& param, int32_t value);
		void SetShaderParameter(const std::string& param, const DirectX::XMFLOAT2& vector);
		void SetShaderParameter(const std::string& param, const DirectX::XMFLOAT3& vector);
		void SetShaderParameter(const std::string& param, const DirectX::XMFLOAT4& vector);
		void SetShaderParameter(const std::string& param, const DirectX::XMFLOAT4X4& matrix);
		void SetShaderParameter(const std::string& param, const DirectX::XMMATRIX& matrix);
		//void SetShaderParameter(const std::string& param, const float* pData, uint32_t count);	// 이건 아직 뭔지 감도 안잡힌다.

		ConstantBuffer* GetOrCreateConstantBuffer(eShaderType type, uint32_t index, uint32_t size);

		Texture* GetTexture(size_t index);
		void SetTexture(size_t index, Texture* pTexture);

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

		bool LoadShaders();

		// 윈도우 크기 변경시 호출. 백버퍼, 렌더타겟 재생성 함수 호출.
		void OnResizeWindow();

	private:
		void prepareDraw();

	private:
		HINSTANCE m_hInstance;
		HWND m_WindowHandle;
		std::wstring m_WindowTitle;
		DirectX::XMINT2 m_WindowPosition;
		DWORD m_WindowStyles;
		uint32_t m_WindowFlags;
		int32_t m_Width;
		int32_t m_Height;

		// 디폴트 모드 같은데, 초기화 및 설정하는 부분을 찾지 못했다.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// DX11 Device & Default Resources
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;

		ID3D11RenderTargetView* m_pDefaultRenderTargetView;
		ID3D11Texture2D* m_pDefaultDepthStencilTexture;
		ID3D11DepthStencilView* m_pDefaultDepthStencilView;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		D3D11_VIEWPORT m_Viewport;

		Texture2D* m_pRenderTargets[MAX_RENDERTARGETS];
		Texture2D* m_pDepthStencil;

		ID3D11RenderTargetView* m_pCurRenderTargetViews[MAX_RENDERTARGETS];
		ID3D11DepthStencilView* m_pCurDepthStencilView;

		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;

		ShaderVariation* m_pVertexShaderVariation;
		ShaderVariation* m_pPiexlShaderVariation;

		std::string m_LastShaderName;
		Shader* m_pLastShader;

		// InputLayout
		uint64_t m_InputLayoutHash = 0;
		std::unordered_map<uint64_t, InputLayout*> m_InputLayouts;

		Texture* m_pTextures[16];
		// 얘네는 원래 impl에서 관리하던 거다.
		ID3D11ShaderResourceView* m_pShaderResourceViews[16];
		ID3D11SamplerState* m_pSamplers[16];
		bool m_bTextureDirty;	// 이것두 impl에서 관리했다.

		// dirty check 전부 모으기.
		bool m_bVertexTypeDirty;
		bool m_bRenderTargetsDirty;

		// ConstantBuffers
		std::map<std::pair<ShaderVariation*, ShaderVariation*>, ShaderProgram*> m_ShaderPrograms;
		ShaderProgram* m_pCurShaderProgram ;
		std::unordered_map<uint32_t, ConstantBuffer*> m_AllConstantBuffers;
		std::vector<ConstantBuffer*> m_DirtyConstantBuffers;
		ID3D11Buffer* m_pCurVertexShaderConstantBuffers[7];
		ID3D11Buffer* m_pCurPixelShaderConstantBuffers[7];
	};

	void RegisterGraphicsObject(Context* pContext);
}