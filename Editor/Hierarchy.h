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
		void treeAddGameObject(Dive::GameObject* pGameObject);

		void setSelected(Dive::GameObject* pGameObject);
		void handleClicking();
		void handleDragDrop(Dive::GameObject* pGameObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		Dive::Scene* m_pScene;

		Dive::GameObject* m_pSelected;
		Dive::GameObject* m_pClicked;
		Dive::GameObject* m_pHovered;
		Dive::GameObject* m_pCopied;

		DragDropPayload m_payload;
		bool m_bPopupRename;
	};
}