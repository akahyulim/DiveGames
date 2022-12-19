#include "divepch.h"
#include "Context.h"
#include "CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Context::~Context()
	{
		for (auto& pFactory : m_Factories)
		{
			DV_DELETE(pFactory.second);
		}
		m_Factories.clear();

		// 순서를 맞춰야 할 수 있다.
		for (auto& pSubsystem : m_Subsystmes)
		{
			DV_DELETE(pSubsystem.second);
		}
		m_Subsystmes.clear();

		DV_LOG_ENGINE_TRACE("Context 소멸 완료");
	}

	void Context::RegisterSubsystem(Object* pObject)
	{
		if (!pObject)
			return;

		m_Subsystmes[pObject->GetTypeHash().GetValue()] = pObject;
	}

	void Context::RemoveSubsystem(StringHash typeHash)
	{
		auto it = m_Subsystmes.find(typeHash.GetValue());
		if (it != m_Subsystmes.end())
		{
			DV_DELETE(it->second);
			m_Subsystmes.erase(it);
		}
	}

	Object* Context::GetSubsystem(StringHash typeHash) const
	{
		auto it = m_Subsystmes.find(typeHash.GetValue());
		if (it != m_Subsystmes.end())
			return (it->second);
	
		return nullptr;
	}

	void Context::RegisterFactory(ObjectFactory* pFactory)
	{
		if (!pFactory)
			return;

		m_Factories[pFactory->GetTypeHash().GetValue()] = pFactory;
	}
	
	Object* Context::CreateObject(StringHash typeHash)
	{
		auto it = m_Factories.find(typeHash.GetValue());
		if (it != m_Factories.end())
			return it->second->CreateObject();

		return nullptr;
	}
}