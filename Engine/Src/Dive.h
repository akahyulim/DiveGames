#pragma once
#include "Base/Time.h"
#include "Base/Engine.h"
#include "Renderer/Renderer.h"


// ���� ���� �ʿ�� ���� �� ����.
// => �̷��ٿ� Enigne�� static �Լ��� ����� ���� ���� �� ����.
namespace Dive
{
	// ���� ����
	
	Engine* CreateEngine(const WindowData* pData);
	void DestroyEngine(Engine* pEngine = nullptr);
	Engine* GetCurrentEngine();
	void SetCurrentEngine(Engine* pEngine);

	// WINDOW DATA�� �޾� Renderer�� �ʱ�ȭ�ϴ� �Լ��� �ʿ��ϴ�.

	void Run();

}