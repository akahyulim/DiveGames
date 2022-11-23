#pragma once

namespace Dive
{
	class Graphics;
	class Shader;
	struct ShaderParameter;

	// ���̴��� ��� ���ۿ� ���� �Ķ���͸� �����ϴ� ����ü
	struct ShaderProgram
	{
		ShaderProgram(Graphics* pGraphics, Shader* pVS, Shader* pPS);
		~ShaderProgram();

		std::unordered_map<std::string, ShaderParameter> m_Parameters;
		ConstantBuffer* m_pVSConstantBuffers[7] = { nullptr, };
		ConstantBuffer* m_pPSConstantBuffers[7] = { nullptr, };
	};
}