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
					auto world = Engine::CreateWorld();
					world->GetOrAddSystem<ParentSystem>();
					world->GetOrAddSystem<LocalToWorldSystem>();

					// 추후 에디터 카메라 추가

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					auto world = Engine::CreateWorld();
					world->GetOrAddSystem<ParentSystem>();
					world->GetOrAddSystem<LocalToWorldSystem>();

					WorldSerializer serializer(world);
					serializer.Deserialize("NewWorld.dive");
					
					// 역시 에디터 카메라 추가?

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldSerializer serializer(Engine::GetWorld());
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
					auto& entityManager = Engine::GetWorld()->GetEntityManager();
					auto empty = entityManager.CreateEntity("Entity");
					entityManager.AddComponent<ActiveComponent>(empty).IsActive = true;
					entityManager.AddComponent<LocalTransform>(empty);

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
