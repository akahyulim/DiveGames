#include "Inspector.h"

// 현재 선택된 GameObject의 Inspector들을 보여준다.
namespace editor
{
	dive::GameObject* Inspector::m_InspectedTarget = nullptr;

	Inspector::Inspector(Editor* editor)
		: Widget(editor)
	{
		m_Title = "Inspector";
	}

	void Inspector::TickVisible()
	{
		if (m_InspectedTarget)
		{
			// 다른 정보를 먼저 출력해야 한다.
			ImGui::Text("%s", m_InspectedTarget->GetName().c_str());

			showTransform(m_InspectedTarget->GetTransform());
			// 이하 다른 Components

			showAddComponentButton();
		}
		// else if로 material
	}

	void Inspector::SetInspectGameObject(dive::GameObject* target)
	{
		m_InspectedTarget = target;

		// hint인가 뭔가 설정
	}
	
	void Inspector::showTransform(dive::Transform* transform)
	{
		assert(transform);
	}
	
	void Inspector::showAddComponentButton()
	{
	}
}