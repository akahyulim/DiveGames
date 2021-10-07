#pragma once
#include "Graphics/GraphicsDevice.h"
#include "../GraphicsEnums.h"
#include <memory>

/*
*	1. 싱글톤???
*	2. 몇 가지 UpdateXXX()가 존재한다.
*	3. Sampler는 Mapping이라는 걸 사용한다.
*/
namespace dive
{
	class dvRenderer
	{
	public:
		bool Initialize(HWND hWnd, bool fullScreen);

		// Passes
		void DrawScene();
		void DrawLights();

		// Get & Set
		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice.get(); }

		ID3D11DepthStencilState* GetDepthStencilState(DEPTH_STENCIL_STATE_TYPES type) { return m_pDepthStencilStates[type].Get(); }
		ID3D11RasterizerState* GetRasterizerState(RASTERIZER_STATE_TYPES type) { return m_pRasterizerStates[type].Get(); }

	private:
		bool createSamplerStates();
		bool createDepthStencilStates();
		bool createRasterizerStates();
		bool createBlendStates();
		bool createShaders();

	private:
		std::shared_ptr<GraphicsDevice> m_pGraphicsDevice;

		// SamplerStates : 이건 ENUM이 아니라 mapping이라는 걸로 구분한다.
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStates[DEPTH_STENCIL_STATE_TYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerStates[RASTERIZER_STATE_TYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendStates[BLEND_STATE_TYPE_COUNT];

		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShaders[SHADER_TYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayouts[INPUT_LAYOUT_TYPE_COUNT];
	};
}