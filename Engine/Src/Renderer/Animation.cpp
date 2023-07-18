#include "DivePch.h"
#include "Animation.h"
#include "Model.h"
#include "IO/Log.h"

namespace Dive
{
	// node를 계층구조로 관리해야만 한다.
	Animation::Animation()
		: Resource(eResourceType::Animation),
		m_Duration(0),
		m_TickPerSecond(0),
		m_NumChannel(0)
	{
	}

	Animation::Animation(const std::string& name, float duration, float tickPerSecond, uint32_t numChannels)
		: Resource(eResourceType::Animation),
		m_Duration(duration),
		m_TickPerSecond(tickPerSecond),
		m_NumChannel(numChannels)
	{
		SetName(name);
	}

	Animation::~Animation()
	{
	}
	
	bool Animation::LoadFromFile(const std::string& filePath)
	{
		return false;
	}
	
	bool Animation::SaveToFile(const std::string& filePath)
	{
		return false;
	}
	
	Bone* Animation::FindBone(const std::string& name)
	{
		auto it = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& bone)
			{
				return bone.GetBoneName() == name;
			}
		);

		if (it == m_Bones.end())
			return nullptr;
		else
			return &(*it);
	}
}