#pragma once
#include "DivePch.h"

namespace Dive
{
	class SystemManager;

	class ISystem// : public std::enable_shared_from_this<ISystem>
	{
	public:
		ISystem(SystemManager* manager) : m_Manager(manager) {}
		virtual ~ISystem() = default;

		virtual bool Initialize()	= 0;
		virtual void Update()		= 0;

		//template<typename T>
		//std::shared_ptr<T> GetSharedPtr() { return std::dynamic_pointer_cast<T>(shared_from_this()); }

	protected:
		SystemManager* m_Manager = nullptr;
	};

	template<typename T>
	constexpr void ValidateSystemType()
	{
		static_assert(std::is_base_of<ISystem, T>::value, "ISystem을 상속하는 타입이 아닙니다.");
	}
}