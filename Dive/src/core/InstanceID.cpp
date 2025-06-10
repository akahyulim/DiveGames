#include "stdafx.h"
#include "InstanceID.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<UINT64> s_UniformDistribution;

	InstanceID::InstanceID()
		: m_InstanceID(s_UniformDistribution(s_Engine))
	{
	}

	InstanceID::InstanceID(UINT64 instanceID)
		: m_InstanceID(instanceID)
	{
	}
}
