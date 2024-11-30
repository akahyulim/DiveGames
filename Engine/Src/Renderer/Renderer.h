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
		
		static void Update();
		static void Render();
		static void Present();

		static ID3D11RenderTargetView* GetScreenBufferView() { return s_pScreenRenderTargetView; }
		static ID3D11DepthStencilView* GetScreenDepthStencilView() { return s_pScreenDepthStencilView; }

		static ID3D11Device* GetDevice() { return s_pDevice; }
		static ID3D11DeviceContext* GetDeviceContext() { return s_pDeviceContext; }

		static const DirectX::XMFLOAT2& GetRenderTargetSize() { return s_RenerTargetSize; }
		static void SetRenderTargetSize(uint32_t width, uint32_t height);

		static const DirectX::XMFLOAT2& GetScreenSize() { return s_ScreenSize; }
		static void ChangeScreenSize(uint32_t width, uint32_t height);

		static bool IsVSync() { return s_bVSync; }
		static void SetVSync(bool use) { s_bVSync = use; }

		// 추후 제거 대상
		static void SetClearColor(float r, float g, float b, float a) {
			s_ClearColor[0] = r;	s_ClearColor[1] = g;	s_ClearColor[2] = b;	s_ClearColor[3] = a;
		}

	private:
		static bool createRenderTargets();
		static bool updateScreenViews();

	private:
		static IDXGISwapChain* s_pSwapChain;
		static ID3D11Device* s_pDevice;
		static ID3D11DeviceContext* s_pDeviceContext;
		static ID3D11RenderTargetView* s_pScreenRenderTargetView;
		static ID3D11Texture2D* s_pScreenDepthStencilTexture;
		static ID3D11DepthStencilView* s_pScreenDepthStencilView;

		static DirectX::XMFLOAT2 s_RenerTargetSize;	// 직접 설정?
		static DirectX::XMFLOAT2 s_ScreenSize;
		static bool s_bVSync;

		// 추후 제거 대상
		static float s_ClearColor[4];	// 카메라에서 설정

	};

	// urho3d는 뷰포트와 각종 렌더링 설정(그림자, 테크닉, 필터모드, 그림자 맵 사이즈 등)을 관리하고
	// 이들을 이용해 update와 render를 수행한다.
	// 정확하게는 manages drawing of 3d views라고 설명해 놓았다.
	// 문제는 urho3d처럼 구조를 짤 때 리소스들의 생성을 누가 하느냐이다.
	// => 몇몇 요소는 Graphics에서 동적으로 생성하며 직접 관리하는 듯 하다.
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

		std::vector<ViewScreen*> m_ViewScreens;	// 유니티의 레이어와는 다르다. 착각하지 말자.

		std::array<RenderTexture*, static_cast<uint8_t>(eRenderTargetType::Count)> m_RenderTargets;

		bool m_bInitialized;
	};

	Renderer* GetRenderer();
}