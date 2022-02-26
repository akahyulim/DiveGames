#pragma once
#include "Base/Time.h"
#include "Base/Engine.h"
#include "Renderer/Renderer.h"


// 굳이 지울 필요는 없는 것 같다.
namespace Dive
{
	// 전방 선언
	
	Engine* CreateEngine(const WindowData* pData);
	void DestroyEngine(Engine* pEngine = nullptr);
	Engine* GetCurrentEngine();
	void SetCurrentEngine(Engine* pEngine);

	// WINDOW DATA를 받아 Renderer를 초기화하는 함수가 필요하다.

	void Run();

}