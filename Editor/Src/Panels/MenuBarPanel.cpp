#include "MenuBarPanel.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetPanel.h"
#include "Importer/ModelImporter.h"
#include "Utilities/BasicGeometry.h"

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

				m_pActiveScene = new Dive::Scene("Sample_World");
				auto pChoA = m_pActiveScene->CreateGameObject("ChoA");
				auto pChoASprite = pChoA->AddComponent<Dive::SpriteRenderable>();
				pChoA->GetComponent<Dive::Transform>()->SetPosition(DirectX::XMFLOAT3(300.0, 0.0f, 800.0f));
				pChoASprite->SetSprite(m_pChoA);
				
				auto pIU = m_pActiveScene->CreateGameObject("IU");
				auto pIUSprite = pIU->AddComponent<Dive::SpriteRenderable>();
				pIU->GetComponent<Dive::Transform>()->SetPosition(DirectX::XMFLOAT3(-200.0, 0.0f, 900.0f));
				pIUSprite->SetSprite(m_pIU);
				
				auto pParent = m_pActiveScene->CreateGameObject("Knave");
				pParent->AddComponent<Dive::SpriteRenderable>();
				
				auto pChild = m_pActiveScene->CreateGameObject(13254521934309852950, "Complex");
				pChild->AddComponent<Dive::SpriteRenderable>();
				pChild->GetComponent<Dive::Transform>()->SetParent(pParent->GetComponent<Dive::Transform>());

				// 전달까지 직접하는게 맞다.
				// 하지만 방법이 너무 막무가내다....
				// ActiveScene을 static 포인터로 하면 어떨까...?
				// => Editor용 EventSystem을 만드는 건 어떨까?
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
				m_pActiveScene->CreateGameObject("GameObject");
			}
		}

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				if (m_pActiveScene)
				{
					// Model 생성은 일단 임시...
					// 실제로는 이미 생성해 놓은 Model의 복사본을 ResourceManager로부터 리턴받아야 할 듯?
					// 따라서 Resource는 생성 및 제거를 Manager가 관리해야 한다.
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/cube.fbx");
				}
			}

			if (ImGui::MenuItem("Sphere"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/sphere.fbx");
				}
			}

			if (ImGui::MenuItem("Capsule"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/capsule.fbx");
				}
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				if (m_pActiveScene)
				{
					auto pModel = new Dive::Model();
					m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/cylinder.fbx");
				}
			}

			if (ImGui::MenuItem("Plane"))
			{
				if (m_pActiveScene)
				{
					std::vector<Dive::VertexType> vertices;
					std::vector<unsigned int> indices;
					Utilities::Geometry::CreatePlane(&vertices, &indices, 2, 2);

					//auto pModel = new Dive::Model();
					//m_pEditor->GetModelImporter()->Load(pModel, "Assets/Models/Base/cylinder.obj");
				}
			}

			if (ImGui::MenuItem("Quad"))
			{
				if (m_pActiveScene)
				{
					std::vector<Dive::VertexType> vertices;
					std::vector<unsigned int> indices;
					Utilities::Geometry::CreateQuad(&vertices, &indices);

					// 이건 리소스지만
					auto pModel = new Dive::Model();
					pModel->AppendGeometry(vertices, indices);
					pModel->UpdateGeometry();

					// 이것도 리소스일 필요가 있나...?
					// 아... 동일한 모델을 사용하는 다른 GameObject에서 사용할 수 있다.
					auto pMaterial = new Dive::LegacyMaterial;

					auto pGameObject = m_pActiveScene->CreateGameObject("Quad");
					auto pMeshRenderable = pGameObject->AddComponent<Dive::MeshRenderable>();
					pMeshRenderable->SetGeometry("Quad", 0, static_cast<unsigned int>(vertices.size()), 0, static_cast<unsigned int>(indices.size()), pModel);
					pMeshRenderable->SetMaterail(pMaterial);
					pModel->SetRootGameObject(pGameObject);
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