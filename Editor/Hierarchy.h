#pragma once
#include "Widget.h"
#include "DragDrop.h"

namespace Editor
{
	class Hierarchy : public Widget
	{
	public:
		Hierarchy(Editor* pEditor);

		void TickVisible() override;

	private:
		void treeShow();
		void treeAddGameObject(dive::GameObject* pGameObject);

		void setSelected(dive::GameObject* pGameObject);
		void handleClicking();
		void handleDragDrop(dive::GameObject* pGameObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		dive::Scene* m_pScene;

		dive::GameObject* m_pSelected;
		dive::GameObject* m_pClicked;
		dive::GameObject* m_pHovered;
		dive::GameObject* m_pCopied;

		DragDropPayload m_payload;
		bool m_bPopupRename;
	};
}