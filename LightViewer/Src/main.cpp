#include "LightViewer.h"

int main()
{
	LightViewer app;
	if (app.Initialize())
	{
		app.Run();
	}

	app.Release();

	return 0;
}