#include "divepch.h"
#include "Context.h"
#include "CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Context::Context()
	{
	}
	
	Context::~Context()
	{
		DV_LOG_ENGINE_DEBUG("Context 소멸자 호출");

		// remove subsystems: audio, ui, input, renderer, graphics
		// 윈도우를 사용하는 객체들만 제거하는 듯 하다.
		// => 아무래도 모든 객체는 shared_ptr이 기본인 듯 하다.
		// 이 구문이 특별한 이유는 순서를 지켜야 하기 때문인 듯
		RemoveSubsystem("Renderer");
		RemoveSubsystem("Graphics");
	
		m_Subsystems.clear();
	}

	void Context::RegisterSubsystem(std::shared_ptr<Object> pObject)
	{
		if (!pObject)
			return;

		m_Subsystems[pObject->GetType()] = std::move(pObject);
	}

	void Context::RemoveSubsystem(StringHash type)
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
		{
			(it->second).reset();
			m_Subsystems.erase(it);
		}
	}

	Object* Context::GetSubsystem(StringHash type) const
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			return (it->second).get();
	
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