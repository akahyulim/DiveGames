#pragma once
#include "DivePch.h"
#include "Subsystem.h"


namespace Dive
{
	class Context
	{
	public:
		Context() = default;
		~Context()
		{
			for (size_t i = 0; i != m_subsystems.size(); i++)
				m_subsystems[i].reset();
			m_subsystems.clear();
		}

		template<typename T>
		void RegisterSubsystem();

		template<typename T>
		void RemoveSubsystem();

		template<typename T>
		std::shared_ptr<T> GetSubsystem();

	private:
		std::vector<std::shared_ptr<Subsystem>> m_subsystems;
	};

	template<typename T>
	void Context::RegisterSubsystem()
	{
		m_subsystems.emplace_back(std::make_shared<T>(this));
	}

	template<typename T>
	void Context::RemoveSubsystem()
	{
	}

	template<typename T>
	std::shared_ptr<T> Context::GetSubsystem()
	{
		for (auto& subsystem : m_subsystems)
		{
			if (typeid(T) == typeid(*subsystem.get()))
				return std::static_pointer_cast<T>(subsystem);
		}

		return nullptr;
	}
}