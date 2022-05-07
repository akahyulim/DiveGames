#pragma once
#include "Base/Object.h"

namespace Dive
{
	static const char* EXTENSION_MODEL		= "model";
	static const char* EXTENSION_TEXTURE	= "texture";

	enum class eResourceType
	{
		Texture,
		Texture2D,
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

		//virtual bool SaveToFile(const std::string& filepath) { return true; }
		virtual bool LoadFromFile(const std::string& filepath)	{ return true; }

		virtual eResourceType GetType()		const = 0;
		virtual const char* GetTypeStr()	const = 0;

		std::string GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::string& filepath) { m_Filepath = filepath; }
		
	protected:
		std::string m_Filepath;
	};
}