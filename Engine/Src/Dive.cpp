#include "divepch.h"
#include "Dive.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	Engine* g_pEngine = nullptr;


	Engine* CreateEngine()
	{
		Engine* pEngine = new Engine;
		DV_ASSERT(pEngine);
		SetCurrentEngine(pEngine);

		pEngine->Initialize();

		DV_CORE_TRACE("Engine was created");
		
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

		DV_CORE_TRACE("Engine was destroyed");
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
	}

	void Render()
	{
	}

}