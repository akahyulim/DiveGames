#pragma once
#include "Math/StringHash.h"
#include "Variant.h"

namespace Dive
{
	class Context;

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

	class Object
	{
	public:
		explicit Object(Context* pContext);
		virtual ~Object() = default;

		virtual StringHash GetType() const = 0;
		virtual const std::string& GetTypeName() const = 0;
		virtual const TypeInfo* GetTypeInfo() const = 0;
		
		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

		bool IsInstanceOf(StringHash type) const;
		bool IsInstanceOf(const TypeInfo* typeInfo) const;

		Object* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

		Context* GetContext() const { return m_pContext; }

	protected:
		Context* m_pContext;
	};

	template<class T>
	T* Object::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}

	class ObjectFactory
	{
	public:
		explicit ObjectFactory(Context* pContext);
		virtual ~ObjectFactory() = default;

		virtual Object* CreateObject() = 0;

		Context* GetContext() const { return m_pContext; }
		const TypeInfo* GetTypeInfo() const { return m_TypeInfo; }
		StringHash GetType() const { return m_TypeInfo->GetType(); }
		std::string GetTypeName() const { return m_TypeInfo->GetTypeName(); }
 
	protected:
		Context* m_pContext;
		const TypeInfo* m_TypeInfo;
	};

	template<class T>
	class ObjectFactoryImpl : public ObjectFactory
	{
	public:
		explicit ObjectFactoryImpl(Context* pContext)
			: ObjectFactory(pContext)
		{
			m_TypeInfo = T::GetTypeInfoStatic();
		}
		
		Object* CreateObject() override { return new T(m_pContext); }
	};
}