#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;
	class DvWindow;

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

	class DvGraphics : public DvObject
	{ 
		DIVE_OBJECT(DvGraphics, DvObject)

	public:
		// ������.
		explicit DvGraphics(DvContext* pContext);
		// �Ҹ���.
		~DvGraphics();

		// �ʱ�ȭ �Ϸ� ���� ����.
		bool IsInitialized() const;

		// ������, D3D11 ��ġ �� ���ҽ� ����.
		void Destroy();

		// ������ ������ ����.
		bool BeginFrame();
		// ������ ������ ���� �� ���� ü��.
		void EndFrame();

		// Draw �Լ���: �� 5��

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		// ������ Ÿ��Ʋ ����.
		void SetTitle(const std::wstring& title);
		// ������ ��ġ ����.
		void SetPosition(int x, int y);
		
		
		// ������, D3D11 ����̽� ��� ���� �� ����.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
			bool tripleBuffer, int multiSample, int refreshRate);

		// �ӽ�: ������ �޽��� ��ŷ �Լ� ȣ��.
		bool RunWindow();

		// �ӽ�
		HWND GetWindowHandle();

		// �ӽ�
		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }

	private:
		// D3D11 Device, SwapChain ����.
		bool createDevice(int width, int height);
		// �ĸ� ���� ũ�� ���濡 ���� �⺻ �ؽ��� �����.
		bool updateSwapChain(int width, int height);

	private:
		// ������ ��ü ������.
		std::unique_ptr<DvWindow> m_pWindow;
		// ������ Ÿ��Ʋ. => ���� ���.
		std::wstring m_WindowTitle;
		// ������ �߽� ��ġ. => ���� ���.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// ����ü�� ������ ����ȴ�.
		int m_Width;
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
		ID3D11RenderTargetView* m_pDefaultRenderTargetView = nullptr;
		// �⺻ ���� ���ٽ� �ؽ���.
		ID3D11Texture2D* m_pDefaultDepthStencilTexture = nullptr;
		// �⺻ ���� ���ٽ� ��.
		ID3D11DepthStencilView* m_pDefaultDepthStencilView = nullptr;
	};
}