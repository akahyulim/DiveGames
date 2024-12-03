#include "stdafx.h"
#include "Object.h"

namespace Dive
{
	TypeInfo::TypeInfo(const std::string& typeName, const TypeInfo* pBaseTypeInfo)
		: m_TypeHash(std::hash<std::string>{}(typeName))
		, m_TypeName(typeName)
		, m_pBaseTypeInfo(pBaseTypeInfo)
	{
	}

	bool TypeInfo::IsTypeOf(size_t typeHash) const
	{
		const TypeInfo* pCurrent = this;
		while (pCurrent)
		{
			if (pCurrent->GetTypeHash() == typeHash)
				return true;

			pCurrent = pCurrent->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* pTypeInfo) const
	{
		if (!pTypeInfo)
			return false;

		const TypeInfo* pCurrent = this;
		while (pCurrent)
		{
			if (pCurrent == pTypeInfo || pCurrent->GetTypeHash() == pTypeInfo->GetTypeHash())
				return true;

			pCurrent = pCurrent->GetBaseTypeInfo();
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