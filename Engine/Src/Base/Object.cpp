#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	// 혹시나 같은 id가 생성될 때를 대비해 hash가 필요?
	// urho3d의 경우 ++를 통해 생성하고, hashmap을 통해 동일한 id가 이미 생성된 것인지 확인한다.
	// 정확하게 말하자면, scene의 GetComponent(id)로 존재 여부를 확인하고, GetFreeComponentID()로 빈 id를 생성한다.
	// 그리고 Node 역시 scene의 GetNode(id)로 찾고, GetFreeNodeID()로 생성한다.
	// 즉, Scene이 id를 따로 관리한다.

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