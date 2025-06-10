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

					editorContext.ActiveWorld = Engine::CreateWorld();
					editorContext.ActiveWorld->GetOrAddSystem<ParentSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<LocalToWorldSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<RenderMeshSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<EntitiesGraphicsSystem>();

					auto& entityManager = editorContext.ActiveWorld->GetEntityManager();

					editorContext.EditorCamera = entityManager.CreateEntityWithComponents<ActiveComponent, LocalTransform, TagComponent, CameraComponent>();
					entityManager.GetComponent<NameComponent>(editorContext.EditorCamera).Name = "EditorCamera";
					entityManager.GetComponent<TagComponent>(editorContext.EditorCamera).Tag = TagComponent::eTag::EditorOnly;
					entityManager.GetComponent<CameraComponent>(editorContext.EditorCamera).ClearColor = { 0.5f, 0.5f, 1.0f, 1.0f };

					editorContext.MainCamera= entityManager.CreateEntityWithComponents<ActiveComponent, LocalTransform, TagComponent, CameraComponent>();
					entityManager.GetComponent<NameComponent>(editorContext.MainCamera).Name = "Camera";
					entityManager.GetComponent<TagComponent>(editorContext.MainCamera).Tag = TagComponent::eTag::MainCamera;
					entityManager.GetComponent<CameraComponent>(editorContext.MainCamera).ClearColor = { 1.0f, 0.5f, 0.5f, 1.0f };

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					// 역시 기존 월드를 해제해야 한다.

					editorContext.ActiveWorld = Engine::CreateWorld();
					editorContext.ActiveWorld->GetOrAddSystem<ParentSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<LocalToWorldSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<RenderMeshSystem>();
					editorContext.ActiveWorld->GetOrAddSystem<EntitiesGraphicsSystem>();

					WorldSerializer serializer(editorContext.ActiveWorld);
					serializer.Deserialize("NewWorld.dive");
					
					auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
					auto view = entityManager.GetEntityView<TagComponent, CameraComponent>();
					for (auto [entity, tagCom, CameraCom] : view.each())
					{
						if (tagCom.Tag == TagComponent::eTag::EditorOnly)
						{
							editorContext.EditorCamera = entity;
						}
						else if (tagCom.Tag == TagComponent::eTag::MainCamera)
						{
							editorContext.MainCamera = entity;
						}
					}

					// 임시
					ResourceManager::SetResourceFolder("Assets");

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldSerializer serializer(editorContext.ActiveWorld);
					serializer.Serialize("NewWorld.dive");
				}

				if (ImGui::MenuItem("Save World As...", nullptr, nullptr, isShowWorldMenu))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					if (editorContext.ActiveWorld)
					{
						editorContext.ActiveWorld->Clear();
					}
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
					auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
					auto empty = entityManager.CreateEntityWithComponents<ActiveComponent, LocalTransform>();
					entityManager.GetComponent<NameComponent>(empty).Name = "Empty";
				}
				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Triangle", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Triangle);
					}
					if (ImGui::MenuItem("Quad", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Quad);
					}
					
					if (ImGui::MenuItem("Plane", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Plane);
					}
					if (ImGui::MenuItem("Cube", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Cube);
					}
					if (ImGui::MenuItem("Sphere", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Sphere);
					}
					if (ImGui::MenuItem("Capsule", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						entityManager.CreatePrimitive(ePrimitiveType::Capsule);
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
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						editorContext.MainCamera = entityManager.CreateEntityWithComponents<ActiveComponent, LocalTransform, TagComponent, CameraComponent>();
						entityManager.GetComponent<NameComponent>(editorContext.MainCamera).Name = "Camera";
						entityManager.GetComponent<CameraComponent>(editorContext.MainCamera).Type = CameraComponent::eProjectionType::Perspective;
						entityManager.GetComponent<CameraComponent>(editorContext.MainCamera).ClearColor = { 1.0f, 0.5f, 0.5f, 1.0f };
					}

					if (ImGui::MenuItem("Orthographic", nullptr, nullptr, isShowWorldMenu))
					{
						auto& entityManager = editorContext.ActiveWorld->GetEntityManager();
						editorContext.MainCamera = entityManager.CreateEntityWithComponents<ActiveComponent, LocalTransform, TagComponent, CameraComponent>();
						entityManager.GetComponent<NameComponent>(editorContext.MainCamera).Name = "Camera";
						entityManager.GetComponent<CameraComponent>(editorContext.MainCamera).Type = CameraComponent::eProjectionType::Orthographic;
						entityManager.GetComponent<CameraComponent>(editorContext.MainCamera).ClearColor = { 1.0f, 0.5f, 0.5f, 1.0f };

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
