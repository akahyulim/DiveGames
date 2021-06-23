#include "Widget.h"
#include "Editor.h"

namespace editor
{
	Widget::Widget(Editor* editor)
	{
		assert(editor);

		mEditor = editor;
	}

	void Widget::Tick()
	{
		// �̰� Menubaró�� �׻� �׷����� �κ��̴�.
		// �̿ܿ��� Progressbar, Toolbar ���� ���� ����̴�.
		TickAlways();

		if (!mbWindow)	
			return;

		bool bBegun = false;
		{
			if (!mbVisible)
				return;

			if (mPosition.x != -1.0f && mPosition.y != -1.0f)
			{
				ImGui::SetNextWindowPos(mPosition);
			}
			if (mSize.x != -1.0f && mSize.y != -1.0f)
			{
				ImGui::SetNextWindowSize(mSize, ImGuiCond_FirstUseEver);
			}
			// ���⿡�� ũ�⸦ ���ѽ�������ϴµ�...
			if ((mSize.x != -1.0f && mSize.y != -1.0f) || (mMaxSize.x != FLT_MAX && mMaxSize.y != FLT_MAX))
			{
				ImGui::SetNextWindowSizeConstraints(mSize, mMaxSize);
			}
			if (mAlpha != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, mAlpha);
				mVarPushes++;
			}
			if (mPadding.x != -1.0f && mPadding.y != -1.0f)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, mPadding);
				mVarPushes++;
			}

			// Style�� �߰��� ������ Ǫ���ϴ� �κ��̴�.
			OnPushStyleVar();

			if (ImGui::Begin(mTitle.c_str(), &mbVisible, mFlags))
			{
				mWindow = ImGui::GetCurrentWindow();
				mHeight = ImGui::GetWindowHeight();

				bBegun = true;
			}
			else if (mWindow && mWindow->Hidden)
			{
				bBegun = true;
			}

			if (mWindow && mWindow->Appearing)
			{
				OnShow();
			}
			else if (!mbVisible)
			{
				OnHide();
			}
		}

		if (bBegun)
		{
			// �̰� ������ �������� �κ��̴�.
			TickVisible();

			{
				ImGui::End();

				ImGui::PopStyleVar(mVarPushes);
				mVarPushes = 0;
			}
		}
	}
}