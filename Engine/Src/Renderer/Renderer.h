#pragma once
#include "Graphics/Graphics.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eRenderPathType : uint8_t
	{
		Forward = 0,
		Deferred
	};

	enum class eRenderTargetType : uint8_t
	{
		GBuffer_DepthStencil = 0,
		GBuffer_Diffuse,
		GBuffer_Normal,
		GBuffer_Specular,
		FrameRender,
		FrameOutput,
		Count
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class ViewScreen;
	class RenderTexture;
	class ConstantBuffer;
	class Shader;

	class DvRenderer
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static ID3D11RenderTargetView* GetScreenBufferView() { return s_pScreenRenderTargetView; }
		static ID3D11DepthStencilView* GetScreenDepthStencilView() { return s_pScreenDepthStencilView; }

		static ID3D11Device* GetDevice() { return s_pDevice; }
		static ID3D11DeviceContext* GetDeviceContext() { return s_pDeviceContext; }

		static const DirectX::XMFLOAT2& GetRenderTargetSize() { return s_RenerTargetSize; }
		static void SetRenderTargetSize(uint32_t width, uint32_t height);

		static const DirectX::XMFLOAT2& GetScreenSize() { return s_ScreenSize; }
		static void ChangeScreenSize(uint32_t width, uint32_t height);

	private:
		static bool createRenderTargets();
		static bool updateScreenViews();

	private:
		static IDXGISwapChain* s_pSwapChain;
		static ID3D11Device* s_pDevice;
		static ID3D11DeviceContext* s_pDeviceContext;
		static ID3D11RenderTargetView* s_pScreenRenderTargetView;			// ���۶�� �̸��� ������ ���� �ʴ´�.
		static ID3D11Texture2D* s_pScreenDepthStencilTexture;
		static ID3D11DepthStencilView* s_pScreenDepthStencilView;

		static DirectX::XMFLOAT2 s_RenerTargetSize;	// ���� ����?
		static DirectX::XMFLOAT2 s_ScreenSize;
		static BOOL s_bWindowed;

	};

	// urho3d�� ����Ʈ�� ���� ������ ����(�׸���, ��ũ��, ���͸��, �׸��� �� ������ ��)�� �����ϰ�
	// �̵��� �̿��� update�� render�� �����Ѵ�.
	// ��Ȯ�ϰԴ� manages drawing of 3d views��� ������ ���Ҵ�.
	// ������ urho3dó�� ������ © �� ���ҽ����� ������ ���� �ϴ����̴�.
	// => ��� ��Ҵ� Graphics���� �������� �����ϸ� ���� �����ϴ� �� �ϴ�.
	class Renderer
	{
	public:
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		static Renderer* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Renderer;

			return s_pInstance;
		}

		bool Initialize();
		void Shutdown();

		void Update();
		void Render();

		uint32_t GetNumViewScreens() { return static_cast<uint32_t>(m_ViewScreens.size()); }
		void SetNumViewScreens(uint32_t count);

		ViewScreen* GetViewScreen(uint32_t index);
		void SetViewScreen(uint32_t index, ViewScreen* pViewScreen);

		bool IsInitialized() const { return m_bInitialized; }

	private:
		Renderer();
		~Renderer();

		bool createRenderTextures();

	private:
		static Renderer* s_pInstance;

		ID3D11Device* m_pDevice;

		std::vector<ViewScreen*> m_ViewScreens;	// ����Ƽ�� ���̾�ʹ� �ٸ���. �������� ����.

		std::array<RenderTexture*, static_cast<uint8_t>(eRenderTargetType::Count)> m_RenderTargets;

		bool m_bInitialized;
	};

	Renderer* GetRenderer();
}