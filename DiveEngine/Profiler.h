#pragma once

namespace Dive
{
	// 결국 무엇을 측정할 것인가를 정한 후 어디에서 측정할 것인가를 파악해야 한다.
	// 결과값은 수치화될 것이고, 이들을 이용해 그래프 혹은 문자열 형식으로 출력 및 저장할 수 있어야 한다.

	// cpu, gpu, frame, fps 계산
	// 각각의 평균, 최저, 최대 값 
	// 유니티의 경우 메모리 사용량도 보여준다.
	// 그런데 메모리도 cpu와 gpu가 다를텐데... 그냥 보여주는 방법의 차이인가?
	// 주요한 사항부터 적용한 후 차차 추가해가는 방향이 맞는 것 같다.

	// wicked
	// frame, rangeCPU, rangeGPU로 나누어 수행한다.
	// begin ~ end frame은 run time의 run의 거의 시작과 끝이다. 내부에서 cpu, gpu가 개별 측정된다.
	// cpu, gpu는 runtime뿐만 아니라 개별 객체에서도 측정된다.
	class Profiler
	{
	public:
		static Profiler& GetInstance()
		{
			static Profiler instance;
			return instance;
		}

		void SetEnabled(bool use);
		bool IsEnabled() const { return m_bEnabled; }

	private:
		Profiler() = default;
		~Profiler();

	private:
		bool m_bEnabled = false;
	};
}