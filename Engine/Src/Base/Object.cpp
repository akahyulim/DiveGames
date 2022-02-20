#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<unsigned long long> s_UniformDistribution;

	Object::Object(const std::string& name)
		: m_InstanceID(s_UniformDistribution(s_Engine)), m_Name(name)
	{
	}

	Object::Object(unsigned long long id, const std::string& name)
		: m_InstanceID(id), m_Name(name)
	{
	}
}