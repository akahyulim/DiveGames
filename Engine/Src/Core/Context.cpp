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
		DV_LOG_ENGINE_DEBUG("Context �Ҹ��� ȣ��");

		// remove subsystems: audio, ui, input, renderer, graphics
		// �����츦 ����ϴ� ��ü�鸸 �����ϴ� �� �ϴ�.
		// => �ƹ����� ��� ��ü�� shared_ptr�� �⺻�� �� �ϴ�.
		// �� ������ Ư���� ������ ������ ���Ѿ� �ϱ� ������ ��
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