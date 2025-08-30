#pragma once
#include "shader.h"

namespace Dive
{
	class ShaderProgram
	{
	public:
		ShaderProgram(std::shared_ptr<VertexShader> vs, std::shared_ptr<PixelShader> ps, Microsoft::WRL::ComPtr<ID3D11InputLayout> il,
			const std::string& name);
		~ShaderProgram() = default;

		void Bind();

		std::string GetName() const { return m_name; }

	private:
		std::shared_ptr<VertexShader> m_vs;
		std::shared_ptr<PixelShader> m_ps;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_il;

		std::string m_name;
	};
}