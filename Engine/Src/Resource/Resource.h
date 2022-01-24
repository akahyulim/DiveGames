#pragma once
#include "../Core/Object.h"
#include <string>

namespace Dive
{
	enum class eResourceType
	{
		Unknown,
		Texture2D,
		Mesh,
		Static_Mesh,
		Skinned_Mesh,
		Material
	};

	// eResourceType을 가지는게 맞는 것 같다.
	// 그런데 Object의 TypeHash와 좀 안맞는 것 같다.
	class Resource : public Object
	{
	public:
		Resource();
		virtual ~Resource() = default;

		// 이렇게 하면 개별 파일화하겠다는 거다.
		virtual bool SaveToFile(const std::string& filepath) { return true; }
		virtual bool LoadFromFile(const std::string& filepath) { return true; }
		

		eResourceType GetType() const { return m_Type; }

		template<class T>
		static constexpr eResourceType TypeToEnum();

	protected:
		eResourceType m_Type = eResourceType::Unknown;

		// 스파르탄은 디렉토리까지 같이 관리한다.
		// 그리고 굳이 Object name과 비슷한 Resource Name이란걸 가진다...
	};
}