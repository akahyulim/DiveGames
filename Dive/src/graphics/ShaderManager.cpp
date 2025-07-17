#include "stdafx.h"
#include "ShaderManager.h"

namespace Dive
{
	std::unordered_map<std::string, std::unique_ptr<Shader>> ShaderManager::s_Shaders;

	void ShaderManager::Initialize()
	{
		LoadShader("Assets/Shaders/Default.hlsl", eShaderType::Vertex, eInputLayout::Static_Mesh);
		LoadShader("Assets/Shaders/Default.hlsl", eShaderType::Pixel);

		//LoadShader("Assets/Shaders/ForwardLight.hlsl", eShaderType::Vertex, eInputLayout::Static_Mesh);
		//LoadShader("Assets/Shaders/ForwardLight.hlsl", eShaderType::Pixel);
	}

	bool ShaderManager::LoadShader(const std::filesystem::path& filepath, eShaderType type, eInputLayout inputLayout)
	{
		auto shader = std::make_unique<Shader>();
		if (!shader->Create(filepath, type, inputLayout))
			return false;

		if(s_Shaders.find(shader->GetName())!=s_Shaders.end())
		{
			DV_LOG(ShaderManager, err,  "Already exists shader: {}", shader->GetName());
			shader.reset();
			return false;
		}

		s_Shaders[shader->GetName()] = std::move(shader);

		return true;
	}

	Shader* ShaderManager::GetShader(const std::string& name)
	{
		auto it = s_Shaders.find(name);
		if (it == s_Shaders.end())
		{
			DV_LOG(ShaderManager, err, "Shader not found: {}", name);
			return nullptr;
		}

		return it->second.get();
	}
}
