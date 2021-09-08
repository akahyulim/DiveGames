#pragma once
#include "Object.h"
#include <string>

namespace dive
{
	enum class eResourceType
	{
		Unknown,
		Texture2D,
	};

	// eResourceType을 가지는게 맞는 것 같다.
	// 그런데 Object의 TypeHash와 좀 안맞는 것 같다.
	class Resource : public Object
	{
	public:
		Resource(size_t typeHash) : Object(typeHash) {}
		virtual ~Resource() = default;

		// 순수 가상 함수이면 안되나?
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		//virtual bool SaveToFile(const std::string& filepath) { return true; }


		eResourceType GetResourceType() const { return m_Type; }


	protected:
		eResourceType m_Type = eResourceType::Unknown;

		// 스파르탄은 디렉토리까지 같이 관리한다.
		// 그리고 굳이 Object name과 비슷한 Resource Name이란걸 가진다...
	};
}