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
		dive::GameObject* m_pSelectedGameObject = nullptr;
		dive::GameObject* m_pClickedGameObject = nullptr;
		dive::GameObject* m_pHoveredGameObject = nullptr;
		dive::GameObject* m_pCopiedGameObject = nullptr;

		DragDropPayload m_Payload;
		bool m_bPopupRename = false;
	};
}