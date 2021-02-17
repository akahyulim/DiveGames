#pragma once
#include "DivePch.h"

namespace Dive
{
	class Shader;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class Dive_Texture;
	// sampler�� ������ �𸣰����� Data����.

	// �� �� �̻� ���ȴٸ� �̸� ������ ���°͵� ������ ���� �� ����.
	// �׷��� Object�� ����ϴ� Class�� ����� ���� hash�� ������ �Ͽ� ���� State���� �񱳸� ���� Command�� �ߺ� ����� ȸ���� �� �ִ�.
	// ��� ��ü���� ��� ���θ� Ȯ���ϴ� invalid()�� �ʿ��� �� �ִ�. �̶� ���� class�� ��︰��.
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
		Dive_Texture* depthStencilTexture = nullptr;			// ���� �����ߴ�.
		std::vector<Dive_Texture*> renderTargets;				// �ִ� 8���̴�.

	};
}