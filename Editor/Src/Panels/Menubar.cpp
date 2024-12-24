#include "Menubar.h"
#include "ImGuiFileDialog.h"
#include "../Editor.h"

namespace Dive
{
	Menubar::Menubar(Editor* pEditor)
		: Panel(pEditor)
		, m_bShowNewProject(false)
		, m_bShowNewWorld(false)
		, m_bShowOpenProject(false)
		, m_bShowOpenWorld(false)
	{
		m_Title = "Menubar";
		m_bWindow = false;

		// test scene
		{
			//if(m_pEditor->CreateProject("test_project"))
			//	GEngine->CreateWorld("test_world");

			if (m_pEditor->OpenProject("test_project.proj"))
			{
				GEngine->OpenWorld("test_project\\Assets\\Worlds\\test_world.world");
			//	GEngine->CreateWorld("test_world");
			}
		}
	}
	
	void Menubar::renderAlways()
	{
		ImGui::PushFont(m_pEditor->GetFont(eFontTypes::Normal));

		ImGui::BeginMainMenuBar();

		menuFile();
		menuGameObject();
		menuComponent();
		menuTools();
		menuView();
		menuHelp();

		ImGui::EndMainMenuBar();

		showNewProject();
		showNewWorld();
		showOpenProject();
		showOpenWorld();

		ImGui::PopFont();
	}

