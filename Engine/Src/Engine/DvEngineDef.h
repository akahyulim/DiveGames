#pragma once
#include <string>

namespace Dive
{
	struct EngineParameters
	{
		EngineParameters()
		{
			title = L"no named";
			width = 800;
			height = 600;
			positionX = 0;
			positionY = 0;
			bFullscreen = false;
			bReSizable = true;
			bBorderless = false;

			bVSync = false;
			bTripleBuffer = false;
			multiSample = 1;
			refreshRate = 0;
		}

		int GetCenterPositionX() const
		{
			return (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		}

		int GetCenterPositionY() const
		{
			return (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		}

		// window
		std::wstring title;
		int width;
		int height;
		int positionX;
		int positionY;
		bool bFullscreen;
		bool bReSizable;
		bool bBorderless;

		// graphics
		bool bVSync;
		bool bTripleBuffer;
		int multiSample;
		int refreshRate;
		// ¸ð´ÏÅÍ
	};
}