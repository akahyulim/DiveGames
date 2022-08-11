#pragma once
#include "Core/Object.h"
#include "Core/Variant.h"
#include "Core/Timer.h"
#include "EngineDef.h"

namespace Dive
{
	class Context;
	class Event;

	class Engine : public Object
	{
		DIVE_OBJECT(Engine, Object)

	public:
		// ������. �⺻ ���� �ý��� ���� �� �ʱ�ȭ.
		explicit Engine(Context* pContext);
		// �Ҹ���. ��� ���� �ý��� ����.
		~Engine() override;

		// ���޹��� �Ķ���ͷ� ���� �ʱ�ȭ �� ������ ���.
		bool Initialize(const EngineParameters& parameters);

		// ������.
		void RunFrame();
		// ������ ������Ʈ �̺�Ʈ �߻�.
		void Update();
		// ������ ������Ʈ ���� ������.
		void Render();

		// ���� �ʱ�ȭ �÷��� ����.
		bool IsInitialized() const { return m_bInitialized; }
		// ���� �÷��� ����
		bool IsExiting() const { return m_bExiting; }

		// �׷��Ƚ� ������ ���� �� ���� ����.
		void Exit();

		// ���� ��û �̺�Ʈ ó��.
		void OnExitRequested(const Event& e);

		unsigned int GetMaxFPS() const { return m_MaxFps; }
		void SetMaxFPS(unsigned int targetFps) { m_MaxFps = targetFps; }

	private:
		// ������ �ð� ���.
		void updateFrameTimer();
		// ���� ���� ����.
		void doExit();

	private:
		// �ʱ�ȭ �÷���.
		bool m_bInitialized;
		// ���� �÷���.
		bool m_bExiting;

		// ������ ���� ���� Ÿ�̸�.
		Timer m_FrameTimer;

		//std::vector<float> m_LastTmeSteps;

		// ������ ��� �ð�.
		float m_DeltaTime;

		// �ִ� Ÿ�� ������.
		unsigned int m_MaxFps;
	};
}

