#pragma once
#include "Panel.h"

namespace Dive
{
	class InspectorView : public Panel
	{
	public:
		InspectorView(Editor* pEditor);
		virtual ~InspectorView();
	};
}
