#include "divepch.h"
#include "Object.h"

namespace Dive
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	// Ȥ�ó� ���� id�� ������ ���� ����� hash�� �ʿ�?
	// urho3d�� ��� ++�� ���� �����ϰ�, hashmap�� ���� ������ id�� �̹� ������ ������ Ȯ���Ѵ�.
	// ��Ȯ�ϰ� �����ڸ�, scene�� GetComponent(id)�� ���� ���θ� Ȯ���ϰ�, GetFreeComponentID()�� �� id�� �����Ѵ�.
	// �׸��� Node ���� scene�� GetNode(id)�� ã��, GetFreeNodeID()�� �����Ѵ�.
	// ��, Scene�� id�� ���� �����Ѵ�.

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