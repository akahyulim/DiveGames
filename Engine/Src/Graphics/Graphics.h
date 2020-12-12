#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class Window;
	class DepthStencilState;
	class VertexBuffer;
	class IndexBuffer;

	// 전체 창 화면 같은건 해상도와 이 모드의 조합으로 만든다.
	enum class eScreenMode
	{
		FullScreen,
		Borderless,
		Windowed,
	};

	struct DisplayMode
	{
		eScreenMode screenMode;
		bool fullScreen;
		bool borderless;
		bool vSync;
		// refresh rate
	};

	struct VgaInfo
	{
		std::wstring name;
		int videoMemory;
	};

	struct AdapterInfo
	{
		AdapterInfo(unsigned int width, unsigned int height, unsigned int numerator, unsigned int denominator)
		{
			this->width = width;
			this->hegiht = height;
			this->numerator = numerator;
			this->denominator = denominator;
		}

		unsigned int width;
		unsigned int hegiht;
		unsigned int numerator;
		unsigned int denominator;
	};

	// Window, D3D11 RHI 그리고 GPU Resource를 관리합니다.
	class Graphics : public Object
	{
		DIVE_OBJECT(Graphics, Object);

	public:
		Graphics(Context* context);
		~Graphics();

		bool BeginFrame();
		void EndFrame();

		bool IsInitialized();

		// settings를 얻어와야 한다.
		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync);

		std::shared_ptr<Window> GetWindow() const { return m_window; }

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		ID3D11Device* GetRHIDevice() const { return m_device; }
		ID3D11DeviceContext* GetRHIContext() const { return m_immediateContext; }

		//= d3d11 pipeline?
		bool SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer);
		bool SetIndexBuffer();

	private:
		bool createDisplay(int width, int height, DisplayMode displayMode);
		void adjustDisplayMode(int& width, int& height, DisplayMode& displayMode);
		bool createRHI();
		bool depthStencilStates();

	private:
		std::shared_ptr<Window> m_window;

		int m_width;
		int m_height;
		DisplayMode m_displayMode;
		VgaInfo m_vgaInfo;
		std::vector<AdapterInfo> m_adapterInfos;

		ID3D11Device* m_device = nullptr;
		ID3D11DeviceContext* m_immediateContext = nullptr;
		IDXGISwapChain* m_swapChain = nullptr;
		ID3D11RenderTargetView* m_renderTargetView = nullptr;

		//= GPU Resources
		// 다른 곳에 전달하지 않는다면 unique_ptr로 변경
		std::shared_ptr<DepthStencilState> m_depthStencilStateEnabled;
		std::shared_ptr<DepthStencilState> m_depthStencilStateDisabled;

		std::weak_ptr<VertexBuffer> m_vertexBuffer;
	};
}