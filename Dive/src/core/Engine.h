#pragma once

namespace Dive
{
	class Window;
	class Renderer;

	class Engine
	{
	public:
		Engine();
		~Engine() = default;

		bool Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle);
		void Run();
		void Shutdown();

		void OnExit();

		HWND GetWindowHandle();
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

	private:
	private:
		std::unique_ptr<Window> m_pWindow;
		std::unique_ptr<Renderer> m_pRenderer;
	};
}