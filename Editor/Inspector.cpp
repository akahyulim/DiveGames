#include "Inspector.h"

// ���� ���õ� GameObject�� Inspector���� �����ش�.
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
			// �ٸ� ������ ���� ����ؾ� �Ѵ�.
			ImGui::Text("%s", m_InspectedTarget->GetName().c_str());

			showTransform(m_InspectedTarget->GetTransform());
			// ���� �ٸ� Components

			showAddComponentButton();
		}
		// else if�� material
	}

	void Inspector::SetInspectGameObject(dive::GameObject* target)
	{
		m_InspectedTarget = target;

		// hint�ΰ� ���� ����
	}
	
	void Inspector::showTransform(dive::Transform* transform)
	{
		assert(transform);
	}
	
	void Inspector::showAddComponentButton()
	{
	}
}