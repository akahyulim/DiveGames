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

	// ���� Editor�� Engine�� �����ư��鼭 �׸��� ������
	// ȭ���� �����Ÿ���.
	// �ϴ��� Update, Render���� ������ ���� �� Renderer�� path�� ���������� �� ����.
	void Render()
	{
		DV_ASSERT(g_pEngine);
		//g_pEngine->Render();
	}

	Renderer& GetRenderer()
	{
		DV_ASSERT(g_pEngine != nullptr && "Engine ��ü�� �������� �ʽ��ϴ�.");
		return g_pEngine->GetRenderer();
	}

}