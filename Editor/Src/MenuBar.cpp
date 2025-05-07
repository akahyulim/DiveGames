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
					// world
					auto world = Engine::GenerateWorld();

					// editor camera
					auto editorCamera = world->CreateEntity("EditorCamera");
					auto& cc = editorCamera.AddComponent<CameraComponent>();
					cc.RenderTarget = new RenderTexture(1, 1);
					editorCamera.GetComponent<TagComponent>().Tag = eTag::EditorOnly;

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					// world
					auto world = Engine::GenerateWorld();
					WorldSerializer serializer(world);
					serializer.Deserialize("NewWorld.dive");

					// editor camera
					auto editorCamera = world->CreateEntity("EditorCamera");
					editorCamera.AddComponent<CameraComponent>();
					editorCamera.GetComponent<TagComponent>().Tag = eTag::EditorOnly;

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldSerializer serializer(Engine::GetActiveWorld());
					serializer.Serialize("NewWorld.dive");
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

			if (ImGui::BeginMenu("Entity"))
			{
				if (ImGui::MenuItem("Create Empty", nullptr, nullptr, isShowWorldMenu))
				{
					Engine::GetActiveWorld()->CreateEntity("Entity");
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
