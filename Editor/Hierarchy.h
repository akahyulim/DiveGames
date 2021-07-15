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
		void showMainTree();
		void showAddedTree(dive::GameObject* gameObject);

		void setSelectedGameObject(dive::GameObject* gameObject);
		void handleClicking();
		void handleDragDrop(dive::GameObject* gameObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		// ���ĸ�ź�� ��� �̵��� ���� �۷ι� ���� ������ ��
		// Inspector�� ���� ��� �Լ��� ���� �����ߴ�.
		dive::GameObject* m_SelectedGameObject = nullptr;
		dive::GameObject* m_ClickedGameObject = nullptr;
		dive::GameObject* m_HoveredGameObject = nullptr;
		dive::GameObject* m_CopiedGameObject = nullptr;

		DragDropPayload m_Payload;
		bool m_bPopupRename = false;
	};
}