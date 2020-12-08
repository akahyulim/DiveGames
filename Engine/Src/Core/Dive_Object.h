#pragma once


namespace Dive
{
	class Context;

	class Dive_Object
	{
	public:
		Dive_Object(Context* context);
		virtual ~Dive_Object() = default;

		virtual size_t GetTypeHash()		const = 0;
		virtual std::string GetTypeName()	const = 0;

	protected:
		Context* m_context;
	};
}

#define NEW_DIVE_OBJECT(typeName) \
public: \
virtual size_t GetTypeHash() const override { return typeid(typeName).hash_code(); } \
virtual std::string GetTypeName() const override { return typeid(typeName).name(); }