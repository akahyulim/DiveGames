#include "MenuBar.h"


MenuBar::MenuBar(Editor* editor)
	: Widget(editor)
{
	m_bWindow = false;
//	m_pSelectedObj = nullptr;

	/*
	m_pWorld = m_pSystemManager->GetSystem<Dive::World>();
	{
		if (!m_pWorld)
		{
			APP_ERROR("MenuBar::MenuBar >> World�� �������� �ʽ��ϴ�.");
			return;
		}

		if ((!g_pWidgetScene) || (!g_pWidgetHierarchy) || (!g_pWidgetInspector) || (!g_pWidgetConsole) || (!g_pWidgetAssets))
		{
			APP_ERROR("MenuBar::MenuBar >> �޴� ���� ��Ұ� �������� �ʽ��ϴ�.");
			return;
		}
	}

	m_projectName = Dive::ProjectPathManager::GetIntance().GetProjectName();
	m_sceneName = Dive::ProjectPathManager::GetIntance().GetSceneName();
	*/
}

void MenuBar::Tick(float deltaTime)
{
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;

	// �̰� scene�� project�� �����ؾ� �Ѵ�.
	ofn.lpstrFilter = L"All Files(*.*)\0*.scene\0";

	ofn.lpstrFile = nullptr;
	ofn.nMaxFile = MAX_PATH;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				//if (m_pWorld->IsDirty())
				{
					// ����, �������, ���
				}

				// ���ο� scene ����
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				//if (m_pWorld->IsDirty())
				{
					// ����, �������, ���
				}

				if (GetOpenFileName(&ofn) != 0)
				{
					// ���� �ε� ����
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{
				// ���� ���̺갡 ���ٸ� ������ ��ο� scene�̸����� ����
				// ���ĺ��� �ش� scene �̸����� ���� ����
			}

			if (ImGui::MenuItem("Save As..."))
			{
				if (GetSaveFileName(&ofn) != 0)
				{
					// ���� ������ ��ο� ������ �̸����� ����
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New Project"))
			{
			}

			if (ImGui::MenuItem("Open Project"))
			{
				// ������Ʈ ������ �˻��ؾ� �ϳ�?
				//if (m_pWorld->IsDirty())
				{
					// ����, �������, ���
				}

				if (GetOpenFileName(&ofn) != 0)
				{
					// ���� �ε� ����
				}
			}

			if (ImGui::MenuItem("Save Project"))
			{
				if (GetSaveFileName(&ofn) != 0)
				{
					// ���� �ε� ����
				}
			}

			// settings???

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				//if (m_pWorld->IsDirty())
				{
					// ���� ���� Ȯ��
				}

				PostQuitMessage(0);
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				//m_pWorld->CreateGameObject();
			}

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Triangle"))
				{
					/*
					auto pObject = m_pWorld->CreateGameObject();
					auto pRenderable = pObject->AddComponent<Dive::Renderable>();
					pRenderable->SetDefaultGeometry(Dive::eGeometryType::Default_Triangle);
					pRenderable->SetDefaultMaterial();
					pObject->SetName("Triangle");
					*/
				}

				if (ImGui::MenuItem("Quad"))
				{
					/*
					auto pObject = m_pWorld->CreateGameObject();
					auto pRenderable = pObject->AddComponent<Dive::Renderable>();
					pRenderable->SetDefaultGeometry(Dive::eGeometryType::Default_Quad);
					pRenderable->SetDefaultMaterial();
					pObject->SetName("Quad");
					*/
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Cube"))
				{
					/*
					auto pObject = m_pWorld->CreateGameObject();
					auto pRenderable = pObject->AddComponent<Dive::Renderable>();
					pRenderable->SetDefaultGeometry(Dive::eGeometryType::Default_Cube);
					pRenderable->SetDefaultMaterial();
					pObject->SetName("Cube");
					*/
				}

				if (ImGui::MenuItem("Plane"))
				{
					/*
					auto pObject = m_pWorld->CreateGameObject();
					auto pRenderable = pObject->AddComponent<Dive::Renderable>();
					pRenderable->SetDefaultGeometry(Dive::eGeometryType::Default_Plane);
					pRenderable->SetDefaultMaterial();
					pObject->SetName("Plane");
					*/
				}

				if (ImGui::MenuItem("Sphere"))
				{
					/*
					auto pObject = m_pWorld->CreateGameObject();
					auto pRenderable = pObject->AddComponent<Dive::Renderable>();
					pRenderable->SetDefaultGeometry(Dive::eGeometryType::Default_Sphere);
					pRenderable->SetDefaultMaterial();
					pObject->SetName("Sphere");
					*/
				}

				if (ImGui::MenuItem("Cylinder"))
				{
				}

				if (ImGui::MenuItem("Cone"))
				{
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional"))
				{
					/*
					auto pLightObject = m_pWorld->CreateGameObject();
					auto pComLight = pLightObject->AddComponent<Dive::Light>();
					pComLight->SetLightType(Dive::eLightType::Directional);
					pLightObject->SetName("Directional_Light");
					*/
				}

				if (ImGui::MenuItem("Spot"))
				{
				}

				if (ImGui::MenuItem("Point"))
				{
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Camera"))
			{
				//m_pWorld->CreateCameraObject();
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("Component"))
		{
			// �ܵ� ���� �Ұ�!!! SelectedObject�� �־�� �Ѵ�.
			// => ���ٸ� �Ʒ��� �޴� ��Ȱ��
			// �׸��� Transform�� �������� �ʴ´�.
			// �ߺ� ������ Ȯ���� �ʿ��ϴ�.
			if (ImGui::MenuItem("Camera"))
			{
				//if (m_pSelectedObj)
				{
				//	if (!m_pSelectedObj->GetComponent<Dive::Camera>())
				//		m_pSelectedObj->AddComponent<Dive::Camera>();
				}
			}

			if (ImGui::MenuItem("Renderable"))
			{
			}

			if (ImGui::MenuItem("Light"))
			{
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("Window"))
		{
			/*
			ImGui::MenuItem("Scene", NULL, &g_pWidgetScene->IsVisible());
			ImGui::MenuItem("Game", NULL, &g_pWidgetGame->IsVisible());
			ImGui::MenuItem("Hierarchy", NULL, &g_pWidgetHierarchy->IsVisible());
			ImGui::MenuItem("Components", NULL, &g_pWidgetInspector->IsVisible());
			ImGui::MenuItem("Log", NULL, &g_pWidgetConsole->IsVisible());
			ImGui::MenuItem("Assets", NULL, &g_pWidgetAssets->IsVisible());

			ImGui::Separator();

			ImGui::MenuItem("Image Importer");// , NULL, &g_pWidgetImageImporter->IsVisible());
			ImGui::MenuItem("Model Importer");

			ImGui::Separator();

			ImGui::MenuItem(u8"����");
			*/
			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem(u8"Dive Editor ����", nullptr, &m_bPopupAbout);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_bPopupAbout)
	{
		ImGui::SetNextWindowFocus();
		popupAbout();
	}
}

void MenuBar::popupAbout()
{
	// �ѱ� ����� �ȵȴ�.
	if (ImGui::Begin(u8"Dive Engine", &m_bPopupAbout, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::NewLine();
		ImGui::Text(u8"����: 0.001");
		ImGui::Text(u8"������: 2020/04/30");
		ImGui::Text(u8"����: ������( akahyulim@gmail.com )");		// text url�� ��� �ϳ�?

		ImGui::NewLine();

		ImGui::Text(u8"Dive Engine ������ ���α׷����� �Ʒ��� ����� ������ �� �ֽ��ϴ�.");
		ImGui::Text(u8"");
		ImGui::Text(u8"");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text(u8"�Ʒ��� ���� ���̺귯���� ����մϴ�.");
		ImGui::Text("spdlog");
		ImGui::Text("ImGui");

		ImGui::NewLine();

		ImGui::End();
	}
}
