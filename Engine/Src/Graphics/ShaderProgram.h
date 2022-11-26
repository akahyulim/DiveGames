#pragma once

namespace Dive
{
	class Graphics;
	class ShaderVariation;
	struct ShaderParameter;

	// 셰이더의 상수 버퍼와 개별 파라미터를 관리하는 구조체
	struct ShaderProgram
	{
		ShaderProgram(Graphics* pGraphics, ShaderVariation* pVS, ShaderVariation* pPS);
		~ShaderProgram();

		std::unordered_map<std::string, ShaderParameter> m_Parameters;
		ConstantBuffer* m_pVSConstantBuffers[7] = { nullptr, };
		ConstantBuffer* m_pPSConstantBuffers[7] = { nullptr, };
	};
}