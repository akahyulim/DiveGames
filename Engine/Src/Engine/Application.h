#pragma once

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		// 생성자. 커맨드라인으로부터 엔진 파라미터를 파싱 한 후 엔진을 초기화되지 않은 상태로 생성.
		Application();
		virtual ~Application() = default;

		// 엔진 초기화 이전에 호출. 이 곳에서 엔진 파라미터를 수정 가능.
		virtual void Setup() {}
		
		// 엔진 초기화 후, 메인 루프 실행 전 설정.
		virtual void Start() {}
		
		// 메인 루프 후 클린업, 어플리케이션이 호출.
		virtual void Stop() {}

		// 엔진 초기화 후 메인 루프 실행.
		int Run();
		// 에러 메시지를 출력. 메인 루프를 종료하고, 실패 종료 코드를 설정.
		void ErrorExit(const std::string& message = std::string());

	protected:
		// Dive 엔진
		std::shared_ptr<Engine> m_pEngine;
		// 엔진 파라미터 맵
		// 어플리케이션 종료 코드
		int m_ExitCode;
	};
}