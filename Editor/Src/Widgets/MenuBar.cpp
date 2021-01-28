#include "MenuBar.h"


MenuBar::MenuBar(Editor* editor)
	: Widget(editor)
{
	m_bWindow = false;
	m_scene = m_editor->GetContext()->GetSubsystem<Scene>().get();
	assert(m_scene);
//	m_pSelectedObj = nullptr;

	/*
	m_pWorld = m_pSystemManager->GetSystem<Dive::World>();
	{
		if (!m_pWorld)
		{
			APP_ERROR("MenuBar::MenuBar >> World가 존재하지 않습니다.");
			return;
		}

		if ((!g_pWidgetScene) || (!g_pWidgetHierarchy) || (!g_pWidgetInspector) || (!g_pWidgetConsole) || (!g_pWidgetAssets))
		{
			APP_ERROR("MenuBar::MenuBar >> 메뉴 구성 요소가 존재하지 않습니다.");
			return;
		}
	}

	m_projectName = Dive::ProjectPathManager::GetIntance().GetProjectName();
	m_sceneName = Dive::ProjectPathManager::GetIntance().GetSceneName();
	*/
}

void MenuBar::Tick(float deltaTime)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				// 일단 Scene을 Clear하자.
				m_scene->New();
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				// 하드 코딩으로 특정 이름의 파일을 열자.
				m_scene->LoadFromFile("../Assets/Scenes/LastDay.scene");
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{
				// 하등 코딩으로 특정 이름의 파일로 저장하자.
				m_scene->SaveToFile("../Assets/Scenes/LastDay.scene");
			}

			if (ImGui::MenuItem("Save As..."))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New Project"))
			{
			}

			if (ImGui::MenuItem("Open Project"))
			{
			}

			if (ImGui::MenuItem("Save Project"))
			{
			}

			// settings???

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				//if (m_pWorld->IsDirty())
				{
					// 저장 여부 확인
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
				auto addedGameObject = m_scene->CreateGameObject();
				addedGameObject->SetName("Empty");
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
				auto camera = m_scene->CreateGameObject();
				camera->AddComponent<Camera>();
				camera->SetName("Camera");
			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("Component"))
		{
			// 단독 생성 불가!!! SelectedObject가 있어야 한다.
			// => 없다면 아래의 메뉴 비활성
			// 그리고 Transform은 포함하지 않는다.
			// 중복 생성도 확인이 필요하다.
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

			ImGui::MenuItem(u8"설정");
			*/
			ImGui::EndMenu();
		}

		ImGui::SameLine(0.0f, 30.0f);

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem(u8"Dive Editor 정보", nullptr, &m_bPopupAbout);

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
	// 한글 출력이 안된다.
	if (ImGui::Begin(u8"Dive Engine", &m_bPopupAbout, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::NewLine();
		ImGui::Text(u8"버전: 0.001");
		ImGui::Text(u8"릴리즈: 2020/04/30");
		ImGui::Text(u8"제작: 서보윤( akahyulim@gmail.com )");		// text url은 어떻게 하냐?

		ImGui::NewLine();

		ImGui::Text(u8"Dive Engine 에디터 프로그램으로 아래의 기능을 수행할 수 있습니다.");
		ImGui::Text(u8"");
		ImGui::Text(u8"");

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Text(u8"아래의 공개 라이브러리를 사용합니다.");
		ImGui::Text("spdlog");
		ImGui::Text("ImGui");

		ImGui::NewLine();

		ImGui::End();
	}
}
