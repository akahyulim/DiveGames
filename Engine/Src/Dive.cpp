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

	void Update()
	{
		DV_ASSERT(g_pEngine);
		g_pEngine->Update();
	}

	// 현재 Editor와 Engine이 번갈아가면서 그리기 때문에
	// 화면이 깜빡거린다.
	// 일단은 Update, Render부터 설정을 잡은 후 Renderer의 path로 내려가야할 것 같다.
	void Render()
	{
		DV_ASSERT(g_pEngine);
		//g_pEngine->Render();
	}

	Renderer& GetRenderer()
	{
		DV_ASSERT(g_pEngine != nullptr && "Engine 객체가 존재하지 않습니다.");
		return g_pEngine->GetRenderer();
	}

}