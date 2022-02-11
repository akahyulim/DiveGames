#pragma once
#include "Base/Time.h"

namespace Dive
{
	// ���� ����
	class Engine;
	
	Engine* CreateEngine();
	void DestroyEngine(Engine* pEngine = nullptr);
	Engine* GetCurrentEngine();
	void SetCurrentEngine(Engine* pEngine);


	// WINDOW DATA�� �޾� Renderer�� �ʱ�ȭ�ϴ� �Լ��� �ʿ��ϴ�.

	void Update();
	void Render();

	// get

}