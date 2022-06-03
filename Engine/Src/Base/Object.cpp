#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<unsigned long long> s_UniformDistribution;

	// 혹시나 같은 id가 생성될 때를 대비해 hash가 필요?

	Object::Object()
		: m_InstanceID(s_UniformDistribution(s_Engine))
	{
	}

	Object::Object(unsigned long long id)
		: m_InstanceID(id)
	{
	}

	// 괜히 매개변수를 차지한다.
	// 둘 다 함수로 설정할 수 있게끔 하는 것이 나을 것 같다.
	// 아니면 적어도 추후 수정이 필요해야 한다.
	//Object::Object(const std::string& name, unsigned long long id)
	//{
	//	m_Name			= name;
	//	m_InstanceID	= id == 0 ? s_UniformDistribution(s_Engine) : id;
	//}
}