#pragma once
#include "StringHash.h"
#include "Variant.h"

namespace Dive
{
	class DvContext;

	// 타입의 확인 및 비교에 사용된다.
	class TypeInfo
	{
	public:
		TypeInfo(const char* pTypeName, const TypeInfo* pBaseTypeInfo);
		~TypeInfo() = default;

		bool IsTypeOf(StringHash type) const;
		bool IsTypeOf(const TypeInfo* pTypeInfo) const;

		StringHash GetType() const { return m_Type; }
		const std::string& GetTypeName() const { return m_TypeName; }
		const TypeInfo* GetBaseTypeInfo() const { return m_pBaseTypeInfo; }

		template<typename T>
		bool IsTypeOf() const
		{
			return IsTypeOf(T::GetTypeInfoStatic());
		}

	private:
		StringHash m_Type;
		std::string m_TypeName;
		const TypeInfo* m_pBaseTypeInfo;
	};

#define DIVE_OBJECT(typeName, baseTypeName)	\
public:\
	using className = typeName;	\
	using BaseClassName = baseTypeName;	\
	virtual Dive::StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); }	\
	virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); }	\
	virtual const Dive::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); }	\
	static Dive::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); }	\
	static const std::string GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }	\
	static const Dive::TypeInfo* GetTypeInfoStatic() \
	{	\
		static const Dive::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic());	\
		return &typeInfoStatic;	\
	}

	
	class DvObject
	{
	public:
		explicit DvObject(DvContext* pContext);
		virtual ~DvObject() = default;

		virtual StringHash GetType() const = 0;
		virtual const std::string& GetTypeName() const = 0;
		virtual const TypeInfo* GetTypeInfo() const = 0;
		
		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

		bool IsInstanceOf(StringHash type) const;
		bool IsInstanceOf(const TypeInfo* typeInfo) const;

		DvObject* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

		DvContext* GetContext() const { return m_pContext; }

	protected:
		DvContext* m_pContext;
	};

	template<class T>
	T* DvObject::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}
 }