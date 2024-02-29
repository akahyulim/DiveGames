#pragma once

namespace Dive
{
	class TypeInfo
	{
	public:
		TypeInfo(const std::string& pTypeName, const TypeInfo* pBaseTypeInfo);
		virtual ~TypeInfo() = default;

		bool IsTypeOf(size_t typeHash) const;
		bool IsTypeOf(const TypeInfo* pTypeInfo) const;
		template<typename T> bool IsTypeOf() const;

		size_t GetTypeHash() const { return m_TypeHash; }
		const std::string& GetTypeName() const { return m_TypeName; }
		const TypeInfo* GetBaseTypeInfo() const { return m_pBaseTypeInfo; }

	private:
		size_t m_TypeHash;
		std::string m_TypeName;

		const TypeInfo* m_pBaseTypeInfo;
	};

	template<typename T>
	inline bool TypeInfo::IsTypeOf() const
	{
		return IsTypeOf(T::GetTypeInfoStatic());
	}
}

#define DIVE_TYPEINFO(typeName, baseTypeName) \
	public: \
		virtual size_t GetTypeHash() const override { return GetTypeInfoStatic()->GetTypeHash(); }	\
		virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); }	\
		virtual const TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); }	\
		static size_t GetTypeHashStatic() { return GetTypeInfoStatic()->GetTypeHash(); }	\
		static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }	\
		static const TypeInfo* GetTypeInfoStatic() { static const TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; }