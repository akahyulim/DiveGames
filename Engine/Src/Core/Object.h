#pragma once
#include "Math/StringHash.h"

namespace Dive
{
	class Context;

	// StringHash를 활용한 타입 정보 클래스.
	class TypeInfo
	{
	public:
		TypeInfo(const char* pTypeName, const TypeInfo* pBaseTypeInfo);
		~TypeInfo() = default;

		bool IsTypeOf(StringHash typeHash) const;
		bool IsTypeOf(const TypeInfo* pTypeInfo) const;

		StringHash GetTypeHash() const { return m_Hash; }
		const std::string& GetTypeName() const { return m_Name; }
		const TypeInfo* GetBaseTypeInfo() const { return m_BaseTypeInfo; }

		template<typename T>
		bool IsTypeOf() const
		{
			return IsTypeOf(T::GetTypeInfoStatic());
		}

	private:
		StringHash m_Hash;
		std::string m_Name;
		const TypeInfo* m_BaseTypeInfo;
	};

// 오브젝트의 타입 함수 매크로.
#define DIVE_OBJECT(ClassName, BaseClassName)	\
public:\
	virtual Dive::StringHash GetTypeHash() const override { return GetTypeInfoStatic()->GetTypeHash(); }	\
	virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); }	\
	virtual const Dive::TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); }	\
	static Dive::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetTypeHash(); }	\
	static const std::string GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }	\
	static const Dive::TypeInfo* GetTypeInfoStatic() \
	{	\
		static const Dive::TypeInfo typeInfoStatic(#ClassName, BaseClassName::GetTypeInfoStatic());	\
		return &typeInfoStatic;	\
	}

	// 타입 및 상속관계 정보를 제공하는 오브젝트의 기본 클래스.
	class Object
	{
	public:
		explicit Object(Context* pContext);
		virtual ~Object() = default;

		virtual StringHash GetTypeHash() const = 0;
		virtual const std::string& GetTypeName() const = 0;
		virtual const TypeInfo* GetTypeInfo() const = 0;
		
		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

		bool IsInstanceOf(StringHash typeHash) const;
		bool IsInstanceOf(const TypeInfo* pTypeInfo) const;

		Object* GetSubsystem(StringHash typeHash) const;
		template<class T> T* GetSubsystem() const;

		Context* GetContext() const { return m_pContext; }

	protected:
		Context* m_pContext;
	};

	template<class T>
	T* Object::GetSubsystem() const
	{
		return dynamic_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}

	// 오브젝트 생성을 위한 팩토리 베이스 클래스.
	class ObjectFactory
	{
	public:
		explicit ObjectFactory(Context* pContext);
		virtual ~ObjectFactory() = default;

		virtual Object* CreateObject() = 0;

		Context* GetContext() const { return m_pContext; }
		const TypeInfo* GetTypeInfo() const { return m_pTypeInfo; }
		StringHash GetTypeHash() const { return m_pTypeInfo->GetTypeHash(); }
		std::string GetTypeName() const { return m_pTypeInfo->GetTypeName(); }
 
	protected:
		Context* m_pContext;
		const TypeInfo* m_pTypeInfo;
	};

	// 오브젝트 생성을 구현한 구체 팩토리 클래스.
	template<class T>
	class ObjectFactoryImpl : public ObjectFactory
	{
	public:
		explicit ObjectFactoryImpl(Context* pContext)
			: ObjectFactory(pContext)
		{
			m_pTypeInfo = T::GetTypeInfoStatic();
		}
		
		Object* CreateObject() override { return static_cast<Object*>(new T(m_pContext)); }
	};
}