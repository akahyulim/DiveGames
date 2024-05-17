#pragma once
#include "RendererDefs.h"		// 추후 이 곳으로 흡수시키기

namespace Dive
{
	class ViewScreen;
	class RenderTexture;
	class ConstantBuffer;
	class Shader;

	enum class eVSConstBuf
	{
		Model,
		Camera,
		Light,
		Count
	};

	enum class ePSConstBuf
	{
		Model,
		Camera,
		Light,
		Count
	};

	struct VSConstBuf_Model
	{
		DirectX::XMMATRIX world;
	};

	struct VSConstBuf_Camera
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct VSConstBuf_Light
	{
		DirectX::XMMATRIX shadow;
	};

	struct PSConstBuf_Model
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 normal;	// xmfloat3이어야 하나...

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};

	struct PSConstBuf_Camera
	{
		DirectX::XMFLOAT3 position;
		float padding;

		DirectX::XMFLOAT4 perspectiveValue;

		DirectX::XMMATRIX viewInverse;
	};

	struct PSConstBuf_Light
	{
		DirectX::XMFLOAT3 color;
		float outerConeAngle;

		DirectX::XMFLOAT3 position;
		float rangeRcp;

		DirectX::XMFLOAT3 direction;
		float innerConeAngle;
		
		uint32_t options;
		int shadowEnabled;
		DirectX::XMFLOAT2 padding;

		DirectX::XMMATRIX shadow;
	};

	enum class eDepthStencilState : uint8_t
	{
		DepthReadWrite,
		DepthReadWrite_StencilReadWrite,
		GBuffer,
		DepthDisabled,   // skydome에서 off용으로...
		ForwardLight,
		Count
	};

	enum class eGBuffer : uint8_t
	{
		DepthStencil,
		ColorSpecIntensity,
		Normal,
		SpecPower,
		Count
	};

	enum class eShader : uint8_t
	{
		LightDepth,
		ForwardLightShadow,
		Deferred,
		DeferredLights,
		Count
	};

	enum class eRendererShader : uint8_t
	{
		Depth_VS,
		ForwardLight_VS,
		ForwardLight_PS,
		GBuffer_VS,
		GBuffer_PS,
		DeferredLight_VS,
		DeferredLight_PS,
		Count
	};

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

		uint32_t GetNumViewScreens() { return (uint32_t)m_ViewScreens.size(); }
		void SetNumViewScreens(uint32_t count);

		ViewScreen* GetViewScreen(uint32_t index);
		void SetViewScreen(uint32_t index, ViewScreen* pViewScreen);

		ID3D11RasterizerState* GetRasterizerState(eRasterizerState rs) { return m_RasterizerStates[(size_t)rs]; }
		ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState ds) { return m_DepthStencilStates[(size_t)ds]; }
		ID3D11BlendState* GetBlendState(eBlendState bs) { return m_BlendStates[(size_t)bs]; }

		ConstantBuffer* GetVSConstantBuffer(eVSConstBuf type) { return m_VSConstantBuffers[(uint32_t)type]; }
		ConstantBuffer* GetPSConstantBuffer(ePSConstBuf type) { return m_PSConstantBuffers[(uint32_t)type]; }

		Shader* GetShader(eRendererShader type) { return m_Shaders[static_cast<uint32_t>(type)]; }

		RenderTexture* GetGBufferTexture(eGBuffer type) { return m_GBuffer[(size_t)type]; }

	private:
		Renderer();
		~Renderer();

		void createRasterizerStates();
		void createDepthStencilStates();
		void createBlendStates();
		void createRenderTextures();
		void createConstantBuffers();
		void createShaders();
		void createGBuffer();

	private:
		static Renderer* s_pInstance;

		std::vector<ViewScreen*> m_ViewScreens;	// 유니티의 레이어와는 다르다. 착각하지 말자.

		std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> m_RasterizerStates;
		std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> m_DepthStencilStates;
		std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> m_BlendStates;
		std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> m_RenderTargets;

		std::array<ConstantBuffer*, static_cast<size_t>(eVSConstBuf::Count)> m_VSConstantBuffers;
		std::array<ConstantBuffer*, static_cast<size_t>(ePSConstBuf::Count)> m_PSConstantBuffers;

		std::array<Shader*, static_cast<uint32_t>(eRendererShader::Count)> m_Shaders;

		std::array<RenderTexture*, static_cast<size_t>(eGBuffer::Count)> m_GBuffer;
	};

	Renderer* GetRenderer();
}