#pragma once
#include "Base/Base.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"
#include "Events/EventSystem.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	struct WindowData;

	class GameObject;
	class Transform;
	class SpriteRenderer;

	enum class eRasterizerStateType : size_t
	{
		CullBackSolid = 0,
		CullBackWire,
		Count
	};

	enum class eShaderType : size_t
	{
		Sprite = 0,
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

		static void DrawSprite(Transform* pTransform, SpriteRenderer* pRenderer);
		static void DrawSprite(DirectX::XMMATRIX matView, DirectX::XMMATRIX matProj, GameObject* pObj);

		//static void SetViewport(float width, float height);

		static void SetResolution(unsigned int width, unsigned int height);
		// �̸��� �ٲٰ� �ʹ�. ������ createTextures�� �ϱ� �����̴�.
		static void SetTextures(unsigned int width, unsigned int height);

		static void OnWindowData(const Event& e);

		static GraphicsDevice& GetGraphicsDevice() { return m_GraphicsDevice; }

		static unsigned int GetTextureWidth() { return m_TextureWidth; }
		static unsigned int GetTextureHeight() { return m_TextureHeight; }

		static ID3D11RasterizerState* GetRasterizerState(eRasterizerStateType type);
		static Shader* GetShader(eShaderType type);

		// test ============================================================
		static Texture2D* GetSampleTexture() { return m_pSampleTex; }
		static Texture2D* GetDepthStencilTexture() { return m_pDepthStencilTex; }

	private:
		static void createRenderTargets();
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
		static ID3D11SamplerState* m_pLinearSampler;

		// depth stencil states

		// rasterizer states
		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerStateType::Count)> m_RasterizerStates;

		// shaders
		static std::array<Shader, static_cast<size_t>(eShaderType::Count)> m_Shaders;

		// constant buffer
		static ID3D11Buffer* m_pMatrixBuffer;

		struct MatrixBufferType
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
		};

		// test resources =================
		static Texture2D* m_pSampleTex;
		static Texture2D* m_pDepthStencilTex;
		static ID3D11DepthStencilState* m_pDepthStencilState;
	};
}