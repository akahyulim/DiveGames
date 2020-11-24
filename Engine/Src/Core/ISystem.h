#pragma once
#include "DivePch.h"


namespace Dive
{
	class SystemManager;

	class ISystem : public std::enable_shared_from_this<ISystem>
	{
	public:
		ISystem(const std::shared_ptr<SystemManager>& manager) : m_Manager(manager) {}
		virtual ~ISystem() = default;

		virtual bool Initialize()	= 0;
		virtual void Update()		= 0;

	protected:
		std::weak_ptr<SystemManager> m_Manager;
	};

	template<typename T>
	constexpr void ValidateSystemType()
	{
		static_assert(std::is_base_of<ISystem, T>::value, "");
	}
}