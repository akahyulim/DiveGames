#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{
	TypeInfo::TypeInfo(const std::string& typeName, const TypeInfo* baseTypeInfo)
		: m_TypeHash(std::hash<std::string>{}(typeName))
		, m_TypeName(typeName)
		, m_BaseTypeInfo(baseTypeInfo)
	{
	}

	bool TypeInfo::IsTypeOf(size_t typeHash) const
	{
		const TypeInfo* curTypeInfo = this;
		while (curTypeInfo)
		{
			if (curTypeInfo->GetTypeHash() == typeHash)
				return true;

			curTypeInfo = curTypeInfo->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* typeInfo) const
	{
		if (!typeInfo)
			return false;

		const TypeInfo* curTypeInfo = this;
		while (curTypeInfo)
		{
			if (curTypeInfo == typeInfo || curTypeInfo->GetTypeHash() == typeInfo->GetTypeHash())
				return true;

			curTypeInfo = curTypeInfo->GetBaseTypeInfo();
		}

		return false;
	}

	Resource::~Resource()
	{
		DV_LOG(Resource, debug, "Delete Resource - name: {0}, instanceID: {1}", m_Name, m_InstanceID);
	}

	void Resource::SetFilepath(const std::filesystem::path& filepath)
	{
		m_Name = filepath.stem().string();
		// 추후 상대경로 저장 수정 필요
		// 이를 위해서 FileUtils에 관련 함수를 만들어야 하는데
		// 상대경로의 기준으 되는 WorkingDir 설정도 필요하다.
		m_Filepath = filepath;
	}
}
