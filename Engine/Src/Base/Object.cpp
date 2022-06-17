#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	// 혹시나 같은 id가 생성될 때를 대비해 hash가 필요?

	Object::Object()
	{
		m_InstanceID = GenerateInstanceID();
	}

	uint64_t Object::GenerateInstanceID()
	{
		return s_UniformDistribution(s_Engine);
	}

	bool Object::operator!=(const Object& rhs)
	{
		return GetInstanceID() != rhs.GetInstanceID();
	}
	
	bool Object::operator==(const Object& rhs)
	{
		return GetInstanceID() == rhs.GetInstanceID();
	}
}