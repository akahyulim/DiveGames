#pragma once
#include "GraphicsInclude.h"
#include "Object.h"
#include <assert.h>

namespace dive
{
	// ���߿� �ٸ� ���� ��� ����.
	// �׷��� ���� id�� �ʿ��Ѱ� �ʹ�.
	struct PipelineState : public Object
	{
		PipelineState() : Object(0) {}

		ID3D11VertexShader* pVS = nullptr;
		ID3D11PixelShader* pPS = nullptr;
		ID3D11InputLayout* pIL = nullptr;
		ID3D11DepthStencilState* pDSS = nullptr;
		ID3D11RasterizerState* pRSS = nullptr;
		ID3D11SamplerState* pSS = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	};

}