#pragma once

namespace Dive
{
	// Application�� �����ϴµ� �̰� ���� �ʿ��ֳ� �ʹ�.
	class Engine
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Tick();

		static void Exit();

		static bool IsExiting();
	};
}