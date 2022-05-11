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

// �޴��ٴ� �׻� �׷����� �Ѵ�.
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
					// ���� ���� Ȯ��
				}

				Dive::GetCurrentEngine()->CreateScene("new_world");
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
					// ���� ���� Ȯ��
				}

				m_pActiveScene = Dive::GetCurrentEngine()->CreateScene();

				// ���� Scene �̸��� ��������.
				Dive::ResourceManager::GetInstance().LoadFromDataFile();

				Dive::Serializer serializer(m_pActiveScene);
				serializer.Deserialize("Assets/Scenes/New_World.scene");
			}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Close"))
		{
			if (m_pActiveScene)
			{
				// ���� ���� Ȯ��

				// Engine���� Scene�� Close�ؾ� �Ѵ�.
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
				Dive::Serializer serializer(m_pActiveScene);
				// dir�� ��������!
				const std::string dir = "Assets/Scenes";
				serializer.Serialize(dir);

				// scene �̸��� ��������.
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
				// ���� ���� Ȯ��
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
					// ����Ƽ���� ��ũ��Ʈ �������� �Լ��� �����Ѵ�.
					// GameObject�� CreatePrimitive����...
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

		ImGui::EndMenu();
	}
}

void MenuBarPanel::menuWindow()
{
	if (ImGui::BeginMenu("Window"))
	{
		// �ش� Panel�� ��ü�� �����;� �Ѵ�.

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