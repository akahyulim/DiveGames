#include "stdafx.h"
#include "ShaderProgram.h"
#include "Graphics.h"

namespace Dive
{
	void ShaderProgram::SetShaderStage(eShaderType type, IShader* shader)
	{
		assert(type != eShaderType::Undefined);
		assert(shader);

		if (m_shaders[type] != nullptr)
			DV_LOG(ShaderProgram, warn, "[::SetShaderStage] ShaderStage 덮어 쓰기 발생");

		m_shaders[type] = shader;
	}

	void ShaderProgram::Bind()
	{
		Graphics::GetDeviceContext()->IASetInputLayout(m_inputLayout);

		for (const auto& [stage, shader] : m_shaders)
		{
			if (shader)	shader->Bind();
		}
	}
}