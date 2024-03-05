#include "DivePch.h"
#include "Object.h"

namespace Dive
{
	Object::Object()
		: m_ID(0)
	{}

	Object::Object(const std::string& name)
		: m_ID(0)
		, m_Name(name)
	{}
}

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
}