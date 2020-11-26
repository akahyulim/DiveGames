#pragma once
#include "Dive.h"
#include "../Editor.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_internal.h"


class Widget
{
public:
	Widget(Editor* editor) : m_Editor(editor) 
	{
		m_SystemManager = m_Editor->GetSystemManager();
	}
	virtual ~Widget() = default;

	bool Begin()
	{
		if (!m_bWindow || !m_bVisible)
			return false;

		m_VarCount = 0;

		if (m_position.x != -1.0f && m_position.y != -1.0f)
		{
			ImGui::SetNextWindowPos(ImVec2(m_position.x, m_position.y));
		}

		if (m_padding.x != -1.0f && m_padding.y != -1.0f)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_padding.x, m_padding.y));
			m_VarCount++;
		}

		if (m_Alpha != -1.0f)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_Alpha);
			m_VarCount++;
		}

		if (m_size.x != -1.0f && m_size.y != -1.0f)
		{
			ImGui::SetNextWindowSize(ImVec2(m_size.x, m_size.y), ImGuiCond_FirstUseEver);
		}

		if ((m_size.x != -1.0f && m_size.y != -1.0f) || (m_sizeMax.x != FLT_MAX && m_sizeMax.y != FLT_MAX))
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(m_size.x, m_size.y), ImVec2(m_sizeMax.x, m_sizeMax.y));
		}

		ImGui::Begin(m_title.c_str(), &m_bVisible, m_WndFlags);
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
	const std::string& GetTitle()	{ return m_title; }

protected:
	Editor* m_Editor						= nullptr;
	//Dive::SystemManager* m_SystemManager	= nullptr;
	std::shared_ptr<Dive::SystemManager> m_SystemManager;

	ImGuiWindow* m_Window	= nullptr;
	std::string m_title		= "no name";
	bool m_bVisible			= true;
	bool m_bWindow			= true;
	int m_WndFlags			= ImGuiWindowFlags_NoCollapse;
	float m_Height			= 0;
	float m_Alpha			= -1.0f;

	DirectX::XMFLOAT2 m_position{ -1.0f, -1.0f };
	DirectX::XMFLOAT2 m_size{ -1.0f, -1.0f };
	DirectX::XMFLOAT2 m_sizeMax{ FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT2 m_padding{ -1.0f, -1.0f };

private:
	bool m_bWindowBegun		= false;
	unsigned int m_VarCount = 0;
};
