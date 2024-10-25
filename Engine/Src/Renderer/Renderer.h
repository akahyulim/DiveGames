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