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
		// �Ҹ���
		~DvGraphics();

		// �ʱ�ȭ ���� ����.
		bool IsInitialized() const;

		// ������ ������ ����.
		bool BeginFrame();
		// ������ ������ ���� �� ���� ü��.
		void EndFrame();

		// ������ Ÿ��Ʋ ����.
		void SetWindowTitle(const std::wstring& title);
		// ������ �߽� ��ġ ����.
		void SetWindowPosition(int x, int y);
		
		
		// ������, D3D11 ����̽� ��� ���� �� ����.
		// bool triple buffer, int multi sample, int refresh rate
		bool SetMode(int width, int height, bool bFullScreen, bool bBorderless, bool bResizable, bool bVSync);

		// �ӽ�: ������ �޽��� ��ŷ �Լ� ȣ��.
		bool RunWindow();

	private:
	private:
		// ������ ��ü ������.
		std::unique_ptr<DvWindow> m_pWindow;
		// ������ Ÿ��Ʋ.
		std::wstring m_WindowTitle;
		// ������ �߽� ��ġ.
		DirectX::XMINT2 m_WindowPosition{-1, -1};
		// ����ü�� ������ ����ȴ�.
		int m_Width;
		int m_Height;
		// ����Ʈ ��� ������, �ʱ�ȭ �� �����ϴ� �κ��� ã�� ���ߴ�.
		WindowModeParams m_WindowMode;
		ScreenModeParams m_ScreenMode;
		// icon image

		// Device
	};
}