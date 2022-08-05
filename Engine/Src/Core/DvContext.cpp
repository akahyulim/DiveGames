#include "divepch.h"
#include "DvContext.h"
#include "DvObject.h"
#include "Base/Base.h"	// core로 이동?
#include "IO/DvLog.h"

namespace Dive
{
	DvContext::DvContext()
	{
	}
	
	DvContext::~DvContext()
	{
		DV_LOG_ENGINE_DEBUG("DvContext 소멸자 호출");

		// remove subsystems: audio, ui, input, renderer, graphics
		// 윈도우를 사용하는 객체들만 제거하는 듯 하다.
		// => 아무래도 모든 객체는 shared_ptr이 기본인 듯 하다.
		// 이 구문이 특별한 이유는 순서를 지켜야 하기 때문인 듯
		RemoveSubsystem("DvRenderer");
		RemoveSubsystem("DvGraphics");
	
		m_Subsystems.clear();
	}

	void DvContext::RegisterSubsystem(std::shared_ptr<DvObject> pObject)
	{
		if (!pObject)
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