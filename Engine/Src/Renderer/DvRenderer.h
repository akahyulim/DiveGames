#pragma once
#include "RendererDefs.h"		// 추후 이 곳으로 흡수시키기

namespace Dive
{
	class ViewScreen;
	class RenderTexture;
	class DvConstantBuffer;

	enum class eConstantBuffer
	{
		Frame,
		Material,
		Camera,
		Light,
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

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};

	struct CameraBuffer
	{
		DirectX::XMFLOAT3 position;
		float p0;
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT3 color;
		float spotAngle;

		DirectX::XMFLOAT3 position;
		float range;

		DirectX::XMFLOAT3 direction;
		uint32_t options;
	};

	class DvRenderer
	{
	public:
		static bool Initialize();

		static void Update();
		static void Render();

		static uint32_t GetNumViewScreens() { return (uint32_t)m_ViewScreens.size(); }
		static void SetNumViewScreens(uint32_t count);

		static ViewScreen* GetViewScreen(uint32_t index);
		static void SetViewScreen(uint32_t index, ViewScreen* pViewScreen);

		static ID3D11RasterizerState* GetRasterizerState(eRasterizerState rs) { return m_RasterizerStates[(size_t)rs]; }
		static ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState ds) { return m_DepthStencilStates[(size_t)ds]; }
		static ID3D11BlendState* GetBlendState(eBlendState bs) { return m_BlendStates[(size_t)bs]; }
		static DvConstantBuffer* GetConstantBuffer(eConstantBuffer cb) { return m_ConstantBuffers[(size_t)cb]; }

	private:
		static void createRasterizerStates();
		static void createDepthStencilStates();
		static void createBlendStates();
		static void createRenderTextures();
		static void createConstantBuffers();


	private:
		static std::vector<ViewScreen*> m_ViewScreens;	// 유니티의 레이어와는 다르다. 착각하지 말자.

		static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> m_RasterizerStates;
		static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> m_DepthStencilStates;
		static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> m_BlendStates;
		static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> m_RenderTargets;
		static std::array<DvConstantBuffer*, static_cast<size_t>(eConstantBuffer::Count)> m_ConstantBuffers;
	};
}