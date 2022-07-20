#include "divepch.h"
#include "DvContext.h"
#include "DvObject.h"

namespace Dive
{
	DvContext::DvContext()
	{
	}
	
	DvContext::~DvContext()
	{
		// remove subsystems: audio, ui, input, renderer, graphics
		// 윈도우 때문에 graphics가 가장 마지막인듯
		// 전부 다 가 아니다...

		m_Subsystems.clear();

	}

	void DvContext::RegisterSubsystem(DvObject* pObject)
	{
		if (!pObject)
			return;

		m_Subsystems[pObject->GetType()] = pObject;
	}

	void DvContext::RemoveSubsystem(StringHash type)
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			m_Subsystems.erase(it);
	}

	DvObject* DvContext::GetSubsystem(StringHash type) const
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			return it->second;
	
		return nullptr;
	}
}