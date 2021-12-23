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
		// 스파르탄의 경우 이들을 정적 글로벌 변수 선언한 후
		// Inspector의 정적 멤버 함수를 통해 전달했다.
		DiveEngine::GameObject* m_pSelectedObject = nullptr;
		DiveEngine::GameObject* m_pClickedObject = nullptr;
		DiveEngine::GameObject* m_pHoveredObject = nullptr;
		DiveEngine::GameObject* m_pCopiedObject = nullptr;

		DragDropPayload m_payload;
		bool m_bPopupRename = false;
	};
}