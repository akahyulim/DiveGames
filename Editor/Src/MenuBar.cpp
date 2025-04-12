#include "MenuBar.h"
#include "Editor.h"
#include "Views/HierarchyView.h"

namespace Dive
{
	MenuBar::MenuBar(Editor* editor)
		: m_Editor(editor)
	{
	}

	void MenuBar::Draw()
	{
		static bool isShowWorldMenu = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create Project"))
				{

				}
				if (ImGui::MenuItem("Open Project"))
				{

				}
				if (ImGui::MenuItem("Save Project"))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("New World"))
				{
					m_ActiveWorld = std::make_shared<World>();
					m_Editor->GetWidget<HierarchyView>()->SetWorld(m_ActiveWorld);
					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
				}

				if (ImGui::MenuItem("Save World As...", nullptr, nullptr, isShowWorldMenu))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Window::Close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::MenuItem("Create Empty", nullptr, nullptr, isShowWorldMenu))
				{
					m_ActiveWorld->CreateGameObject("GameObject");
				}
				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Cube", nullptr, nullptr, isShowWorldMenu))
					{

					}
					if (ImGui::MenuItem("Sphere", nullptr, nullptr, isShowWorldMenu))
					{

					}
					if (ImGui::MenuItem("Plane", nullptr, nullptr, isShowWorldMenu))
					{

					}
					if (ImGui::MenuItem("Quad", nullptr, nullptr, isShowWorldMenu))
					{

					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional Light", nullptr, nullptr, isShowWorldMenu))
					{

					}
					if (ImGui::MenuItem("Point Light", nullptr, nullptr, isShowWorldMenu))
					{

					}
					if (ImGui::MenuItem("Spot Light", nullptr, nullptr, isShowWorldMenu))
					{

					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Camera"))
				{
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
