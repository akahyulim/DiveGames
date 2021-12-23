#pragma once
#include "Widget.h"
#include "DragDrop.h"

namespace DiveEditor
{
	class Hierarchy : public Widget
	{
	public:
		Hierarchy(Editor* pEditor);

		void TickVisible() override;

	private:
		void showMainTree();
		void showAddedTree(DiveEngine::GameObject* pObject);

		void setSelectedGameObject(DiveEngine::GameObject* pObject);
		void handleClicking();
		void handleDragDrop(DiveEngine::GameObject* pObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		// ���ĸ�ź�� ��� �̵��� ���� �۷ι� ���� ������ ��
		// Inspector�� ���� ��� �Լ��� ���� �����ߴ�.
		DiveEngine::GameObject* m_pSelectedObject = nullptr;
		DiveEngine::GameObject* m_pClickedObject = nullptr;
		DiveEngine::GameObject* m_pHoveredObject = nullptr;
		DiveEngine::GameObject* m_pCopiedObject = nullptr;

		DragDropPayload m_payload;
		bool m_bPopupRename = false;
	};
}