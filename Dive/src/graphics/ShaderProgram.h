#pragma once
#include "shader.h"

namespace Dive
{
	enum class eShaderType
	{
		Vertex,
		Hull,
		Domain,
		Pixel,
		Compute,
		Undefined
	};

	// Shader 묶음이다.
	// ShaderManager에서 다양한 구성으로 만들어지며
	// Material에서 사용한다.
	class ShaderProgram
	{
	public:
		ShaderProgram() = default;
		~ShaderProgram() = default;

		void SetShaderStage(eShaderType type, IShader* shader);
		void SetInputLayout(ID3D11InputLayout* inputLayout) { m_inputLayout = inputLayout; }

		void Bind();

	private:
		std::unordered_map<eShaderType, IShader*> m_shaders;
		ID3D11InputLayout* m_inputLayout = nullptr;
	};
}