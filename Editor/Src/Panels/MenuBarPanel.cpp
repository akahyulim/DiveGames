#include "MenuBarPanel.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetPanel.h"

MenuBarPanel::MenuBarPanel(Editor* pEditor)
	: Panel(pEditor, "MenuBar")
{
	m_bWindow = false;

	SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
}

MenuBarPanel::~MenuBarPanel()
{
}

void MenuBarPanel::OnActiveScene(const Dive::Event& e)
{
	const auto& evt = dynamic_cast<const Dive::CreateSceneEvent&>(e);
	m_pActiveScene = const_cast<Dive::Scene*>(evt.GetCreatedScene());
}

// 메뉴바는 항상 그려져야 한다.
void MenuBarPanel::renderAlways()
{
	ImGui::BeginMainMenuBar();

	menuFile();
	menuGameObject();
	menuComponent();
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
				if (m_pActiveScene)
				{
					// 저장 여부 확인
				}

				m_pActiveScene = Dive::GetCurrentEngine()->CreateScene("new_world");
				auto lightObj = m_pActiveScene->CreateGameObject("Directional_Light");
				lightObj->AddComponent<Dive::Transform>();
				auto lightCom = lightObj->AddComponent<Dive::Light>();
				lightCom->SetLightType(Dive::eLightType::Directional);
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
				if (m_pActiveScene)
				{
					// 저장 여부 확인
				}

				m_pActiveScene = Dive::GetCurrentEngine()->CreateScene();

				// 역시 Scene 이름을 전달하자.
				Dive::ResourceManager::GetInstance().LoadFromDataFile();

				m_pActiveScene->Deserialize("Assets/Scenes/New_World.scene");
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Close"))
		{
			if (m_pActiveScene)
			{
				// 저장 여부 확인

				// Engine에서 Scene을 Close해야 한다.
			}

		}
		if (ImGui::MenuItem("Close Project"))
		{

		}

		ImGui::Separator();

		if (ImGui::MenuItem("Save"))
		{
			if (m_pActiveScene)
			{
				m_pActiveScene->Serialize("Assets/Scenes/New_World.scene");

				// scene 이름을 전달하자.
				Dive::ResourceManager::GetInstance().SaveToDataFile();
			}
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
			if (m_pActiveScene)
			{
				// 저장 여부 확인
			}

			m_pEditor->Close();
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
			if (m_pActiveScene)
			{
				auto pAddedGameObject = m_pActiveScene->CreateGameObject("GameObject");
				pAddedGameObject->AddComponent<Dive::Transform>();
			}
		}

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				if (m_pActiveScene)
				{
					// 유니티에선 스크립트 차원에서 함수가 존재한다.
					// GameObject의 CreatePrimitive였나...

					// 일단 스파르탄의 경우 static 함수로 만들었다.
					// 과정은
					// 1. 빈 게임오브젝트 생성
					// 2. Renderable component 추가
					// 3. Renderable에서 GeometrySet & UseDefaultMaterial
					// 4. 마지막으로 이름 설정
					// => 분석 결과 Model을 직접생성하고, vertices, indices를 직접 전달하여
					// 버퍼를 생성한 후 해당 Renderable이 관리한다.
					// 즉, Resource로서의 관리가 전혀 되지 않고 있다.
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cube.obj");
				}
			}

			if (ImGui::MenuItem("Sphere"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Sphere.obj");
				}
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cylinder.obj");
				}
			}

			if (ImGui::MenuItem("Cone"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cone.obj");
				}
			}

			if (ImGui::MenuItem("Plane"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Plane.obj");
				}
			}

			ImGui::Separator();

			// hazel의 경우 mesh라는 entity를 만들고 mesh component에서 파일을 로드하였다.
			if (ImGui::MenuItem("Stormtrooper"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/dancing-stormtrooper/source/silly_dancing.fbx");
				}
			}

			if (ImGui::MenuItem("Pilot"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/pilot-avatar/source/Pilot_LP_Animated.fbx");
				}
			}

			if (ImGui::MenuItem("Sponza"))
			{
				if (m_pActiveScene)
				{
					Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Sponza-master/sponza.obj");
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
			if (ImGui::MenuItem("Directional"))
			{
				if (m_pActiveScene)
				{
					auto lightObj = m_pActiveScene->CreateGameObject("Directional_Light");
					lightObj->AddComponent<Dive::Transform>();
					auto lightCom = lightObj->AddComponent<Dive::Light>();
					lightCom->SetLightType(Dive::eLightType::Directional);
				}
			}

			if (ImGui::MenuItem("Point"))
			{
				if (m_pActiveScene)
				{
					auto lightObj = m_pActiveScene->CreateGameObject("Point_Light");
					lightObj->AddComponent<Dive::Transform>();
					auto lightCom = lightObj->AddComponent<Dive::Light>();
					lightCom->SetLightType(Dive::eLightType::Point);
				}
			}

			if (ImGui::MenuItem("Spot"))
			{
				if (m_pActiveScene)
				{
					auto lightObj = m_pActiveScene->CreateGameObject("Spot_Light");
					lightObj->AddComponent<Dive::Transform>();
					auto lightCom = lightObj->AddComponent<Dive::Light>();
					lightCom->SetLightType(Dive::eLightType::Spot);
				}
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
		}

		if (ImGui::MenuItem("Light"))
		{
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