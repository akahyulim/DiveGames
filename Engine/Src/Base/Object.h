#pragma once

namespace Dive
{
	class Object
	{
	public:
		Object();
		virtual ~Object() = default;

		uint64_t GetInstanceID() const { return m_InstanceID; }
		void SetInstanceID(uint64_t id) { m_InstanceID = id; }
		static uint64_t GenerateInstanceID();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		bool operator!=(const Object& rhs);
		bool operator==(const Object& rhs);

	private:
		Object(const Object&) = delete;

	protected:
		unsigned long long m_InstanceID;
		std::string m_Name;
	};
}