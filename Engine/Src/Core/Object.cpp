#include "divepch.h"
#include "Object.h"
#include "Context.h"
#include "CoreDefs.h"

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

	Object::Object(Context* pContext)
		: m_pContext(pContext)
	{
		DV_ASSERT(m_pContext);
	}

	bool Object::IsInstanceOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	bool Object::IsInstanceOf(const TypeInfo* typeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(typeInfo);
	}

	Object* Object::GetSubsystem(StringHash type) const
	{
		return m_pContext->GetSubsystem(type);
	}
}