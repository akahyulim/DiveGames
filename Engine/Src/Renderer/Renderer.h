#pragma once
#include "RendererDefs.h"		// ���� �� ������ �����Ű��

namespace Dive
{
	class ViewScreen;
	class RenderTexture;
	class ConstantBuffer;
	class Shader;

	enum class eConstantBuffer
	{
		Frame,
		Material,
		Camera,
		Light,
		LightVS,
		Count
	};

	struct FrameBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct MaterialBuffer
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 normal;	// xmfloat3�̾�� �ϳ�...

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};

	struct CameraBuffer
	{
		DirectX::XMFLOAT3 position;
		float padding;

		DirectX::XMFLOAT4 perspectiveValue;

		DirectX::XMMATRIX viewInverse;
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT3 color;
		float outerConeAngle;

		DirectX::XMFLOAT3 position;
		float rangeRcp;

		DirectX::XMFLOAT3 direction;
		float innerConeAngle;
		
		uint32_t options;
		DirectX::XMFLOAT3 padding;

		DirectX::XMMATRIX shadow;
	};

	struct LightVSBuffer
	{
		DirectX::XMMATRIX shadow;
	};

	enum class eDepthStencilState
	{
		DepthReadWrite,
		DepthReadWrite_StencilReadWrite,
		GBuffer,
		DepthDisabled,   // skydome���� off������...
		ForwardLight,
		Count
	};

	enum class eGBuffer
	{
		DepthStencil,
		ColorSpecIntensity,
		Normal,
		SpecPower,
		Total
	};

	enum class eShader
	{
		ForwardLight,
		Deferred,
		DeferredLights,
		ShadowGen,
		ForwardLightShadow,
		Total
	};

	class Renderer
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Update();
		static void Render();

		static uint32_t GetNumViewScreens() { return (uint32_t)m_ViewScreens.size(); }
		static void SetNumViewScreens(uint32_t count);

		static ViewScreen* GetViewScreen(uint32_t index);
		static void SetViewScreen(uint32_t index, ViewScreen* pViewScreen);

		static ID3D11RasterizerState* GetRasterizerState(eRasterizerState rs) { return m_RasterizerStates[(size_t)rs]; }
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState ds) { return m_DepthStencilStates[(size_t)ds]; }
		static ID3D11BlendState* GetBlendState(eBlendState bs) { return m_BlendStates[(size_t)bs]; }
		static ConstantBuffer* GetConstantBuffer(eConstantBuffer cb) { return m_ConstantBuffers[(size_t)cb]; }

		static Shader* GetShader(eShader type) { return m_Shaders[(size_t)type]; }

		static RenderTexture* GetGBufferTexture(eGBuffer type) { return m_GBuffer[(size_t)type]; }

	private:
		static void createRasterizerStates();
		static void createDepthStencilStates();
		static void createBlendStates();
		static void createRenderTextures();
		static void createConstantBuffers();
		static void createShaders();
		static void createGBuffer();


	private:
		static std::vector<ViewScreen*> m_ViewScreens;	// ����Ƽ�� ���̾�ʹ� �ٸ���. �������� ����.

		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Total)> m_RasterizerStates;
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> m_DepthStencilStates;
		static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Total)> m_BlendStates;
		static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Total)> m_RenderTargets;
		static std::array<ConstantBuffer*, static_cast<size_t>(eConstantBuffer::Count)> m_ConstantBuffers;

		static std::array<Shader*, static_cast<size_t>(eShader::Total)> m_Shaders;

		static std::array<RenderTexture*, static_cast<size_t>(eGBuffer::Total)> m_GBuffer;
	};
}