#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class Window;

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
		bool SetScreenMode();

		std::shared_ptr<Window> GetWindow() const { return m_window; }

	private:
		bool createRHI();

	private:
		std::shared_ptr<Window> m_window;

		ID3D11Device* m_device = nullptr;
		ID3D11DeviceContext* m_immediateContext = nullptr;
		IDXGISwapChain* m_swapChain = nullptr;
		ID3D11RenderTargetView* m_renderTargetView = nullptr;
	};
}