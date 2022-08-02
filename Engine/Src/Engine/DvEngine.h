#pragma once
#include "Core/DvObject.h"
#include "Core/Variant.h"
#include "Core/DvTimer.h"
#include "DvEngineDef.h"

namespace Dive
{
	class DvContext;

	class DvEngine : public DvObject
	{
		DIVE_OBJECT(DvEngine, DvObject)

	public:
		// 생성자. 기본 서브 시스템 생성 및 초기화.
		explicit DvEngine(DvContext* pContext);
		// 소멸자. 모든 서브 시스템 제거.
		~DvEngine() override;

		// 전달받은 파라미터로 엔진 초기화 및 윈도우 출력.
		bool Initialize(const EngineParameters& parameters);

		// 프레임.
		void RunFrame();
		// 프레임 업데이트 이벤트 발생.
		void Update();
		// 프레임 업데이트 이후 렌더링.
		void Render();

		// 엔진 초기화 플래그 리턴.
		bool IsInitialized() const { return m_bInitialized; }
		// 종료 플래그 리턴
		bool IsExiting() const { return m_bExiting; }

		// 그래픽스 윈도우 종료 및 실행 종료.
		void Exit();

		// 종료 요청 이벤트 처리.
		void OnExitRequested();

		unsigned int GetMaxFPS() const { return m_MaxFps; }
		void SetMaxFPS(unsigned int targetFps) { m_MaxFps = targetFps; }

	private:
		// 프레임 시간 계산.
		void updateFrameTimer();
		// 실제 종료 수행.
		void doExit();

	private:
		// 초기화 플래그.
		bool m_bInitialized;
		// 종료 플래그.
		bool m_bExiting;

		// 프레임 간격 측정 타이머.
		DvTimer m_FrameTimer;

		//std::vector<float> m_LastTmeSteps;

		// 프레임 경과 시간.
		float m_DeltaTime;

		// 최대 타겟 프레임.
		unsigned int m_MaxFps;
	};
}

