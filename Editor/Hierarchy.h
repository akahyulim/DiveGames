#pragma once
#include "Widget.h"
#include "DragDrop.h"

namespace editor
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
		dive::Scene* mpScene;

		dive::GameObject* mpSelected;
		dive::GameObject* mpClicked;
		dive::GameObject* mpHovered;
		dive::GameObject* mpCopied;

		DragDropPayload mPayload;
		bool mbPopupRename;
	};
}