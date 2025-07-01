#pragma once

namespace Dive
{
	using UINT64 = UINT64;

	// https://docs.unity3d.com/ScriptReference/Object.html
	class Object
	{
	public:
		Object();
		Object(const std::string& name);
		virtual ~Object() = default;

		UINT64 GetInstanceID() const { return m_InstanceID; }

		virtual std::string GetName() { return m_Name; }
		virtual void SetName(const std::string& name) { m_Name = name; }

		bool operator==(const Object& other) const { return m_InstanceID == other.m_InstanceID; }
		bool operator!=(const Object& other) const { return m_InstanceID != other.m_InstanceID; }

	private:
		UINT64 m_InstanceID;
		std::string m_Name;
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