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
		void showAddedTree(DiveEngine::GameObject* gameObject);

		void setSelectedGameObject(DiveEngine::GameObject* gameObject);
		void handleClicking();
		void handleDragDrop(DiveEngine::GameObject* gameObject);

		void popupPropertyMenu();
		void popupGameObjectRename();

	private:
		// ���ĸ�ź�� ��� �̵��� ���� �۷ι� ���� ������ ��
		// Inspector�� ���� ��� �Լ��� ���� �����ߴ�.
		DiveEngine::GameObject* m_pSelectedGameObject = nullptr;
		DiveEngine::GameObject* m_pClickedGameObject = nullptr;
		DiveEngine::GameObject* m_pHoveredGameObject = nullptr;
		DiveEngine::GameObject* m_pCopiedGameObject = nullptr;

		DragDropPayload m_Payload;
		bool m_bPopupRename = false;
	};
}