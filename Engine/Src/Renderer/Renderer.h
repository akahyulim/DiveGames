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