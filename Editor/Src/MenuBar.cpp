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
		auto& editorContext = EditorContext::GetInstance();
		
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
					// 기존 월드를 해제해야 한다.

					DvEditorContext::ActiveWorld = Engine::CreateWorld();

					DvEditorContext::EditorCamera = DvEditorContext::ActiveWorld->CreateGameObject("EditorCamera");
					DvEditorContext::MainCamera = DvEditorContext::ActiveWorld->CreateGameObject("MainCamera");

					// 임시
					ResourceManager::SetResourceFolder("Assets");
			
					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					// 역시 기존 월드를 해제해야 한다.


					DvEditorContext::ActiveWorld = Engine::CreateWorld();
					DvWorldSerializer serializer(DvEditorContext::ActiveWorld);
					serializer.Deserialize("Assets/NewWorld.dive");

					// 임시 : 현재 작업디렉토리 때문에 직렬화, 역직렬화 대상 경로가 나뉘어져 버렸다.
					// 임시로 역직렬화할 경우 경로에 Asset를 명시적으로 추가했다.
					ResourceManager::SetResourceFolder("Assets");

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					DvWorldSerializer serializer(DvEditorContext::ActiveWorld);
					serializer.Serialize("NewWorld.dive");
				}

				if (ImGui::MenuItem("Save World As...", nullptr, nullptr, isShowWorldMenu))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					if (DvEditorContext::ActiveWorld)
					{
						DvEditorContext::ActiveWorld->Clear();
					}
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
					DvEditorContext::ActiveWorld->CreateGameObject("Empty");
				}
				if (ImGui::BeginMenu("3D TypeInfo"))
				{
					if (ImGui::MenuItem("Triangle", nullptr, nullptr, isShowWorldMenu))
					{
						//auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						//entityManager.CreatePrimitive(ePrimitiveType::Triangle);
					}
					if (ImGui::MenuItem("Quad", nullptr, nullptr, isShowWorldMenu))
					{
					}
					
					if (ImGui::MenuItem("Plane", nullptr, nullptr, isShowWorldMenu))
					{
					}
					if (ImGui::MenuItem("Cube", nullptr, nullptr, isShowWorldMenu))
					{
					}
					if (ImGui::MenuItem("Sphere", nullptr, nullptr, isShowWorldMenu))
					{
					}
					if (ImGui::MenuItem("Capsule", nullptr, nullptr, isShowWorldMenu))
					{
					}

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional Light", nullptr, nullptr, isShowWorldMenu))
					{
						auto current_path = std::filesystem::current_path();
						DV_LOG(MenuBar, debug, "Current Path: {}", current_path.string());
					}
					if (ImGui::MenuItem("Point Light", nullptr, nullptr, isShowWorldMenu))
					{
						ResourceManager::Load<Texture2D>("Textures/dmc.jpg");
						ResourceManager::Load<Texture2D>("Textures/DokeV.jpeg");
						ResourceManager::Load<Texture2D>("Textures/relaxed_morning.jpg");
						ResourceManager::Load<Texture2D>("Textures/sky_daytime_blue.jpg");
					}
					if (ImGui::MenuItem("Spot Light", nullptr, nullptr, isShowWorldMenu))
					{

					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Camera"))
				{
					if (ImGui::MenuItem("Perpective", nullptr, nullptr, isShowWorldMenu))
					{
					}

					if (ImGui::MenuItem("Orthographic", nullptr, nullptr, isShowWorldMenu))
					{
					}
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
