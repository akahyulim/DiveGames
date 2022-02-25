#include "divepch.h"
#include "Dive.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	Engine* g_pEngine = nullptr;

	Engine* CreateEngine(const WindowData* pData)
	{
		Engine* pEngine = new Engine;
		DV_ASSERT(pEngine);
		SetCurrentEngine(pEngine);

		pEngine->Initialize(pData);

		return pEngine;
	}

	void DestroyEngine(Engine* pEngine)
	{
		if (pEngine == nullptr)
			pEngine = g_pEngine;
		
		pEngine->Shutdown();

		if (g_pEngine == pEngine)
			SetCurrentEngine(nullptr);

		DV_DELETE(pEngine);
	}

	Engine* GetCurrentEngine()
	{
		return g_pEngine;
	}

	void SetCurrentEngine(Engine* pEngine)
	{
		g_pEngine = pEngine;
	}

	void BeginScene()
	{
		GetRenderer().BeginScene();
	}

	void EndScene()
	{
		GetRenderer().EndScene();
	}

	void Run()
	{
		DV_ASSERT(g_pEngine != nullptr && "Engine 객체가 존재하지 않습니다.");
		g_pEngine->Run();
	}

	Renderer& GetRenderer()
	{
		DV_ASSERT(g_pEngine != nullptr && "Engine 객체가 존재하지 않습니다.");
		return g_pEngine->GetRenderer();
	}

}