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
		// 스파르탄의 경우 이들을 정적 글로벌 변수 선언한 후
		// Inspector의 정적 멤버 함수를 통해 전달했다.
		dive::GameObject* m_pSelectedGameObject = nullptr;
		dive::GameObject* m_pClickedGameObject = nullptr;
		dive::GameObject* m_pHoveredGameObject = nullptr;
		dive::GameObject* m_pCopiedGameObject = nullptr;

		DragDropPayload m_Payload;
		bool m_bPopupRename = false;
	};
}