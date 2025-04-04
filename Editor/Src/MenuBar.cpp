#include "MenuBar.h"
#include "Editor.h"

namespace Dive
{
	Editor* MenuBar::s_Editor = nullptr;
	Camera* MenuBar::s_EditorCamera = nullptr;

	void MenuBar::Initialize(Editor* editor)
	{
		s_Editor = editor;
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

				}
				if (ImGui::MenuItem("Open World"))
				{
					auto activeWorld = WorldManager::LoadFromFile("NewProject/Assets/Worlds/NewWorld.dive");
					isShowWorldMenu = true;

					// 현재 EditorCamera가 world 파일에 저장되어 있다.
					auto editorCameraGameObject = activeWorld->CreateGameObject("Editor Camera");
					editorCameraGameObject->SetTag("Editor_Only");
					s_EditorCamera = editorCameraGameObject->AddComponent<Camera>();
					s_EditorCamera->SetBackgroundColor(0.7f, 0.2f, 0.9f, 1.0f);
					s_EditorCamera->SetRenderTexture(std::make_shared<RenderTexture>(1024, 760));
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldManager::SaveToFile();
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

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tool"))
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
