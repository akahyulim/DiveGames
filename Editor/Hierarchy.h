#pragma once
#include "Widget.h"
#include "DragDrop.h"

namespace editor
{
	class Hierarchy : public Widget
	{
	public:
		Hierarchy(Editor* editor);

		void TickVisible() override;

	private:
		void treeShow();
		void treeAddGameObject(dive::GameObject* gameObject);

		void setSelected(dive::GameObject* gameObject);
		void handleClicking();
		void handleDragDrop(dive::GameObject* gameObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

		// m_Scene과 함께 Widget에 넣는 편이 나을 것 같다.
		void OnSetActiveScene();

	private:
		dive::Scene* m_Scene;

		dive::GameObject* m_Selected;
		dive::GameObject* m_Clicked;
		dive::GameObject* m_Hovered;
		dive::GameObject* m_Copied;

		DragDropPayload m_Payload;
		bool m_bPopupRename;
	};
}