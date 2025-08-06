#include "MenuBar.h"
#include "Editor.h"
#include "Views/HierarchyView.h"

namespace Dive
{
	MenuBar::MenuBar(Editor* editor)
		: m_editor(editor)
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
					// 이름을 먼저 입력받아야 한다.
					
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
					// 추후 LoadWorld로 생성해야 한다.
					
					// 임시 : 현재 작업디렉토리 때문에 직렬화, 역직렬화 대상 경로가 나뉘어져 버렸다.
					// 임시로 역직렬화할 경우 경로에 Asset를 명시적으로 추가했다.
					// 이때문에 imgui.ini의 로딩과 저장 시점이 달라진다.
					EditorContext::ActiveWorld = WorldManager::CreateWorld("NewWorld");
					WorldSerializer serializer(EditorContext::ActiveWorld);
					serializer.Deserialize(Graphics::GetDevice(), "NewWorld.dive");

					auto allCameras = Camera::GetAllCameras();
					for (auto camera : allCameras)
					{
						if (camera->GetGameObject()->GetTag() == "EditorOnly")
							EditorContext::EditorCamera = camera->GetGameObject();
						else if (camera->GetGameObject()->GetTag() == "MainCamera")
							EditorContext::MainCamera = camera->GetGameObject();
					}

					isShowWorldMenu = true;
				}
				if (ImGui::MenuItem("Save World", nullptr, nullptr, isShowWorldMenu))
				{
					WorldSerializer serializer(EditorContext::ActiveWorld);
					serializer.Serialize("NewWorld.dive");
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
						staticMeshRender->SetStaticMesh(MeshFactory::CreateTriangle(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice())); 	// 현재 ResourceManager가 관리하지 않는다.
					}
					if (ImGui::MenuItem("Quad", nullptr, nullptr, isShowWorldMenu))
					{
						auto quadGO = EditorContext::ActiveWorld->CreateGameObject("Quad");
						auto staticMeshRender = quadGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateQuad(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice()));
					}
					
					if (ImGui::MenuItem("Plane", nullptr, nullptr, isShowWorldMenu))
					{
						auto planeGO = EditorContext::ActiveWorld->CreateGameObject("Plane");
						auto staticMeshRender = planeGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreatePlane(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice()));
					}
					if (ImGui::MenuItem("Cube", nullptr, nullptr, isShowWorldMenu))
					{
						auto cubeGO = EditorContext::ActiveWorld->CreateGameObject("Cube");
						auto staticMeshRender = cubeGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateCube(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice()));
					}
					if (ImGui::MenuItem("Sphere", nullptr, nullptr, isShowWorldMenu))
					{
						auto shpereGO = EditorContext::ActiveWorld->CreateGameObject("Sphere");
						auto staticMeshRender = shpereGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateSphere(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice()));
					}
					if (ImGui::MenuItem("Capsule", nullptr, nullptr, isShowWorldMenu))
					{
						auto capsuleGO = EditorContext::ActiveWorld->CreateGameObject("Capsule");
						auto staticMeshRender = capsuleGO->AddComponent<MeshRenderer>();
						staticMeshRender->SetStaticMesh(MeshFactory::CreateCapsule(Graphics::GetDevice()));
						staticMeshRender->SetMaterial(std::make_shared<Material>(Graphics::GetDevice()));
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

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
