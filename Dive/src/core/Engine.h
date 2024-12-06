#pragma once

namespace Dive
{
	extern class Engine* GEngine;

	class Window;
	class Graphics;
	class Renderer;
	class Input;

	class Engine
	{
	public:
		~Engine() = default;

		static Engine* GetInstance()
		{
			std::call_once(s_OnceFlag, []()
			{
					s_pInstance = new Engine;
			});
			return s_pInstance;
		}

		bool Initialize(HINSTANCE hInstance, uint32_t width, uint32_t height, LPCWSTR pTitle);
		void Shutdown(); 

		void Tick();

		void OnExit();

		// window
		HWND GetWindowHandle();

		// graphics
		IDXGISwapChain* GetSwapChain();
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		// systems
		Window& GetWindow() const { return *m_pWindow.get(); }
		Graphics& GetGraphics() const { return *m_pGraphics.get(); }
		Renderer& GetRenderer() const { return *m_pRenderer.get(); }
		Input& GetInput() const { return *m_pInput.get(); }

		// time & fps
		double GetElapsedTimeMS() const;
		double GetElapsedTimeSec() const;
		float GetDeltaTimeMS() const;
		float GetDeltaTimeSec() const;
		uint16_t GetFps() const;

	private:
		Engine();

	private:
		static Engine* s_pInstance;
		static std::once_flag s_OnceFlag;

		std::unique_ptr<Window> m_pWindow;
		std::shared_ptr<Graphics> m_pGraphics;
		std::unique_ptr<Renderer> m_pRenderer;
		std::unique_ptr<Input> m_pInput;

		double m_ElapsedTimeMS;
		float m_DeltaTimeMS;
		std::chrono::steady_clock::time_point m_LastTickTime;
		uint16_t m_Fps;
	};
}