#include "LightViewer.h"

int main()
{
	LightViewer app;
	if (app.Initialize())
	{
		DV_TRACE("��~��!");
		while (app.Run())
		{
		}
		DV_TRACE("�ߴ�!");
	}

	DV_TRACE("����");

	return 0;
}