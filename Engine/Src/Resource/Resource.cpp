#include "divepch.h"
#include "Resource.h"
#include "Core/Context.h"
#include "IO/FileStream.h"
#include "IO/Log.h"

namespace Dive
{
	Resource::Resource(Context* pContext)
		: Object(pContext)
	{
	}

	Resource::~Resource()
	{
	}

	bool Resource::LoadFromFile(const std::string& fileName)
	{
		FileStream deserializer(fileName, eFileStreamMode::Read);
		return Load(&deserializer);
	}

	bool Resource::SaveToFile(const std::string& fileName)
	{
		FileStream serializer(fileName, eFileStreamMode::Write);
		return Save(&serializer);
	}

	bool Resource::Load(FileStream* pDeserializer)
	{
		DV_LOG_ENGINE_ERROR("{:s}(은)는 불러오기를 지원하지 않습니다.", GetTypeName());
		return false;
	}

	bool Resource::Save(FileStream* pSerializer)
	{
		DV_LOG_ENGINE_ERROR("{:s}(은)는 저장을 지원하지 않습니다.", GetTypeName());
		return false;
	}

	void Resource::SetName(const std::string& name)
	{
		m_Name = name;
		m_NameHash = name;
	}
}