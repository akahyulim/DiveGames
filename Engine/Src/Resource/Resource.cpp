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
		DV_LOG_ENGINE_ERROR("{:s}(��)�� �ҷ����⸦ �������� �ʽ��ϴ�.", GetTypeName());
		return false;
	}

	bool Resource::Save(FileStream* pSerializer)
	{
		DV_LOG_ENGINE_ERROR("{:s}(��)�� ������ �������� �ʽ��ϴ�.", GetTypeName());
		return false;
	}

	void Resource::SetName(const std::string& name)
	{
		m_Name = name;
		m_NameHash = name;
	}
}