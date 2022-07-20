#include "divepch.h"
#include "DvObject.h"
#include "DvContext.h"

namespace Dive
{
	TypeInfo::TypeInfo(const char* pTypeName, const TypeInfo* pBaseTypeInfo)
		: m_Type(pTypeName),
		m_TypeName(pTypeName),
		m_pBaseTypeInfo(pBaseTypeInfo)
	{
	}

	bool TypeInfo::IsTypeOf(StringHash type) const
	{
		const TypeInfo* pCurrent = this;

		while (pCurrent)
		{
			if (pCurrent->GetType() == type)
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
			if (pCurrent == pTypeInfo || pCurrent->GetType() == pTypeInfo->GetType())
				return true;

			pCurrent = pCurrent->GetBaseTypeInfo();
		}

		return false;
	}

	DvObject::DvObject(DvContext* pContext)
		: m_pContext(pContext)
	{
		// dv assert로 바꾸기?
		assert(m_pContext);
	}

	bool DvObject::IsInstanceOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	bool DvObject::IsInstanceOf(const TypeInfo* typeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(typeInfo);
	}

	DvObject* DvObject::GetSubsystem(StringHash type) const
	{
		return m_pContext->GetSubsystem(type);
	}
}