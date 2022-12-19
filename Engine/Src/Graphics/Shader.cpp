#include "divepch.h"
#include "Shader.h"
#include "ShaderVariation.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceCache.h"
#include "IO/FileSystem.h"
#include "IO/FileStream.h"
#include "IO/Log.h"

namespace Dive
{
	Shader::Shader(Context* pContext)
		: Resource(pContext),
		m_VariationCount(0)
	{}

	Shader::~Shader()
	{
		m_VSSourceCode.clear();
		m_PSSourceCode.clear();

		for (auto variation : m_VSVariations)
			DV_DELETE(variation.second);
		m_VSVariations.clear();

		for (auto variation : m_PSVariations)
			DV_DELETE(variation.second);
		m_PSVariations.clear();

		DV_LOG_ENGINE_TRACE("Shader 소멸 완료({:s})", GetName());
	}

	ShaderVariation* Shader::GetVariation(eShaderType type, const std::string& defines)
	{
		std::unordered_map<std::string, ShaderVariation*>& variations = type == eShaderType::Vertex ? m_VSVariations : m_PSVariations;
		auto it = variations.find(defines);
		if (it == variations.end())
		{
			auto newVariation = new ShaderVariation(this, type);
			newVariation->SetName(GetName());
			//newVariation->SetDefines(defines);		// 결국엔 이걸로 가야할 듯 한데...
			newVariation->AddDefine(defines);

			variations.emplace(defines, newVariation);

			++m_VariationCount;

			return newVariation;
		}

		return it->second;
	}

	bool Shader::Load(FileStream* pDeserializer)
	{
		if(!pDeserializer)
			return false;

		// 결국 파일의 모든 내용을 string에 넣는다.
		std::string sourceCode;
		while (!pDeserializer->IsEof())
		{
			std:: string lineStr = pDeserializer->ReadLine();

			sourceCode += lineStr;
			sourceCode += "\n";
		}

		m_VSSourceCode = sourceCode;
		m_PSSourceCode = sourceCode;

		SetName(pDeserializer->GetFileName());
		m_FilePath = pDeserializer->GetFilePath();

		return true;
	}
	
	void Shader::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Shader>();
	}
}