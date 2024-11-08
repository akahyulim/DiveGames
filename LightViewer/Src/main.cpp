#include "LightViewer.h"

int main()
{
	LightViewer app;
	if (app.Initialize())
	{
		DV_TRACE("시~작!");
		while (app.Run())
		{
		}
		DV_TRACE("중단!");
	}

	DV_TRACE("종료");

	return 0;
}