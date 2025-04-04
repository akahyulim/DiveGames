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
		// 머티리얼 버전도 있다.

	private:
		static GameObject* s_InspectedGameObject;
	};
}
