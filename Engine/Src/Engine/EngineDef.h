#pragma once
#include <string>
#include <Windows.h>

namespace Dive
{
	struct EngineParameters
	{
		EngineParameters()
		{
			Title = L"no named";
			Width = 800;
			Height = 600;
			PositionX = 0;
			PositionY = 0;
			bFullscreen = false;
			bReSizable = true;
			bBorderless = false;

			bVSync = false;
			bTripleBuffer = false;
			MultiSample = 1;
			RefreshRate = 0;
		}

		int32_t GetCenterPositionX() const
		{
			return (::GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
		}

		int32_t GetCenterPositionY() const
		{
			return (::GetSystemMetrics(SM_CYSCREEN) - Height) / 2;
		}

		// window
		std::wstring Title;
		int32_t Width;
		int32_t Height;
		int32_t PositionX;
		int32_t PositionY;
		bool bFullscreen;
		bool bReSizable;
		bool bBorderless;

		// graphics
		bool bVSync;
		bool bTripleBuffer;
		int32_t MultiSample;
		int32_t RefreshRate;
		// ¸ð´ÏÅÍ
	};
}