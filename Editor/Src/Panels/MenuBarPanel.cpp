#include "MenuBarPanel.h"

namespace Editor
{
	MenuBarPanel::MenuBarPanel(Editor* pEditor)
		: Panel(pEditor, "MenuBar")
	{
		m_bWindow = false;

		// CreateScene을 구독해야 한다.
	}

	MenuBarPanel::~MenuBarPanel()
	{
	}

	// 메뉴바는 항상 그려져야 한다.
	void MenuBarPanel::renderAlways()
	{
		ImGui::BeginMainMenuBar();

		menuFile();
		menuGameObject();
		menuComponent();
		menuTools();
		menuWindow();
		menuHelp();

		ImGui::EndMainMenuBar();
	}

	void MenuBarPanel::menuFile()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Project"))
				{

				}
				if (ImGui::MenuItem("Scene"))
				{
					if (s_pActiveScene)
					{
						// 저장 여부 확인
						DV_DELETE(s_pActiveScene);
					}

					s_pActiveScene = new Dive::Scene(m_pEditor->GetContext());

					DV_LOG_CLIENT_TRACE("새로운 Scene({:s})을 생성하였습니다.", s_pActiveScene->GetName());
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open"))
			{
				if (ImGui::MenuItem("Project"))
				{

				}
				if (ImGui::MenuItem("Scene"))
				{
					/*
					if (m_pActiveScene)
					{
						// 저장 여부 확인
					}

					m_pActiveScene = Dive::GetCurrentEngine()->CreateScene();

					// 역시 Scene 이름을 전달하자.
					Dive::ResourceManager::GetInstance().LoadFromDataFile();

					m_pActiveScene->Deserialize("Assets/Scenes/New_World.scene");
					*/
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close"))
			{
				if (s_pActiveScene)
				{
					// 저장 여부 확인

					// Engine에서 Scene을 Close해야 한다.


					auto sceneName = s_pActiveScene->GetName();
					DV_DELETE(s_pActiveScene);

					DV_LOG_CLIENT_TRACE("Scene({:s})을 닫았습니다.", sceneName);
				}

			}
			if (ImGui::MenuItem("Close Project"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{
				/*
				if (m_pActiveScene)
				{
					m_pActiveScene->Serialize("Assets/Scenes/New_World.scene");

					// scene 이름을 전달하자.
					Dive::ResourceManager::GetInstance().SaveToDataFile();
				}
				*/
			}
			if (ImGui::MenuItem("Save As..."))
			{

			}
			if (ImGui::MenuItem("Save Project"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				if (s_pActiveScene)
				{
					// 저장 여부 확인

					DV_DELETE(s_pActiveScene);
				}
				
				FIRE_EVENT(Dive::ExitRequestEvent());
			}

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuGameObject()
	{
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				if (s_pActiveScene)
				{
					auto* pNewGameObject = s_pActiveScene->CreateGameObject("GameObject");
					pNewGameObject->CreateComponent<Dive::Transform>();
					pNewGameObject->SetName("GameObject " + std::to_string(pNewGameObject->GetID()));

					DV_LOG_CLIENT_TRACE("빈 GameObject를 생성하였습니다.");
				}
			}

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					/*
					if (m_pActiveScene)
					{
						// 유니티에선 스크립트 차원에서 함수가 존재한다.
						// GameObject의 CreatePrimitive였나...

						// 가장 기본적인 형태는
						// GameObject를 생성하고
						// MeshRenderable을 추가한 후
						// ResourceManager를 통해 Model을 Set하는 것이다.

						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cube.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Sphere"))
				{
					/*
					if (m_pActiveScene)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Sphere.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Cylinder"))
				{
					/*
					if (m_pActiveScene)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cylinder.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Cone"))
				{
					/*
					if (m_pActiveScene)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cone.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Plane"))
				{
					/*
					if (m_pActiveScene)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Plane.obj");
					}
					*/
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					/*
					if (m_pActiveScene)
					{
						auto lightObj = m_pActiveScene->CreateGameObject("Directional_Light");
						lightObj->AddComponent<Dive::Transform>();
						auto lightCom = lightObj->AddComponent<Dive::Light>();
						lightCom->SetLightType(Dive::eLightType::Directional);
					}
					*/
				}

				if (ImGui::MenuItem("Point"))
				{
					/*
					if (m_pActiveScene)
					{
						auto lightObj = m_pActiveScene->CreateGameObject("Point_Light");
						lightObj->AddComponent<Dive::Transform>();
						auto lightCom = lightObj->AddComponent<Dive::Light>();
						lightCom->SetLightType(Dive::eLightType::Point);
					}
					*/
				}

				if (ImGui::MenuItem("Spot"))
				{
					/*
					if (m_pActiveScene)
					{
						auto lightObj = m_pActiveScene->CreateGameObject("Spot_Light");
						lightObj->AddComponent<Dive::Transform>();
						auto lightCom = lightObj->AddComponent<Dive::Light>();
						lightCom->SetLightType(Dive::eLightType::Spot);
					}
					*/
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Camera"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuComponent()
	{
		if (ImGui::BeginMenu("Component"))
		{
			if (ImGui::MenuItem("SpriteRenderable"))
			{
				/*
				if (m_pActiveScene)
				{
					auto pSelected = m_pEditor->GetHierarchy()->GetSeletecedObject();
					if (pSelected)
					{
						if (!pSelected->HasComponent<Dive::SpriteRenderable>())
						{
							pSelected->AddComponent<Dive::SpriteRenderable>();
						}
					}
				}
				*/
			}

			if (ImGui::MenuItem("Light"))
			{
				/*
				if (m_pActiveScene)
				{
					auto pSelected = m_pEditor->GetHierarchy()->GetSeletecedObject();
					if (pSelected)
					{
						if (!pSelected->HasComponent<Dive::Light>())
						{
							pSelected->AddComponent<Dive::Light>();
						}
					}
				}
				*/
			}

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuTools()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			// 새 창을 띄워 옵션을 선택토록 해야 한다.
			// 유니티의 경우엔 파일 드래그를 하면 Inspector에 설정창이 뜬다.
			if (ImGui::MenuItem("Model Import"))
			{
				/*
				auto path = Editor::FileOpen("All Files(*.*)\0");
				if (!path.empty())
				{
					// 왜인지 모르겠지만 간헐적으로 두 번째 이상 로드 시도시 멈춘다.
					m_pEditor->GetModelImporter()->LoadFromFile(path);
				}
				*/
			}

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuWindow()
	{
		if (ImGui::BeginMenu("Window"))
		{
			// 해당 Panel의 객체를 가져와야 한다.

			ImGui::EndMenu();
		}
	}

	void MenuBarPanel::menuHelp()
	{
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
	}
}