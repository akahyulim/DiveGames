#pragma once
#include "core/InstanceID.h"

namespace Dive
{
	class TypeInfo
	{
	public:
		TypeInfo(const std::string& typeName, const TypeInfo* baseTypeInfo);
		virtual ~TypeInfo() = default;

		bool IsTypeOf(size_t typeHash) const;
		bool IsTypeOf(const TypeInfo* typeInfo) const;
		template<typename T> bool IsTypeOf() const;

		size_t GetTypeHash() const { return m_TypeHash; }
		const std::string& GetTypeName() const { return m_TypeName; }
		const TypeInfo* GetBaseTypeInfo() const { return m_BaseTypeInfo; }

	private:
		size_t m_TypeHash;
		std::string m_TypeName;

		const TypeInfo* m_BaseTypeInfo;
	};

	template<typename T>
	inline bool TypeInfo::IsTypeOf() const
	{
		return IsTypeOf(T::GetTypeInfoStatic());
	}

#define DV_CLASS(typeName, baseTypeName) \
	public: \
		virtual size_t GetTypeHash() const override { return GetTypeInfoStatic()->GetTypeHash(); }	\
		virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); }	\
		virtual const TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); }	\
		static size_t GetTypeHashStatic() { return GetTypeInfoStatic()->GetTypeHash(); }	\
		static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }	\
		static const TypeInfo* GetTypeInfoStatic() { static const TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; }

	class Resource
	{
	public:
		Resource() : m_InstanceID(InstanceID()) {}
		virtual ~Resource();

		virtual size_t GetTypeHash() const = 0;
		virtual const std::string& GetTypeName() const = 0;
		virtual const TypeInfo* GetTypeInfo() const = 0;
		static const TypeInfo* GetTypeInfoStatic() { return nullptr; }

		UINT64 GetInstanceID() const { return m_InstanceID; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::filesystem::path& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::filesystem::path& path);

		virtual bool LoadFromFile(const std::filesystem::path& path) { return true; }
		virtual bool SaveToFile(const std::filesystem::path& path) { return true; }

	protected:
		UINT64 m_InstanceID;
		std::string m_Name{};
		std::filesystem::path m_Filepath{};
	};
}
