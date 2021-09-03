#pragma once
#include "Object.h"
#include <string>

namespace dive
{
	class Resource : public Object
	{
	public:
		Resource(size_t typeHash) : Object(typeHash) {}
		virtual ~Resource() = default;

		// 순수 가상 함수이면 안되나?
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		virtual bool SaveToFile(const std::string& filepath) { return true; }


		// get resource type


	protected:

		// 스파르탄은 디렉토리까지 같이 관리한다.
		// 그리고 굳이 Object name과 비슷한 Resource Name이란걸 가진다...
	};
}