#pragma once
#include "View.h"

namespace Dive
{
	class InspectorView : public View
	{
	public:
		InspectorView(Editor* editor);
		virtual ~InspectorView();

		void drawView() override;

		static void Inspect(GameObject* gameObject);
		// ��Ƽ���� ������ �ִ�.

	private:
		static GameObject* s_InspectedGameObject;
	};
}
