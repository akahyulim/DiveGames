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
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = nullptr;

	// 이건 scene와 project를 구분해야 한다.
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
					// 저장, 저장안함, 취소
				}

				// 새로운 scene 생성
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				//if (m_pWorld->IsDirty())
				{
					// 저장, 저장안함, 취소
				}

				if (GetOpenFileName(&ofn) != 0)
				{
					// 이후 로드 실행
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save"))
			{
				// 기존 세이브가 없다면 지정된 경로에 scene이름으로 저장
				// 이후부턴 해당 scene 이름으로 새로 저장
			}

			if (ImGui::MenuItem("Save As..."))
			{
				if (GetSaveFileName(&ofn) != 0)
				{
					// 이후 지정된 경로에 지정된 이름으로 저장
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("New Project"))
			{
			}

			if (ImGui::MenuItem("Open Project"))
			{
				// 프로젝트 단위도 검사해야 하나?
				//if (m_pWorld->IsDirty())
				{
					// 저장, 저장안함, 취소
				}

				if (GetOpenFileName(&ofn) != 0)
				{
					// 이후 로드 실행
				}
			}

			if (ImGui::MenuItem("Save Project"))
			{
				if (GetSaveFileName(&ofn) != 0)
				{
					// 이후 로드 실행
				}
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
