#pragma once
#include "Graphics.h"	// ���� cpp�� �ű��

namespace Dive
{
//	class Graphics;

	class Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		bool Initialize(uint32_t width, uint32_t height, HWND hWnd);
		void Shutdown();

		void Update();
		void Render();

		void Present();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

	private:
	private:
		std::unique_ptr<Graphics> m_pGraphics;
	};
}