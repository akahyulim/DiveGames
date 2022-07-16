#pragma once
#include "StringHash.h"
#include "Variant.h"

namespace Dive
{
	class DvContext;
	class DvEventHandler;

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
		virtual void OnEvent(DvObject* pSender, StringHash eventType, VariantMap& eventData);

		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

		bool IsInstanceOf(StringHash type) const;
		bool IsInstanceOf(const TypeInfo* typeInfo) const;

		// evnet control
		void SubscribeEvent(StringHash eventType, DvEventHandler* pHandler);
		void SubscribeEvent(DvObject* pSender, StringHash eventType, DvEventHandler* pHandler);
		void UnsubscribeEvent(StringHash eventType);
		void SendEvent(StringHash eventType);
		void SendEvent(StringHash eventType, VariantMap& eventData);
		
		// get subsystem
		DvObject* GetSubsystem(StringHash type) const;
		template<class T> T* GetSubsystem() const;

		DvContext* GetContext() const { return m_pContext; }

	protected:
		DvContext* m_pContext;

	private:
		// event handler
		std::list<DvEventHandler*> m_EventHandlers;
		// bool m_bBlockEvents;
	};

	template<class T>
	T* DvObject::GetSubsystem() const
	{
		return static_cast<T*>(GetSubsystem(T::GetTypeStatic()));
	}

	// functual을 가지는 구체 클래스도 있다.
	class DvEventHandler
	{
	public:
		explicit DvEventHandler(DvObject* pReceiver, void* pUserData = nullptr)
			: m_pSender(nullptr),
			m_pReceiver(pReceiver),
			m_pUserData(pUserData)
		{}
		virtual ~DvEventHandler() = default;

		void SetSenderAndEventType(DvObject* pSender, StringHash evnetType)
		{
			m_pSender = pSender;
			m_EventType = evnetType;
		}

		virtual void Invoke(VariantMap& eventData) = 0;
		virtual DvEventHandler* Clone() const = 0;

		DvObject* GetSender() const { return m_pSender; }
		DvObject* GetReceiver() const { return m_pReceiver; }
		StringHash GetEventType() const { return m_EventType; }
		void* GetUserData() const { return m_pUserData; }

	private:

	protected:
		DvObject* m_pSender;
		DvObject* m_pReceiver;
		StringHash m_EventType;
		void* m_pUserData;
	};
}