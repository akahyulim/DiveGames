#pragma once

namespace Dive
{
	class Object
	{
	public:
		Object(const std::string& name = std::string());
		Object(unsigned long long id, const std::string& name = std::string());
		virtual ~Object() = default;

		unsigned long long GetInstanceID() const { return m_InstanceID; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		Object(const Object&) = delete;

	protected:
		unsigned long long m_InstanceID;
		std::string m_Name;
	};
}