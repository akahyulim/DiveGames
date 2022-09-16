#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"

namespace Dive 
{
	class Context;
	class FileStream;

	class Resource : public Object
	{
		DIVE_OBJECT(Resource, Object)

	public:
		explicit Resource(Context* pContext);
		~Resource() override;
	
		virtual bool LoadFromFile(const std::string& fileName);
		virtual bool SaveToFile(const std::string& fileName);

		virtual bool Load(FileStream* pDeserializer);
		virtual bool Save(FileStream* pSerializer);
	
		std::string GetName() const { return m_Name; }
		StringHash GetNameHash() const { return m_NameHash; }
		void SetName(const std::string& name);

	private:
	private:
		std::string m_Name;
		StringHash m_NameHash;
	};
}