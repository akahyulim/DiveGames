#include "DivePch.h"
#include "Animation.h"
#include "IO/Log.h"

namespace Dive
{
	Animation::Animation()
		: Resource(eResourceType::Animation)
	{
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
			});

		return it == m_Bones.end() ? nullptr : &(*it);
	}
}