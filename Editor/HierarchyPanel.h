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
		// 스파르탄의 경우 이들을 정적 글로벌 변수 선언한 후
		// Inspector의 정적 멤버 함수를 통해 전달했다.
		Dive::GameObject* m_pSelectedObject = nullptr;
		Dive::GameObject* m_pClickedObject = nullptr;
		Dive::GameObject* m_pHoveredObject = nullptr;
		Dive::GameObject* m_pCopiedObject = nullptr;

		DragDropPayload m_payload;
		bool m_bPopupRename = false;
	};
}