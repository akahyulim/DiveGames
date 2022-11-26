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

	// 이름을 바꾸자.
	// ShaderVaritaion을 직접 지우진 않는다.
	void Pass::releaseShaders()
	{
		m_VertexShaders.clear();
		m_PixelShaders.clear();
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
			m_Passes[it->second] = nullptr;
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

	unsigned int Technique::GetPassIndex(const std::string& name)
	{
		// 일단 앞 8개의 index는 비워둔다.

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
}