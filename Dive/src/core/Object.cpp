#include "stdafx.h"
#include "Object.h"

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

	Object::Object()
		: m_Id(GenerateId())
		, m_Name("dive_object")
	{
	}

	uint64_t Object::GenerateId()
	{
		// 랜덤 엔진 및 분포 설정 
		std::random_device rd; std::mt19937_64 eng(rd()); 
		std::uniform_int_distribution<uint64_t> distr; 
		
		// 고유한 숫자 생성 
		return distr(eng);
	}
}