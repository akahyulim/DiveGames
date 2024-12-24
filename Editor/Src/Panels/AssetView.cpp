#include "AssetView.h"

namespace Dive
{
	AssetView::AssetView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "Assets";
	}

	AssetView::~AssetView()
	{
	}
}