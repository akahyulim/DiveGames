#include "divepch.h"
#include "Context.h"
#include "CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Context::~Context()
	{
		DV_LOG_ENGINE_TRACE("Context 소멸자 호출");

		for (auto& pFactory : m_Factories)
		{
			DV_DELETE(pFactory.second);
		}
		m_Factories.clear();

		// 순서를 맞춰야 할 수 있다.
		for (auto& pSubsystem : m_Subsystems)
		{
			DV_DELETE(pSubsystem.second);
		}
		m_Subsystems.clear();
	}

	void Context::RegisterSubsystem(Object* pObject)
	{
		if (!pObject)
			return;

		m_Subsystems[pObject->GetType().Value()] = pObject;
	}

	void Context::RemoveSubsystem(StringHash type)
	{
		auto it = m_Subsystems.find(type.Value());
		if (it != m_Subsystems.end())
		{
			DV_DELETE(it->second);
			m_Subsystems.erase(it);
		}
	}

	Object* Context::GetSubsystem(StringHash type) const
	{
		auto it = m_Subsystems.find(type.Value());
		if (it != m_Subsystems.end())
			return (it->second);
	
		return nullptr;
	}

	void Context::RegisterFactory(ObjectFactory* pFactory)
	{
		if (!pFactory)
			return;

		m_Factories[pFactory->GetType().Value()] = pFactory;
	}
	
	Object* Context::CreateObject(StringHash type)
	{
		auto it = m_Factories.find(type.Value());
		if (it != m_Factories.end())
			return it->second->CreateObject();

		return nullptr;
	}
}