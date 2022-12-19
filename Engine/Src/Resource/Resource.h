#pragma once
#include "Core/Object.h"
#include "Math/StringHash.h"

namespace Dive 
{
	class Context;
	class FileStream;

	// 리소스의 직렬화 인터페이스와 이름 관리 기능을 가진 리소스 베이스 클래스.
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