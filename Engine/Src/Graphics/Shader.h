#pragma once
#include "GraphicsDefs.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Context;
	class ShaderVariation;
	class FileStream;

	class Shader : public Resource
	{
		DIVE_OBJECT(Shader, Resource)

	public:
		explicit Shader(Context* pContext);
		~Shader() override;

		ShaderVariation* GetVariation(eShaderType type, const std::string& defines);

		const std::string& GetSourceCode(eShaderType type) const { return type == eShaderType::Vertex ? m_VSSourceCode : m_VSSourceCode; }

		std::string GetFilePath() const { return m_FilePath; }

		uint32_t GetVariationCount() const { return m_VariationCount; }

		// override
		bool Load(FileStream* pDeserializer) override;
		
		// static
		static void RegisterObject(Context* pContext);

	private:
		std::string m_FilePath;

		std::string m_VSSourceCode;
		std::string m_PSSourceCode;

		std::unordered_map<std::string, ShaderVariation*> m_VSVariations;
		std::unordered_map<std::string, ShaderVariation*> m_PSVariations;

		uint32_t m_VariationCount;
	};
}