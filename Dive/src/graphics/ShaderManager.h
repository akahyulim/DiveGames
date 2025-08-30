#pragma once
#include "ShaderProgram.h"

namespace Dive
{
	class ShaderManager
	{
	public:
		static bool Initialize();

		static std::shared_ptr<ShaderProgram> GetProgram(const std::string& name);

		static const std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>& GetAllPrograms() { return s_programCache; }
		
		static std::shared_ptr<ShaderProgram> CreateShaderProgram(const std::string& name, eInputLayout type);
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> CreateInputLayout(std::shared_ptr<VertexShader> vs, eInputLayout type);

	private:
		static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> s_programCache;
	};
}
