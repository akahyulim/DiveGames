#pragma once

namespace Dive
{
	class Object
	{
	public:
		Object();
		virtual ~Object() = default;

		uint64_t GetID() const { return m_ID; }
		void SetID(uint64_t id) { m_ID = id; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	protected:
		uint64_t m_ID;
		std::string m_Name;
	};
}