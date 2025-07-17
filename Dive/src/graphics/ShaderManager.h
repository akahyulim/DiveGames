#pragma once
#include "Shader.h"

namespace Dive
{
	class ShaderManager
	{
	public:
		static void Initialize();

		static bool LoadShader(const std::filesystem::path& filepath, eShaderType type, eInputLayout inputLayout = eInputLayout::None);
		static Shader* GetShader(const std::string& name);

	private:
		static std::unordered_map<std::string, std::unique_ptr<Shader>> s_Shaders;
	};
}
