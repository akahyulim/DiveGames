#pragma once
#include "RendererDefs.h"		// 추후 이 곳으로 흡수시키기

namespace Dive
{
	class ViewScreen;
	class RenderTexture;
	class ConstantBuffer;
	class Shader;

	enum class eVSConstantBuffers
	{
		Model,
		Camera,
		Light,
		Count
	};

	enum class ePSConstantBuffers
	{
		Material,
		Camera,
		Light,
		Count
	};

	struct ModelConstantBufferVS
	{
		DirectX::XMMATRIX world;
	};

	struct CameraConstantBufferVS
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightConstantBufferVS
	{
		DirectX::XMMATRIX shadow;
	};

	struct MaterialConstantBufferPS
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 normal;	// xmfloat3이어야 하나...

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};

	struct CameraConstantBufferPS
	{
		DirectX::XMFLOAT3 position;
		float padding;

		DirectX::XMFLOAT4 perspectiveValue;

		DirectX::XMMATRIX viewInverse;
	};

	struct LightConstantBufferPS
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

	enum class eDepthStencilState
	{
		DepthReadWrite,
		DepthReadWrite_StencilReadWrite,
		GBuffer,
		DepthDisabled,   // skydome에서 off용으로...
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
		
		static ConstantBuffer* GetVSConstantBuffer(eVSConstantBuffers type) { return m_VSConstantBuffers[(uint32_t)type]; }
		static ConstantBuffer* GetPSConstantBuffer(ePSConstantBuffers type) { return m_PSConstantBuffers[(uint32_t)type]; }

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
		static std::vector<ViewScreen*> m_ViewScreens;	// 유니티의 레이어와는 다르다. 착각하지 말자.

		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Total)> m_RasterizerStates;
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> m_DepthStencilStates;
		static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Total)> m_BlendStates;
		static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Total)> m_RenderTargets;
		
		static std::array<ConstantBuffer*, static_cast<size_t>(eVSConstantBuffers::Count)> m_VSConstantBuffers;
		static std::array<ConstantBuffer*, static_cast<size_t>(ePSConstantBuffers::Count)> m_PSConstantBuffers;


		static std::array<Shader*, static_cast<size_t>(eShader::Total)> m_Shaders;

		static std::array<RenderTexture*, static_cast<size_t>(eGBuffer::Total)> m_GBuffer;
	};
}