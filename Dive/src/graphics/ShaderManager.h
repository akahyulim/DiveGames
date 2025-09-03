#pragma once
#include "ShaderProgram.h"

namespace Dive
{
	class ShaderManager
	{
	public:
		static bool Initialize();

		static std::shared_ptr<ShaderProgram> GetProgram(const std::string& name);

		static const std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>& GetAllPrograms() { return s_spCache; }

		static bool CreateVertexShaderAndInputLayout(const std::filesystem::path& filepath, eInputLayout type);
		static bool CreatePixelShader(const std::filesystem::path& filepath);
		static bool CreateShaderProgram(const std::string& vs, const std::string& ps, const std::string& name);

	private:
		static std::unordered_map<std::string, std::pair<std::shared_ptr<VertexShader>, Microsoft::WRL::ComPtr<ID3D11InputLayout>>> s_vss;
		static std::unordered_map<std::string, std::shared_ptr<PixelShader>> s_pss;
		static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> s_spCache;
	};
}
