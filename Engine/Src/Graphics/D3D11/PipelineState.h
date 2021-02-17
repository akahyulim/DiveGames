#pragma once
#include "DivePch.h"

namespace Dive
{
	class Shader;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class Dive_Texture;
	// sampler는 왜인지 모르겠지만 Data같다.

	// 두 번 이상 사용된다면 미리 생성해 놓는것도 나쁘지 않을 것 같다.
	// 그럴땐 Object를 상속하는 Class로 만들어 고유 hash를 가지게 하여 이전 State와의 비교를 통해 Command에 중복 등록을 회피할 수 있다.
	// 멤버 객체들의 사용 여부를 확인하는 invalid()가 필요할 수 있다. 이때 역시 class가 어울린다.
	struct PipelineState
	{
		PipelineState()		= default;
		~PipelineState()	= default;

		Shader* vertexShader						= nullptr;
		Shader* pixelShader							= nullptr;
		RasterizerState* rasterizerState			= nullptr;
		BlendState* blendState						= nullptr;
		DepthStencilState* depthStencilState		= nullptr;

		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology	= D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		D3D11_VIEWPORT viewport;
		
		ID3D11RenderTargetView* swapChainRenderTargetView = nullptr;
		Dive_Texture* depthStencilTexture = nullptr;			// 따로 구분했다.
		std::vector<Dive_Texture*> renderTargets;				// 최대 8장이다.

	};
}