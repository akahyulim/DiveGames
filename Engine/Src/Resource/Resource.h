#pragma once
#include "Base/Object.h"

namespace Dive
{
	enum class eResourceType
	{
		Unknown = 0,
		Texture,
		Model,
		Count
	};

#define RESOURCE_CLASS_TYPE(type)	static eResourceType GetStaticType() { return eResourceType::type;} \
									virtual eResourceType GetType() const override { return GetStaticType(); } \
									virtual const char* GetTypeStr() const override { return #type;} 
 
	class Resource : public Object
	{
	public:
		Resource(const std::string& name = "", unsigned long long id = 0);
		virtual ~Resource() = default;

		virtual bool LoadFromFile(const std::string& filepath)	{ return true; }
		virtual bool SaveToFile(const std::string& filepath)	{ return true; }

		virtual eResourceType GetType()		const = 0;
		virtual const char* GetTypeStr()	const = 0;

		std::string GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::string& filepath) { m_Filepath = filepath; }
		
	protected:
		eResourceType m_Type = eResourceType::Unknown;
		std::string m_Filepath;
	};
}