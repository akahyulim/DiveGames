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
		// 이건 Menubar처럼 항상 그려지는 부분이다.
		// 이외에도 Progressbar, Toolbar 등이 구현 대상이다.
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
			// 여기에서 크기를 제한시켜줘야하는데...
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

			// Style을 추가한 내용을 푸쉬하는 부분이다.
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
			// 이게 실제로 보여지는 부분이다.
			TickVisible();

			{
				ImGui::End();

				ImGui::PopStyleVar(mVarPushes);
				mVarPushes = 0;
			}
		}
	}
}