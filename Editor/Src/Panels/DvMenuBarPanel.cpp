#include "DvMenuBarPanel.h"
//#include "ScenePanel.h"
//#include "HierarchyPanel.h"
//#include "InspectorPanel.h"
//#include "AssetPanel.h"
//#include "Importer/ModelImporter.h"

namespace Editor
{
	DvMenuBarPanel::DvMenuBarPanel(DvEditor* pEditor)
		: DvPanel(pEditor, "MenuBar")
	{
		m_bWindow = false;

		//SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
	}

	DvMenuBarPanel::~DvMenuBarPanel()
	{
	}

	void DvMenuBarPanel::OnActiveScene(const Dive::Event& e)
	{
		//const auto& evt = dynamic_cast<const Dive::CreateSceneEvent&>(e);
		//m_pActiveScene = const_cast<Dive::Scene*>(evt.GetCreatedScene());
	}

	// �޴��ٴ� �׻� �׷����� �Ѵ�.
	void DvMenuBarPanel::renderAlways()
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

	void DvMenuBarPanel::menuFile()
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
					/*
					if (m_pActiveScene)
					{
						// ���� ���� Ȯ��
					}

					m_pActiveScene = Dive::GetCurrentEngine()->CreateScene("new_world");
					auto lightObj = m_pActiveScene->CreateGameObject("Directional_Light");
					lightObj->AddComponent<Dive::Transform>();
					auto lightCom = lightObj->AddComponent<Dive::Light>();
					lightCom->SetLightType(Dive::eLightType::Directional);
					*/
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
						// ���� ���� Ȯ��
					}

					m_pActiveScene = Dive::GetCurrentEngine()->CreateScene();

					// ���� Scene �̸��� ��������.
					Dive::ResourceManager::GetInstance().LoadFromDataFile();

					m_pActiveScene->Deserialize("Assets/Scenes/New_World.scene");
					*/
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close"))
			{
				//if (m_pActiveScene)
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
				/*
				if (m_pActiveScene)
				{
					m_pActiveScene->Serialize("Assets/Scenes/New_World.scene");

					// scene �̸��� ��������.
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
				/*
				if (m_pActiveScene)
				{
					// ���� ���� Ȯ��
				}
				*/
				
				DV_EVENT_FIRE(Dive::eDvEventType::ExitRequested);
			}

			ImGui::EndMenu();
		}
	}

	void DvMenuBarPanel::menuGameObject()
	{
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				/*
				if (m_pActiveScene)
				{
					auto pAddedGameObject = m_pActiveScene->CreateGameObject("GameObject");
					pAddedGameObject->AddComponent<Dive::Transform>();
				}
				*/
			}

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					/*
					if (m_pActiveScene)
					{
						// ����Ƽ���� ��ũ��Ʈ �������� �Լ��� �����Ѵ�.
						// GameObject�� CreatePrimitive����...

						// ���� �⺻���� ���´�
						// GameObject�� �����ϰ�
						// MeshRenderable�� �߰��� ��
						// ResourceManager�� ���� Model�� Set�ϴ� ���̴�.

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

	void DvMenuBarPanel::menuComponent()
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

	void DvMenuBarPanel::menuTools()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			// �� â�� ��� �ɼ��� ������� �ؾ� �Ѵ�.
			// ����Ƽ�� ��쿣 ���� �巡�׸� �ϸ� Inspector�� ����â�� ���.
			if (ImGui::MenuItem("Model Import"))
			{
				/*
				auto path = Editor::FileOpen("All Files(*.*)\0");
				if (!path.empty())
				{
					// ������ �𸣰����� ���������� �� ��° �̻� �ε� �õ��� �����.
					m_pEditor->GetModelImporter()->LoadFromFile(path);
				}
				*/
			}

			ImGui::EndMenu();
		}
	}

	void DvMenuBarPanel::menuWindow()
	{
		if (ImGui::BeginMenu("Window"))
		{
			// �ش� Panel�� ��ü�� �����;� �Ѵ�.

			ImGui::EndMenu();
		}
	}

	void DvMenuBarPanel::menuHelp()
	{
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
	}
}