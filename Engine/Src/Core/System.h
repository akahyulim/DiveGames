#pragma once
#include "DivePch.h"


namespace Dive
{
	class SystemManager;

	class System : public std::enable_shared_from_this<System>
	{
	public:
		System(const std::shared_ptr<SystemManager>& manager) : m_manager(manager) {}
		virtual ~System() = default;

		virtual bool Initialize()	= 0;
		virtual void Update()		= 0;

	protected:
		std::weak_ptr<SystemManager> m_manager;
	};

	template<typename T>
	constexpr void ValidateSystemType()
	{
		static_assert(std::is_base_of<System, T>::value, "");
	}
}