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
		// 스파르탄의 경우 이들을 정적 글로벌 변수 선언한 후
		// Inspector의 정적 멤버 함수를 통해 전달했다.
		DiveEngine::GameObject* m_pSelectedGameObject = nullptr;
		DiveEngine::GameObject* m_pClickedGameObject = nullptr;
		DiveEngine::GameObject* m_pHoveredGameObject = nullptr;
		DiveEngine::GameObject* m_pCopiedGameObject = nullptr;

		DragDropPayload m_Payload;
		bool m_bPopupRename = false;
	};
}