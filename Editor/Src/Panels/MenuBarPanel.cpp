#include "MenuBarPanel.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetPanel.h"
#include "Importer/ModelImporter.h"

MenuBarPanel::MenuBarPanel(Editor* pEditor)
	: Panel(pEditor, "MenuBar")
{
	m_bWindow = false;

	m_pChoA = Dive::Texture2D::Create("Assets/Textures/ChoA.jpg", "ChoA");
	m_pIU = Dive::Texture2D::Create("Assets/Textures/IU.jpg", "IU");
}

MenuBarPanel::~MenuBarPanel()
{
	DV_DELETE(m_pChoA);
	DV_DELETE(m_pIU);

	DV_DELETE(m_pActiveScene);
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
					DV_DELETE(m_pActiveScene);

				m_pActiveScene = new Dive::Scene("new_world");

				m_pEditor->GetScene()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetHierarchy()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetInspector()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetModelImporter()->SetScene(m_pActiveScene);
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
					DV_DELETE(m_pActiveScene);

				// 원래는 대화상자로 선택
				m_pActiveScene = new Dive::Scene;
				Dive::Serializer serializer(m_pActiveScene);
				serializer.Deserialize("Assets/Scene/New_World.scene");

				// 번잡해...
				m_pEditor->GetScene()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetHierarchy()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetInspector()->SetActiveScene(m_pActiveScene);

				m_pEditor->GetModelImporter()->SetScene(m_pActiveScene);
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Close"))
		{
			if (m_pActiveScene)
			{
				DV_DELETE(m_pActiveScene);
			}

			m_pEditor->GetScene()->SetActiveScene(nullptr);
			m_pEditor->GetHierarchy()->SetActiveScene(nullptr);
			m_pEditor->GetInspector()->SetActiveScene(nullptr);
		}
		if (ImGui::MenuItem("Close Project"))
		{

		}

		ImGui::Separator();

		if (ImGui::MenuItem("Save"))
		{
			if (m_pActiveScene)
			{
				Dive::Serializer serializer(m_pActiveScene);
				// dir만 전달하자!
				const std::string dir = "Assets/Scene";
				serializer.Serialize(dir);
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
			// Scene 저장 확인

			// Scene을 제거할 때 마다 사용하는 곳 모두에게 직접 전달하고 있다.
			// 너무 비효율적이다.
			if (m_pActiveScene)
			{
				DV_DELETE(m_pActiveScene);
			}
			m_pEditor->GetScene()->SetActiveScene(nullptr);
			m_pEditor->GetHierarchy()->SetActiveScene(nullptr);
			m_pEditor->GetInspector()->SetActiveScene(nullptr);

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
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/Cube.obj");
				}
			}

			if (ImGui::MenuItem("Sphere"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/sphere.obj");
				}
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/cylinder.obj");
				}
			}

			if (ImGui::MenuItem("Cone"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/cone.obj");
				}
			}

			if (ImGui::MenuItem("Plane"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/plane.obj");
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Stormtrooper"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/dancing-stormtrooper/source/silly_dancing.fbx");
				}
			}

			if (ImGui::MenuItem("Pilot"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/pilot-avatar/source/Pilot_LP_Animated.fbx");
				}
			}

			if (ImGui::MenuItem("Sponza"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Sponza-master/sponza.obj");
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Light"))
		{
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