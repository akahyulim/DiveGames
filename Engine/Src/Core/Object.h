#pragma once


namespace Dive
{
	class Context;

	class Object
	{
	public:
		Object(Context* context);
		virtual ~Object() = default;

		virtual size_t GetTypeHash()			const = 0;
		virtual std::string GetTypeName()		const = 0;
		virtual size_t GetBaseTypeHash()		const = 0;
		virtual std::string GetBaseTypeName()	const = 0;

		template<class T> std::shared_ptr<T> GetSubsystem()
		{
			if (!m_context)
				return nullptr;

			return m_context->GetSubsystem<T>();
		}

	protected:
		Context* m_context;
	};
}

#define DIVE_OBJECT(typeName, baseTypeName)														\
public:																							\
virtual size_t GetTypeHash() const override { return typeid(typeName).hash_code(); }			\
virtual std::string GetTypeName() const override { return typeid(typeName).name(); }			\
virtual size_t GetBaseTypeHash() const override { return typeid(baseTypeName).hash_code(); }	\
virtual std::string GetBaseTypeName() const override { return typeid(baseTypeName).name(); }
// static을 추가하면 타입으로도 위의 값들을 확인할 수 있다.