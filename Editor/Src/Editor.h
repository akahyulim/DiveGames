#pragma once
#include "Dive.h"

using namespace Dive;

class Widget;

// ImGUI의 Render는 Event로 호출했다.
// -> 엔진 외부 Render 호출은 문제를 발생시킬 수 있다.
// ImGUI의 MessageHandler를 호출할 수 없다.
// -> Engine의 Window 혹은 Graphics에서 Callable을 받는 모양새도 이상하다.
// Engine이 아니라 직접 Subsystem을 구성하는 편이 나을 것 같다.
// -> 현재 Graphics와 Window의 결합이 강하므로 좀 더 생각해보아야 한다.
// -> 외부 윈도우 생성 혹은 추가에 대해서도 생각해보자.

class Editor
{
public:
	Editor();
	~Editor();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool Initialize();
	void DoExit();

	void RunFrame();

	bool IsInitialized()	const { return m_bInitialized; }
	bool IsExiting()		const { return m_bExiting; }

	// 사실 애네 둘은 private이어야 한다.
	void Update();
	void Render();

private:
private:
	Context* m_context;
	bool m_bInitialized;
	bool m_bExiting;

	std::vector<std::shared_ptr<Widget>> m_widgets;
};