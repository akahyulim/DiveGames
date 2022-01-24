#pragma once
#include "Panel.h"
#include "DragDrop.h"

namespace Dive
{
	class HierarchyPanel : public Panel
	{
	public:
		HierarchyPanel(Editor* pEditor);

		void TickVisible() override;

	private:
		void showMainTree();
		void showAddedTree(Dive::GameObject* pObject);

		void setSelectedGameObject(Dive::GameObject* pObject);
		void handleClicking();
		void handleDragDrop(Dive::GameObject* pObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		// ���ĸ�ź�� ��� �̵��� ���� �۷ι� ���� ������ ��
		// Inspector�� ���� ��� �Լ��� ���� �����ߴ�.
		Dive::GameObject* m_pSelectedObject = nullptr;
		Dive::GameObject* m_pClickedObject = nullptr;
		Dive::GameObject* m_pHoveredObject = nullptr;
		Dive::GameObject* m_pCopiedObject = nullptr;

		DragDropPayload m_payload;
		bool m_bPopupRename = false;
	};
}