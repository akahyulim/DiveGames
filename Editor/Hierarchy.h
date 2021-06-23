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

	private:
		dive::Scene* mScene;

		dive::GameObject* mSelected;
		dive::GameObject* mClicked;
		dive::GameObject* mHovered;
		dive::GameObject* mCopied;

		DragDropPayload mPayload;
		bool mbPopupRename;
	};
}