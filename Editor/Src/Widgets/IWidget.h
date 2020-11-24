#pragma once
#include "Core/DiveEngine.h"
#include "../Editor.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_internal.h"


class IWidget
{
public:
	IWidget(Editor* editor) : m_Editor(editor) 
	{
		m_SystemManager = m_Editor->GetSystemManager();
	}
	virtual ~IWidget() = default;

	bool Begin()
	{
		if (!m_bWindow || !m_bVisible)
			return false;

		m_VarCount = 0;

		if (m_Position.x != -1.0f && m_Position.y != -1.0f)
		{
			ImGui::SetNextWindowPos(ImVec2(m_Position.x, m_Position.y));
		}

		if (m_Padding.x != -1.0f && m_Padding.y != -1.0f)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Padding.x, m_Padding.y));
			m_VarCount++;
		}

		if (m_Alpha != -1.0f)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_Alpha);
			m_VarCount++;
		}

		if (m_Size.x != -1.0f && m_Size.y != -1.0f)
		{
			ImGui::SetNextWindowSize(ImVec2(m_Size.x, m_Size.y), ImGuiCond_FirstUseEver);
		}

		if ((m_Size.x != -1.0f && m_Size.y != -1.0f) || (m_SizeMax.x != FLT_MAX && m_SizeMax.y != FLT_MAX))
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(m_Size.x, m_Size.y), ImVec2(m_SizeMax.x, m_SizeMax.y));
		}

		ImGui::Begin(m_Title.c_str(), &m_bVisible, m_WndFlags);
		m_bWindowBegun = true;

		return true;
	}

	virtual void Tick(float deltaTime = 0.0f) = 0;

	bool End()
	{
		if (!m_bWindowBegun)
			return false;

		m_Window = ImGui::GetCurrentWindow();
		m_Height = ImGui::GetWindowHeight();
		ImGui::End();
		ImGui::PopStyleVar(m_VarCount);

		return true;
	}

	template<class T>
	void PushStyleVar(ImGuiStyleVar idx, T val)
	{
		ImGui::PushStyleVar(idx, val);
		m_varCount++;
	}

	bool IsWindow()					{ return m_bWindow; }
	bool& IsVisible()				{ return m_bVisible; }
	void SetVisible(bool visible)	{ m_bVisible = visible; }
	float GetHeight()				{ return m_Height; }
	ImGuiWindow* GetWindow()		{ return m_Window; }
	const std::string& GetTitle()	{ return m_Title; }

protected:
	Editor* m_Editor						= nullptr;
	Dive::SystemManager* m_SystemManager	= nullptr;

	ImGuiWindow* m_Window	= nullptr;
	std::string m_Title		= "no name";
	bool m_bVisible			= true;
	bool m_bWindow			= true;
	int m_WndFlags			= ImGuiWindowFlags_NoCollapse;
	float m_Height			= 0;
	float m_Alpha			= -1.0f;

	DirectX::XMFLOAT2 m_Position{ -1.0f, -1.0f };
	DirectX::XMFLOAT2 m_Size{ -1.0f, -1.0f };
	DirectX::XMFLOAT2 m_SizeMax{ FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT2 m_Padding{ -1.0f, -1.0f };

private:
	bool m_bWindowBegun		= false;
	unsigned int m_VarCount = 0;
};
