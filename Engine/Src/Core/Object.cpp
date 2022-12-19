#include "divepch.h"
#include "Object.h"
#include "Context.h"
#include "CoreDefs.h"

namespace Dive
{
	TypeInfo::TypeInfo(const char* pTypeName, const TypeInfo* pBaseTypeInfo)
		: m_Hash(pTypeName),
		m_Name(pTypeName),
		m_BaseTypeInfo(pBaseTypeInfo)
	{
		DV_ASSERT(pTypeName);
	}

	bool TypeInfo::IsTypeOf(StringHash typeHash) const
	{
		const TypeInfo* pCurTypeInfo = this;

		while (pCurTypeInfo)
		{
			if (pCurTypeInfo->GetTypeHash() == typeHash)
				return true;

			pCurTypeInfo = pCurTypeInfo->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* pTypeInfo) const
	{
		if (!pTypeInfo)
			return false;

		const TypeInfo* pCurTypeInfo = this;

		while (pCurTypeInfo)
		{
			if (pCurTypeInfo == pTypeInfo || pCurTypeInfo->GetTypeHash() == pTypeInfo->GetTypeHash())
				return true;

			pCurTypeInfo = pCurTypeInfo->GetBaseTypeInfo();
		}

		return false;
	}

	Object::Object(Context* pContext)
		: m_pContext(pContext)
	{
		DV_ASSERT(m_pContext);
	}

	bool Object::IsInstanceOf(StringHash typeHash) const
	{
		return GetTypeInfo()->IsTypeOf(typeHash);
	}

	bool Object::IsInstanceOf(const TypeInfo* pTypeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(pTypeInfo);
	}

	Object* Object::GetSubsystem(StringHash typeHash) const
	{
		return m_pContext->GetSubsystem(typeHash);
	}

	ObjectFactory::ObjectFactory(Context* pContext)
		: m_pContext(pContext),
		m_pTypeInfo(nullptr)
	{
		DV_ASSERT(m_pContext);
	}
}