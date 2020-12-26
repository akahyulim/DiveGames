#pragma once
#include "Dive.h"

using namespace Dive;

class Widget;

// ImGUI�� Render�� Event�� ȣ���ߴ�.
// -> ���� �ܺ� Render ȣ���� ������ �߻���ų �� �ִ�.
// ImGUI�� MessageHandler�� ȣ���� �� ����.
// -> Engine�� Window Ȥ�� Graphics���� Callable�� �޴� ������ �̻��ϴ�.
// Engine�� �ƴ϶� ���� Subsystem�� �����ϴ� ���� ���� �� ����.
// -> ���� Graphics�� Window�� ������ ���ϹǷ� �� �� �����غ��ƾ� �Ѵ�.
// -> �ܺ� ������ ���� Ȥ�� �߰��� ���ؼ��� �����غ���.

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

	// ��� �ֳ� ���� private�̾�� �Ѵ�.
	void Update();
	void Render();

private:
private:
	Context* m_context;
	bool m_bInitialized;
	bool m_bExiting;

	std::vector<std::shared_ptr<Widget>> m_widgets;
};