	void Menubar::menuFile()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Project"))
				{
					// 기존 프로젝트 저장 여부

					m_bShowNewProject = true;
				}
				if (ImGui::MenuItem("World", nullptr, nullptr, m_pEditor->IsProjectLoaded()))
				{
					// 기존 월드 저장 여부

					m_bShowNewWorld = true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open"))
			{
				if (ImGui::MenuItem("Project"))
				{
					ImGuiFileDialog::Instance()->OpenDialog("ChooseProject", "Choose Project", ".proj");

				}
				if (ImGui::MenuItem("World", nullptr, nullptr, m_pEditor->IsProjectLoaded()))
				{
					ImGuiFileDialog::Instance()->OpenDialog("ChooseWorld", "Choose World", ".world", 
						{ m_pEditor->GetProjectWorldsPath()});
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Close World"))
			{
				GEngine->CloseWorld();
			}
			if (ImGui::MenuItem("Close Project", nullptr, nullptr, m_pEditor->IsProjectLoaded()))
			{
				m_pEditor->CloseProject();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save", nullptr, nullptr, GEngine->GetActiveWorld() ? true : false))
			{
				GEngine->GetActiveWorld()->SaveToFile(m_pEditor->GetProjectWorldsPath());
			}

			if (ImGui::MenuItem("Save As..."))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				/*
				if (m_pActiveWorld)
				{
					// 저장 여부 확인
				}
				*/

				DV_FIRE_EVENT(eEventType::Exit);
			}

			ImGui::EndMenu();
		}

	}

	void Menubar::menuGameObject()
	{
		if (ImGui::BeginMenu("GameObject"))
		{
			auto pActiveWorld = GEngine->GetActiveWorld();

			if (ImGui::MenuItem("Create Empty", nullptr, nullptr, pActiveWorld != nullptr))
			{
				auto pNewGameObject = pActiveWorld->CreateGameObject();
			}

			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					/*
					if (m_pActiveWorld)
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
					if (m_pActiveWorld)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Sphere.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Cylinder"))
				{
					/*
					if (m_pActiveWorld)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cylinder.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Cone"))
				{
					/*
					if (m_pActiveWorld)
					{
						Dive::ResourceManager::GetInstance().Load<Dive::Model>("Assets/Models/Base/Cone.obj");
					}
					*/
				}

				if (ImGui::MenuItem("Plane"))
				{
					/*
					if (m_pActiveWorld)
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
					if (m_pActiveWorld)
					{
						auto lightObj = m_pActiveWorld->CreateGameObject("Directional_Light");
						lightObj->AddComponent<Dive::Transform>();
						auto lightCom = lightObj->AddComponent<Dive::Light>();
						lightCom->SetLightType(Dive::eLightType::Directional);
					}
					*/
				}

				if (ImGui::MenuItem("Point"))
				{
					/*
					if (m_pActiveWorld)
					{
						auto lightObj = m_pActiveWorld->CreateGameObject("Point_Light");
						lightObj->AddComponent<Dive::Transform>();
						auto lightCom = lightObj->AddComponent<Dive::Light>();
						lightCom->SetLightType(Dive::eLightType::Point);
					}
					*/
				}

				if (ImGui::MenuItem("Spot"))
				{
					/*
					if (m_pActiveWorld)
					{
						auto lightObj = m_pActiveWorld->CreateGameObject("Spot_Light");
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

	void Menubar::menuComponent()
	{
		if (ImGui::BeginMenu("Component"))
		{
			if (ImGui::MenuItem("SpriteRenderable"))
			{
				/*
				if (m_pActiveWorld)
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
				if (m_pActiveWorld)
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

	void Menubar::menuTools()
	{
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Terrain Editor"))
			{

			}

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

	void Menubar::menuView()
	{
		if (ImGui::BeginMenu("View"))
		{
			static bool showView = true;

			showView = m_pEditor->GetPanel(ePanelTypes::WorldView).IsVisible();
			ImGui::Checkbox("WorldView", &showView);
			m_pEditor->GetPanel(ePanelTypes::WorldView).SetVisible(showView);

			showView = m_pEditor->GetPanel(ePanelTypes::GameView).IsVisible();
			ImGui::Checkbox("GameView", &showView);
			m_pEditor->GetPanel(ePanelTypes::GameView).SetVisible(showView);

			showView = m_pEditor->GetPanel(ePanelTypes::HierarchyView).IsVisible();
			ImGui::Checkbox("HierarchyView", &showView);
			m_pEditor->GetPanel(ePanelTypes::HierarchyView).SetVisible(showView);

			showView = m_pEditor->GetPanel(ePanelTypes::InspectorView).IsVisible();
			ImGui::Checkbox("InspectorView", &showView);
			m_pEditor->GetPanel(ePanelTypes::InspectorView).SetVisible(showView);

			showView = m_pEditor->GetPanel(ePanelTypes::AssetView).IsVisible();
			ImGui::Checkbox("AssetView", &showView);
			m_pEditor->GetPanel(ePanelTypes::AssetView).SetVisible(showView);

			ImGui::EndMenu();
		}
	}

	void Menubar::menuHelp()
	{
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
	}

	void Menubar::showNewProject()
	{
		if (!m_bShowNewProject) 
			return; 
		
		ImGui::SetNextWindowSize(ImVec2(450, 250)); 
		ImGui::Begin("New Project", &m_bShowNewProject, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);
		
		ImGui::PushFont(m_pEditor->GetFont(eFontTypes::Large_Bold)); 
		ImGui::Spacing();
		ImGui::Text(u8"새로운 프로젝트"); 
		ImGui::PopFont(); 
		
		ImGui::Spacing(); 
		ImGui::Spacing(); 
		ImGui::Spacing(); 
		
		static std::string projectName = "new_project"; 
		static std::string currentPath = std::filesystem::current_path().string(); 
		char projectNameBuffer[256]; 
		char currentPathBuffer[512]; 
		strncpy_s(projectNameBuffer, projectName.c_str(), sizeof(projectNameBuffer)); 
		strncpy_s(currentPathBuffer, currentPath.c_str(), sizeof(currentPathBuffer)); 
		
		ImGui::Text(u8"프로젝트 이름"); 
		ImGui::PushItemWidth(400); 
		if (ImGui::InputText("##project_name", projectNameBuffer, sizeof(projectNameBuffer))) 
		{ 
			projectName = std::string(projectNameBuffer); 
		} 
		ImGui::PopItemWidth(); 
		
		ImGui::Spacing(); 
		
		ImGui::Text(u8"위치"); 
		ImGui::PushItemWidth(400); 
		if (ImGui::InputText("##location", currentPathBuffer, sizeof(currentPathBuffer))) 
		{ 
			currentPath = std::string(currentPathBuffer); 
		} 
		ImGui::PopItemWidth(); 
		
		ImGui::SameLine(); 
		
		if (ImGui::Button(u8"...")) 
		{ 
			ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr);
		} 
		
		if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey")) 
		{ 
			ImVec2 dialogSize = ImGui::GetWindowSize(); ImVec2 buttonSize(100, 30); 
			float padding = 10.0f; if (ImGuiFileDialog::Instance()->IsOk()) 
			{
				currentPath = ImGuiFileDialog::Instance()->GetCurrentPath(); 
				strncpy_s(currentPathBuffer, currentPath.c_str(), sizeof(currentPathBuffer)); 
				currentPathBuffer[sizeof(currentPathBuffer) - 1] = '\0'; // 안전한 null-종료 
			} 
			
			ImGuiFileDialog::Instance()->Close();
		} 
		
		ImVec2 window_size = ImGui::GetWindowSize(); 
		ImGui::SetCursorPos(ImVec2(window_size.x - 220, window_size.y - 40)); 
		
		if (ImGui::Button(u8"취소", ImVec2(100, 30))) 
		{ 
			m_bShowNewProject = false; 
		} 
		ImGui::SameLine(); 
		if (ImGui::Button(u8"만들기", ImVec2(100, 30))) 
		{ 
			m_bShowNewProject = false; 

			auto projectPath = currentPath + "\\" + projectName; 
			m_pEditor->CreateProject(projectPath);
		} 
		
		ImGui::End();
	}
	
	void Menubar::showNewWorld()
	{
		if (!m_bShowNewWorld)
			return;

		ImGui::SetNextWindowSize(ImVec2(420, 200));
		ImGui::Begin("New World", &m_bShowNewProject, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

		ImGui::PushFont(m_pEditor->GetFont(eFontTypes::Large_Bold));
		ImGui::Spacing();
		ImGui::Text(u8"새로운 월드");
		ImGui::PopFont();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		static std::string worldName = "new_world";
		char worldNameBuffer[256];
		strncpy_s(worldNameBuffer, worldName.c_str(), sizeof(worldNameBuffer));

		ImGui::Text(u8"월드 이름");
		ImGui::PushItemWidth(400);
		if (ImGui::InputText("##world_name", worldNameBuffer, sizeof(worldNameBuffer)))
		{
			worldName = std::string(worldNameBuffer);
		}

		ImVec2 window_size = ImGui::GetWindowSize();
		ImGui::SetCursorPos(ImVec2(window_size.x - 220, window_size.y - 40));

		if (ImGui::Button(u8"취소", ImVec2(100, 30)))
		{
			m_bShowNewWorld = false;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"만들기", ImVec2(100, 30)))
		{
			bool existSameName = false;
			const std::string path = std::filesystem::current_path().string() + "\\Assets\\Worlds";

			// 동일한 이름의 파일이 있는지 확인
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_regular_file() && entry.path().stem() == worldName)
				{
					existSameName = true;
					break;
				}
			}

			if (existSameName)
			{
				ImGui::OpenPopup("##SameWorldNameWarning");
			}
			else
			{
				GEngine->CreateWorld(worldName);
				m_bShowNewWorld = false;
			}
		}

		if (ImGui::BeginPopupModal("##SameWorldNameWarning", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"동일한 이름의 월드가 이미 존재합니다.");
			if (ImGui::Button(u8"확인", ImVec2(100, 30)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void Menubar::showOpenProject()
	{
		if (ImGuiFileDialog::Instance()->Display("ChooseProject"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				auto filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				m_pEditor->OpenProject(filePathName);
			}

			ImGuiFileDialog::Instance()->Close();
		}
	}
	
	void Menubar::showOpenWorld()
	{
		if (ImGuiFileDialog::Instance()->Display("ChooseWorld"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				auto filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				GEngine->OpenWorld(filePathName);
			}

			ImGuiFileDialog::Instance()->Close();
		}
	}
}