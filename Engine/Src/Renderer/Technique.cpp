#include "divepch.h"
#include "Technique.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Graphics/ShaderVariation.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	Pass::Pass(const std::string& name)
	{
		m_Name = FileSystem::ToLowerCase(name);
		m_Index = Technique::GetPassIndex(name);
	}

	void Pass::SetVertexShader(const std::string& name)
	{
		m_VertexShaderName = name;
		releaseShaders();
	}

	void Pass::SetVertexShaderDefines(const std::string& defines)
	{
		m_VertexShaderDefines = defines;
		releaseShaders();
	}

	void Pass::SetPixelShader(const std::string& name)
	{
		m_PixelShaderName = name;
		releaseShaders();
	}

	void Pass::SetPixelShaderDefines(const std::string& defines)
	{
		m_PixelShaderDefines = defines;
		releaseShaders();
	}

	void Pass::releaseShaders()
	{
		m_VertexShaderVariations.clear();
		m_PixelShaderVariations.clear();
	}

	std::unordered_map<std::string, unsigned int> Technique::m_PassIndices;

	Technique::Technique(Context* pContext)
		: Resource(pContext)
	{
	}

	Pass* Technique::CreatePass(const std::string& name)
	{
		auto pOldPass = GetPass(name);
		if (pOldPass)
			return pOldPass;

		auto pNewPass = new Pass(name);
		auto passIndex = pNewPass->GetIndex();
		if (passIndex >= static_cast<unsigned int>(m_Passes.size()))
			m_Passes.resize(passIndex + 1);
		m_Passes[passIndex] = pNewPass;

		return pNewPass;
	}
	
	void Technique::RemovePass(const std::string& name)
	{
		auto it = m_PassIndices.find(FileSystem::ToLowerCase(name));
		if (it == m_PassIndices.end())
			return;
		else if (it->second < m_Passes.size() && m_Passes[it->second])
		{
			DV_DELETE(m_Passes[it->second]);
			m_Passes.erase(m_Passes.begin() + it->second);
			m_PassIndices.erase(it);
		}
	}

	Pass* Technique::GetPass(const std::string& name)
	{
		auto it = m_PassIndices.find(FileSystem::ToLowerCase(name));
		return it != m_PassIndices.end() ? GetPass(it->second) : nullptr;
	}

	bool Technique::HasPass(const std::string& name)
	{
		auto it = m_PassIndices.find(FileSystem::ToLowerCase(name));
		return it != m_PassIndices.end() && GetPass(it->second) != nullptr;
	}

	// 아직 분석이 덜 되었지만 RenderPathPass와 관련이 있다.
	// pass name이 동일한 형태이다.
	unsigned int Technique::GetPassIndex(const std::string& name)
	{
		// 일단 앞 8개의 index는 비워둔다.
		{

		}

		auto lowerName = FileSystem::ToLowerCase(name);
		auto it = m_PassIndices.find(lowerName);
		if (it != m_PassIndices.end())
			return it->second;
		else
		{
			unsigned int newPassIndex = static_cast<unsigned int>(m_PassIndices.size());
			m_PassIndices[lowerName] = newPassIndex;
			
			return newPassIndex;
		}
	}

	bool Technique::Load(FileStream* pDeserializer)
	{
		// 파일을 분석하여 멤버 변수를 채운다.

		return true;
	}

	void Technique::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Technique>();
	}
}