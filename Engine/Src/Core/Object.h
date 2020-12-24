#pragma once
#include "DivePch.h"
#include "Context.h"


namespace Dive
{
	static unsigned int g_ID = 0;

	class Object
	{
	public:
		Object(Context* context)
		{
			m_context = context;
			m_ID = generateID();
		}
		virtual ~Object() = default;

		virtual size_t GetTypeHash()			const = 0;
		virtual std::string GetTypeName()		const = 0;
		virtual size_t GetBaseTypeHash()		const = 0;
		virtual std::string GetBaseTypeName()	const = 0;
		unsigned int GetID()					const { return m_ID; }

		template<class T> std::shared_ptr<T> GetSubsystem()
		{
			if (!m_context)
				return nullptr;

			return m_context->GetSubsystem<T>();
		}

	protected:
		unsigned int generateID() { return ++g_ID; }

	protected:
		Context* m_context;
		unsigned int m_ID;
	};
}

#define DIVE_OBJECT(typeName, baseTypeName)														\
public:																							\
virtual size_t GetTypeHash() const override { return typeid(typeName).hash_code(); }			\
virtual std::string GetTypeName() const override { return typeid(typeName).name(); }			\
virtual size_t GetBaseTypeHash() const override { return typeid(baseTypeName).hash_code(); }	\
virtual std::string GetBaseTypeName() const override { return typeid(baseTypeName).name(); }