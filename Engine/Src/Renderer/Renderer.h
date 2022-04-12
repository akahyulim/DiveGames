#pragma once
#include "Base/Base.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"
#include "Events/EventSystem.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	struct WindowData;

	class Scene;
	class GameObject;
	class Transform;
	class SpriteRenderable;

	struct FrameBuffer;
	struct UberBuffer;

	struct Visibility
	{
		void Clear()
		{
			visibleSpriteRenderables.clear();
			visibleMeshRenderables.clear();
			visibleSkinnedMeshRenderables.clear();
			visibleLights.clear();
			visibleTerrains.clear();
			visibleParticles.clear();
		}

		Scene* pScene = nullptr;

		// Renderable�� ��������, ������ �������� �ؾ��Ѵ�.
		// �ϴ��� �����ٰ� ��������.
		// �ٵ� �̰� �ʿ��Ѱ�...?
		enum eFlags
		{
			Emtpy = 0,
			SpriteRenderables = 1 << 0,
			MeshRenderables = 1 << 1,
			SkinnedMeshRenderables = 1 << 2,
			Lights = 1 << 3,
			Terrains = 1 << 4,
			Particles = 1 << 5,

			Everything = ~0u
		};

		eFlags flags;

		std::vector<GameObject*> visibleSpriteRenderables;
		std::vector<GameObject*> visibleMeshRenderables;
		std::vector<GameObject*> visibleSkinnedMeshRenderables;
		std::vector<GameObject*> visibleLights;
		std::vector<GameObject*> visibleTerrains;
		std::vector<GameObject*> visibleParticles;
	};

	enum class eRasterizerStateType : size_t
	{
		CullBackSolid = 0,
		CullBackWire,
		Count
	};

	enum class eDepthStencilStateType : size_t
	{
		//DepthOffStencilOff = 0,
		DepthOnStencilOn = 0,
		Count
	};

	enum class eSamplerStateType : size_t
	{
		Linear = 0,
		Count
	};


	enum class eShaderType : size_t
	{
		Sprite = 0,
		Mesh,
		Count
	};

	struct Shader
	{
		~Shader()
		{
			DV_RELEASE(pVertexShader);
			DV_RELEASE(pInputLayout);
			DV_RELEASE(pPixelShader);
		}

		ID3D11VertexShader* pVertexShader	= nullptr;
		ID3D11InputLayout* pInputLayout		= nullptr; 
		ID3D11PixelShader* pPixelShader		= nullptr;
	};

	// ���� GraphicsDeivce�� �ʱ�ȭ �� ������ �ϰ�
	// �������� Base Resource ���� �� ������ �����ؾ� �� �� ����.
	class Renderer
	{
	public:
		static void Initialize(const WindowData* pData);
		static void Shutdown();

		// �ᱹ �����Լ���. Graphics���� ó���ϴ� ���� �´� ��...
		static void BeginScene();
		static void EndScene();

		static void SetResolution(unsigned int width, unsigned int height);
		// �̸��� �ٲٰ� �ʹ�. ������ createTextures�� �ϱ� �����̴�.
		static void SetTextures(unsigned int width, unsigned int height);

		static void OnWindowData(const Event& e);

		static GraphicsDevice& GetGraphicsDevice() { return m_GraphicsDevice; }

		static unsigned int GetTextureWidth() { return m_TextureWidth; }
		static unsigned int GetTextureHeight() { return m_TextureHeight; }

		static ID3D11SamplerState* GetSamplerState(eSamplerStateType type);
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilStateType type);
		static ID3D11RasterizerState* GetRasterizerState(eRasterizerStateType type);
		static Shader* GetShader(eShaderType type);

		static D3D11_VIEWPORT* GetBackbufferViewport() { return &m_Viewport; }

		static ID3D11Buffer* GetMatrixBuffer() { return m_pFrameBuffer; }
		static ID3D11Buffer* GetUberBuffer() { return m_pUberBuffer; }

		static void UpdateVisibility(Visibility& vis);

		// test ============================================================
		static Texture2D* GetSampleTexture() { return m_pSampleTex; }
		static Texture2D* GetDepthStencilTexture() { return m_pDepthStencilTex; }


	private:
		static void createRenderTargets();
		static void removeRenderTargets();
		static void createSamplers();
		static void createDepthStencilStates();
		static void createRasterizerStates();
		static void createShaders();
		static void createMatrixBuffer();

	private:
		static GraphicsDevice m_GraphicsDevice;

		static unsigned int m_TextureWidth;
		static unsigned int m_TextureHeight;

		// render targets

		// samplers
		static std::array<ID3D11SamplerState*, static_cast<size_t>(eSamplerStateType::Count)> m_SamplerStates;

		// depth stencil states
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilStateType::Count)> m_DepthStencilStates;

		// rasterizer states
		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerStateType::Count)> m_RasterizerStates;

		// shaders
		static std::array<Shader, static_cast<size_t>(eShaderType::Count)> m_Shaders;

		// constant buffer
		static ID3D11Buffer* m_pFrameBuffer;
		static ID3D11Buffer* m_pUberBuffer;

		static D3D11_VIEWPORT m_Viewport;

		// test resources =================
		static Texture2D* m_pSampleTex;
		static Texture2D* m_pDepthStencilTex;
	};
}