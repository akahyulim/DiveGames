#include "divepch.h"
#include "DvContext.h"
#include "DvObject.h"
#include "Base/Base.h"	// core�� �̵�?

namespace Dive
{
	DvContext::DvContext()
	{
	}
	
	DvContext::~DvContext()
	{
		// remove subsystems: audio, ui, input, renderer, graphics
		// �����츦 ����ϴ� ��ü�鸸 �����ϴ� �� �ϴ�.
		// => �ƹ����� ��� ��ü�� shared_ptr�� �⺻�� �� �ϴ�.
		// �� ������ Ư���� ������ ������ ���Ѿ� �ϱ� ������ ��
		RemoveSubsystem("DvRenderer");
		RemoveSubsystem("DvGraphics");
	
		m_Subsystems.clear();
	}

	void DvContext::RegisterSubsystem(std::shared_ptr<DvObject> pObject)
	{
		if (!pObject.get())
			return;

		m_Subsystems[pObject->GetType()] = std::move(pObject);
	}

	void DvContext::RemoveSubsystem(StringHash type)
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
		{
			(it->second).reset();
			m_Subsystems.erase(it);
		}
	}

	DvObject* DvContext::GetSubsystem(StringHash type) const
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			return (it->second).get();
	
		return nullptr;
	}
}