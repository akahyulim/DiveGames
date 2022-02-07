#include "divepch.h"
#include "Engine.h"
#include "DiveCore.h"
#include "Time.h"
#include "Log.h"

namespace Dive
{
	Engine* Engine::s_pInstance = nullptr;

	Engine::Engine()
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		// sub system ������ �� ���� �´�.
		Log::Initialize();

		// spartan�� ������ �ʱ�ȭ ���� �����ߴ�.
		// �ٸ� �ʱ�ȭ�� �ʿ��� �����ʹ� �Ű������� �ƴ�
		// Engine�� ���� ȣ���Ͽ� �����.

		CORE_TRACE("Create Engine");
	}

	Engine::~Engine()
	{
		// sub system ����
		CORE_TRACE("Destroy Engine");
	}

	void Engine::Tick()
	{
	}
}