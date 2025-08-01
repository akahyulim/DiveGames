#pragma once

namespace Dive
{
	// https://docs.unity3d.com/ScriptReference/Object.html
	class Object
	{
	public:
		Object();
		Object(const std::string& name);
		virtual ~Object() = default;

		uint64_t GetInstanceID() const { return m_instanceID; }

		virtual std::string GetName() const { return m_name; }
		virtual void SetName(const std::string& name) { m_name = name; }

		bool operator==(const Object& other) const { return m_instanceID == other.m_instanceID; }
		bool operator!=(const Object& other) const { return m_instanceID != other.m_instanceID; }

	private:
		uint64_t m_instanceID;
		std::string m_name;
	};

	inline bool operator==(const Object& lhs, const Object& rhs)
	{
		return lhs.GetInstanceID() == rhs.GetInstanceID();
	}

	inline bool operator!=(const Object& lhs, const Object& rhs)
	{
		return lhs.GetInstanceID() != rhs.GetInstanceID();
	}
}
