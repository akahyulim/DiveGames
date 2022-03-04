#include "MenuBarPanel.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "AssetPanel.h"
#include "Base/EditorScene.h"

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

				m_pActiveScene = new EditorScene("Sample_World");
				auto pChoA = m_pActiveScene->CreateGameObject("ChoA");
				auto pChoASprite = pChoA->AddComponent<Dive::SpriteRenderer>();
				pChoA->GetComponent<Dive::Transform>()->SetPosition(DirectX::XMFLOAT3(300.0, 0.0f, 800.0f));
				pChoASprite->SetTexture(m_pChoA);
				auto pIU = m_pActiveScene->CreateGameObject("IU");
				auto pIUSprite = pIU->AddComponent<Dive::SpriteRenderer>();
				pIU->GetComponent<Dive::Transform>()->SetPosition(DirectX::XMFLOAT3(-200.0, 0.0f, 900.0f));
				pIUSprite->SetTexture(m_pIU);
				auto pParent = m_pActiveScene->CreateGameObject("Knave");
				auto pChild = m_pActiveScene->CreateGameObject(13254521934309852950, "Complex");
				pChild->GetComponent<Dive::Transform>()->SetParent(pParent->GetComponent<Dive::Transform>());

				// ���ޱ��� �����ϴ°� �´�.
				// ������ ����� �ʹ� ����������....
				// ActiveScene�� static �����ͷ� �ϸ� ���...?
				m_pEditor->GetScene()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetHierarchy()->SetActiveScene(m_pActiveScene);
				m_pEditor->GetInspector()->SetActiveScene(m_pActiveScene);
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
			// Scene ���� Ȯ��

			// Scene�� ������ �� ���� ����ϴ� �� ��ο��� ���� �����ϰ� �ִ�.
			// �ʹ� ��ȿ�����̴�.
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