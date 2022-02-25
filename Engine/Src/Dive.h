#pragma once
#include "Base/Time.h"
#include "Base/Engine.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	// ���� ����
	
	Engine* CreateEngine(const WindowData* pData);
	void DestroyEngine(Engine* pEngine = nullptr);
	Engine* GetCurrentEngine();
	void SetCurrentEngine(Engine* pEngine);

	void BeginScene();
	void EndScene();


	// WINDOW DATA�� �޾� Renderer�� �ʱ�ȭ�ϴ� �Լ��� �ʿ��ϴ�.

	void Run();

	// get
	Renderer& GetRenderer();

}