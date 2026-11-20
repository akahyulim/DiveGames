#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#include <memory>
#include <wrl/client.h>
#include "Graphics/Types.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/TextureManager.h"	// handle 선언 참조용
#include "Graphics/RenderPass.h"		// 현재 Graphics에 Set/Clear RenderTarget이 없다.
#include "Graphics/Geometry.h"			// DebugVertex 선언

namespace Dive
{
	class Graphics;
	class VertexBuffer;

	struct FrameData
	{
		DirectX::XMMATRIX viewProj;
	};

	class Renderer2D
	{
		enum class eDepthStencilState : uint8_t
		{
			Default,
			Count
		};

		enum class eRasterizerState : uint8_t
		{
			FillSolid_CullNone,
			Wireframe_CullNone,
			Scissor_CullNone,
			Count
		};

		enum class eBlendState : uint8_t
		{
			Alpha,
			Additive,
			Count
		};

		enum class eSamplerState : uint8_t
		{
			WrapPoint,
			WrapLinear,
			ClampPoint,
			ClampLinear,
			Count
		};

	public:
		Renderer2D(Graphics* graphics);
		~Renderer2D();

		void ResolveToBackbuffer();

		void Resize(uint32_t width, uint32_t height);
		void OnResize(uint32_t width, uint32_t height);

		RenderTexture* GetScreenRenderTarget() const { return m_screenRenderTarget.get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_screenRenderTarget ? m_screenRenderTarget->GetShaderResourceView() : nullptr; }

		// 카메라가 적당하다.
		void BeginScene(const DirectX::XMMATRIX& viewProj);
		void EndScene();

		void DrawLine(const DirectX::XMFLOAT2& from, const DirectX::XMFLOAT2& to, const Color& color);
		void DrawLine(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& to, const Color& color);
		void DrawTriangle(const DirectX::XMFLOAT2& v0, const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2, const Color& color);
		void DrawTriangle(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const Color& color);
		void DrawRect(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const Color& color);
		void DrawRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const Color& color);
		void DrawRect(const DirectX::XMFLOAT4X4& taransform, const Color& color);

		void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const Color& color);
		void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const Color& color);
		void DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, TextureHandle texture, float tilingFactor = 1.0f, const Color& tintColor = Color::White);
		void DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, TextureHandle texture, float tilingFactor = 1.0f, const Color& tintColor = Color::White);

		// DrawSprite

	private:
		void createDepthStencilStates();
		void createRasterizerStates();
		void createBlendStates();
		void createSamplers();
		void createResolutionDependantResources(uint32_t width, uint32_t height);
		
		void startBatch();
		void flush(); 
		void nextBatch();
		
	private:
		Graphics* m_graphics = nullptr;

		uint32_t m_width;
		uint32_t m_height;

		std::array<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, static_cast<size_t>(eDepthStencilState::Count)> m_depthStencilStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11RasterizerState>, static_cast<size_t>(eRasterizerState::Count)>m_rasterizerStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11BlendState>, static_cast<size_t>(eBlendState::Count)> m_blendStates;
		std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> m_samplerStates;

		std::unique_ptr<RenderTexture> m_screenRenderTarget;

		// lines
		uint32_t m_lineVertexCount = 0;
		std::unique_ptr<DebugVertex[]> m_lineVertices;
		std::unique_ptr<VertexBuffer> m_lineVertexBuffer;

		// quad
		uint32_t m_quadVertexCount = 0;
		std::unique_ptr<DebugVertex[]> m_quadVertices;
		std::unique_ptr<VertexBuffer> m_quadVertexBuffer;

		std::unique_ptr<ConstantBuffer<FrameData>> m_cbFrame;

		RenderPassDesc m_pass;
	};
}