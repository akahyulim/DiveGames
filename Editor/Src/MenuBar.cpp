#include "MenuBar.h"
#include "Editor.h"
#include "Tools/MeshFactory.h"
#include "Views/HierarchyView.h"

namespace Dive
{
	MenuBar::MenuBar(Editor* editor)
		: m_Editor(editor)
	{
	}

	void MenuBar::ComposeUI()
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
					if (EditorContext::ActiveWorld)
					{
						WorldManager::UnloadWorld();
						EditorContext::Clear();
					}

					// 이름을 먼저 입력받아야 한다.
					//=> 혹은 저장시 설정해도 된다.
					
					EditorContext::ActiveWorld = WorldManager::CreateWorld("NewWorld");
					EditorContext::EditorCamera = EditorContext::ActiveWorld->CreateGameObject("EditorCamera");
					EditorContext::EditorCamera->SetTag("EditorOnly");
					EditorContext::EditorCamera->AddComponent<Camera>();
					EditorContext::MainCamera = EditorContext::ActiveWorld->CreateGameObject("MainCamera");
					EditorContext::MainCamera->SetTag("MainCamera");
					EditorContext::MainCamera->AddComponent<Camera>();
			
					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Open World"))
				{
					// 폴더에서 파일 선택

					if (EditorContext::ActiveWorld)
					{
						WorldManager::UnloadWorld();
						EditorContext::Clear();
					}

					EditorContext::ActiveWorld = WorldManager::LoadWorld("Assets/Worlds/NewWorld.dive");
					for (auto camera : Camera::GetAllCameras())
					{
						if (camera->GetGameObject()->GetTag() == "EditorOnly")
							EditorContext::EditorCamera = camera->GetGameObject();
						else if (camera->GetGameObject()->GetTag() == "MainCamera")
							EditorContext::MainCamera = camera->GetGameObject();
					}

					m_Editor->SetTitle(StringUtils::StringToWString(EditorContext::ActiveWorld->GetName()));

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldSerializer serializer(EditorContext::ActiveWorld);
					serializer.Serialize("Assets/Worlds/NewWorld.dive");
				}

				if (ImGui::MenuItem("Save World As...", nullptr, nullptr, isShowWorldMenu))
				{

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					if (EditorContext::ActiveWorld)
					{
						EditorContext::Selected = nullptr;
						EditorContext::EditorCamera = nullptr;
						EditorContext::MainCamera = nullptr;
						EditorContext::ActiveWorld = nullptr;
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
					auto emptyGO = EditorContext::ActiveWorld->CreateGameObject("Empty");
				}
				if (ImGui::BeginMenu("3D Object"))
				{
					if (ImGui::MenuItem("Triangle", nullptr, nullptr, isShowWorldMenu))
					{
						auto triangleGO = EditorContext::ActiveWorld->CreateGameObject("Triangle");
						auto staticMeshRender = triangleGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateTriangle());
						staticMeshRender->SetMaterial(std::make_shared<Material>()); 	// 현재 ResourceManager가 관리하지 않는다.
					}
					if (ImGui::MenuItem("Quad", nullptr, nullptr, isShowWorldMenu))
					{
						auto quadGO = EditorContext::ActiveWorld->CreateGameObject("Quad");
						auto staticMeshRender = quadGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateQuad());
						staticMeshRender->SetMaterial(std::make_shared<Material>());
					}
					
					if (ImGui::MenuItem("Plane", nullptr, nullptr, isShowWorldMenu))
					{
						auto planeGO = EditorContext::ActiveWorld->CreateGameObject("Plane");
						auto staticMeshRender = planeGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreatePlane());
						staticMeshRender->SetMaterial(std::make_shared<Material>());
					}
					if (ImGui::MenuItem("Cube", nullptr, nullptr, isShowWorldMenu))
					{
						auto cubeGO = EditorContext::ActiveWorld->CreateGameObject("Cube");
						auto staticMeshRender = cubeGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateCube());
						staticMeshRender->SetMaterial(std::make_shared<Material>());
					}
					if (ImGui::MenuItem("Sphere", nullptr, nullptr, isShowWorldMenu))
					{
						auto shpereGO = EditorContext::ActiveWorld->CreateGameObject("Sphere");
						auto staticMeshRender = shpereGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateSphere());
						staticMeshRender->SetMaterial(std::make_shared<Material>());
					}
					if (ImGui::MenuItem("Capsule", nullptr, nullptr, isShowWorldMenu))
					{
						auto capsuleGO = EditorContext::ActiveWorld->CreateGameObject("Capsule");
						auto staticMeshRender = capsuleGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateCapsule());
						staticMeshRender->SetMaterial(std::make_shared<Material>());
					}

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Directional Light", nullptr, nullptr, isShowWorldMenu))
					{
						auto dirLightGO = EditorContext::ActiveWorld->CreateGameObject("Directional Light");
						auto light = dirLightGO->AddComponent<Light>();
						light->SetLightType(eLightType::Directional);
					}
					if (ImGui::MenuItem("Point Light", nullptr, nullptr, isShowWorldMenu))
					{
						auto pointLightGO = EditorContext::ActiveWorld->CreateGameObject("Point Light");
						auto light = pointLightGO->AddComponent<Light>();
						light->SetLightType(eLightType::Point);
					}
					if (ImGui::MenuItem("Spot Light", nullptr, nullptr, isShowWorldMenu))
					{
						auto spotLightGO = EditorContext::ActiveWorld->CreateGameObject("Spot Light");
						auto light = spotLightGO->AddComponent<Light>();
						light->SetLightType(eLightType::Spot);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Camera"))
				{
					if (ImGui::MenuItem("Perpective", nullptr, nullptr, isShowWorldMenu))
					{
						auto cameraGO = EditorContext::ActiveWorld->CreateGameObject("Camera");
						auto cameraComponent = cameraGO->AddComponent<Camera>();
						cameraComponent->SetProjectionType(eProjectionType::Perspective);
					}

					if (ImGui::MenuItem("Orthographic", nullptr, nullptr, isShowWorldMenu))
					{
						auto cameraGO = EditorContext::ActiveWorld->CreateGameObject("Camera");
						auto cameraComponent = cameraGO->AddComponent<Camera>();
						cameraComponent->SetProjectionType(eProjectionType::Orthographic);
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Import"))
				{
			
				}

				if (ImGui::MenuItem("Terrain"))
				{
				}

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
