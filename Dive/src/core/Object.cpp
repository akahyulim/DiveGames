#include "stdafx.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	Object::Object()
		: m_InstanceID(s_UniformDistribution(s_Engine))
		, m_Name("")
	{
	}

	Object::Object(const std::string& name)
		: m_InstanceID(s_UniformDistribution(s_Engine))
		, m_Name(name)
	{
	}
}
