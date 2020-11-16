#pragma once
#include "Core/ISystem.h"

namespace Dive
{
	class SystemManager;
	class WindowResizeEvent;

	// 굳이 객체들을 구분해 놓을 필요가 있나하는 생각이 든다.
	class Renderer : public ISystem
	{
	public:
		Renderer(SystemManager* manager);
		~Renderer();

		bool Initialize() override;
		void Update() override;

		void Present();

		void OnResize(const WindowResizeEvent* evnt);

	private:
	private:
		ID3D11Device* m_D3d11Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
		IDXGISwapChain* m_SwapChain = nullptr;

		// window data
		HWND m_hWnd;
		int m_Width;
		int m_Height;

		// swap chain data
	};
}

