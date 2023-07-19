#pragma once
#include "GraphicsDefs.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class RenderTexture;
	class InputLayout;
	class ShaderVariation;
	class ConstantBuffer;

	class Graphics
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static bool WindowCreate();

		static bool RunWindow();

		static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static void Close();

		static bool IsInitialized();

		static HINSTANCE GetWindowInstance();
		static HWND GetWindowHandle();
		static std::wstring GetWindowTitle();
		static void SetWindowTitle(const std::wstring& title);

		static void GetWindowSize(int& width, int& height);
		static DirectX::XMINT2 GetWindowSize();
		static int GetWindowWidth();
		static int GetWindowHeight();
		static void ResizeWindow(int width, int height);
		static void ResizeWindow(DirectX::XMINT2 size);
		
		static bool CreateDeviceAndSwapChain();

		static bool UpdateSwapChain(uint32_t width = 0, uint32_t height = 0);

		static bool BeginFrame();
		static void EndFrame();

		static void ClearViews(uint32_t target, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil);
		
		static void Draw(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t vertexCount, uint32_t vertexStart);
		static void DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart = 0);

		static VertexBuffer* GetVertexBuffer();
		static void SetVertexBuffer(VertexBuffer* pVertexBuffer, uint32_t offset = 0);

		static IndexBuffer* GetIndexBuffer();
		static void SetIndexBuffer(IndexBuffer* pIndexBuffer, uint32_t offset = 0);

		static InputLayout* GetInputLayout();
		static void SetShaderVariation(eShaderType type, ShaderVariation* pShaderVariation);

		static void SetConstantBuffer(uint8_t index, eShaderType type, ConstantBuffer* pBuffer);

		static void SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pView);
		static void SetRenderTargetViews(uint32_t start, uint32_t count, ID3D11RenderTargetView** ppViews);
		static void SetDepthStencilView(ID3D11DepthStencilView* pView);

		static void SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, uint32_t stencilRef = 0);

		static Texture* GetTexture(eTextureUnit unit);
		static void SetTexture(eTextureUnit unit, Texture* pTexture);

		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();

		static ID3D11RenderTargetView* GetDefaultRenderTargetView();
		static ID3D11DepthStencilView* GetDefaultDepthStencilView();

	private:
		static void prepareDraw();
	};
}
