#pragma once
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <filesystem>

#include "Types.h"
#include "Shader.h"
#include "InputLayout.h"
#include "ShaderProgram.h"

namespace Dive
{
	enum class eShaderPrograms : uint8_t
	{
		GBuffer,
		GBufferTessellation,
		DeferredLighting,
		Forward,
		ForwardTessellation,
		Skybox,
		SkySphere,
		UniformSky,
		Resolve,
		Picking,
		Count
	};

	class Graphics;

	struct ShaderProgramDesc
	{
		std::string name = "";
		std::filesystem::path vsPath = "";
		std::filesystem::path psPath = "";
		std::filesystem::path dsPath = "";
		std::filesystem::path hsPath = "";
		std::filesystem::path gsPath = "";
		eInputLayout ilType = eInputLayout::Lit;
	};

	class ShaderManager
	{
	public:
		static ShaderManager& Get()
		{
			static ShaderManager instance;
			return instance;
		}

		ShaderManager(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = default;

		ShaderManager& operator=(const ShaderManager&) = delete;
		ShaderManager& operator=(ShaderManager&&) = default;

		bool Initialize(Graphics* graphics);
		void Shutdown();

		ShaderProgram* CreateShaderProgram(Graphics* graphics, ShaderProgramDesc& desc);
		ShaderProgram* GetShaderProgram(const std::string name);

		ComputeShader* CreateComputeShader(Graphics* graphics, const std::filesystem::path& path);
		ComputeShader* GetComputeShader(const std::filesystem::path& path);

	private:
		ShaderManager() = default;
		~ShaderManager();

		//bool createShaderProgram(const std::string& vsName, const std::string& psName, eShaderPrograms sp);

		Microsoft::WRL::ComPtr<ID3DBlob> compileShader(const std::filesystem::path& filepath, eShaderStage stage);

		VertexShader* createVertexShaderAndInputLayout(Graphics* graphics, const std::filesystem::path& path, eInputLayout type);

		template<typename T>
		T* createShaderInternal(Graphics* graphics, const std::filesystem::path& path, eShaderStage stage,
			std::unordered_map<std::filesystem::path, std::unique_ptr<T>>& shaderMap);

	private:
		std::unordered_map<std::filesystem::path, std::unique_ptr<VertexShader>> m_vsMap;
		std::unordered_map<std::filesystem::path, std::unique_ptr<HullShader>> m_hsMap;
		std::unordered_map<std::filesystem::path, std::unique_ptr<DomainShader>> m_dsMap;
		std::unordered_map<std::filesystem::path, std::unique_ptr<PixelShader>> m_psMap;
		std::unordered_map<std::filesystem::path, std::unique_ptr<ComputeShader>> m_csMap;

		using InputLayoutKey = std::pair<std::filesystem::path, eInputLayout>;
		std::map<InputLayoutKey, std::unique_ptr<InputLayout>> m_ilMap;

		std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_shaderProgramMap;
	};

	template<typename T>
	inline T* ShaderManager::createShaderInternal(
		Graphics* graphics, 
		const std::filesystem::path& path, 
		eShaderStage stage, 
		std::unordered_map<std::filesystem::path, std::unique_ptr<T>>& shaderMap)
	{
		if (path.empty())
			return nullptr;

		auto it = shaderMap.find(path);
		if (it != shaderMap.end())
			return it->second.get();

		auto blob = compileShader(path, stage);
		if (blob == nullptr)
			return nullptr;

		shaderMap[path] = std::make_unique<T>(graphics, blob.Get());
		return shaderMap[path].get();
	}
}
