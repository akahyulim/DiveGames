#include "EditorScene.h"
#include "Core/LogManager.h"
#include "ImGui/ImGuiHelper.h"
#include "Graphics/Graphics.h"
#include "Rendering/Renderer2D.h"
#include "Graphics/Types.h"
#include "Graphics/TextureManager.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <ImGuizmo.h>

namespace Sokoban
{
	EditorScene::EditorScene(const GameContext& context)
		: Scene(context)
	{
	}

	EditorScene::~EditorScene()
	{
	}

	void EditorScene::Initialize()
	{
		auto camera = m_gameContext.camera;// ->LookAt({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		// 예시: (x, y) 위치에 있는 2D 카메라
		camera->LookAt(
			{ 0.0f, 0.0f, -10.0f }, // eye
			{ 0.0f, 0.0f,   0.0f }, // target
			{ 0.0f, 1.0f,   0.0f }  // up (+Y가 위쪽)
		);
		camera->SetOrthographic(1600, 900, 0.1f, 100.0f);
	}

	void EditorScene::Cleanup()
	{
	}

	void EditorScene::Update(float deltaTime)
	{
	}

	void EditorScene::Render()
	{
		if (m_gameContext.renderer == nullptr)
			return;

		auto graphics = m_gameContext.graphics;

		m_gameContext.renderer->BeginScene(
			DirectX::XMMatrixTranspose(
				m_gameContext.camera->GetViewMatrix() *
				m_gameContext.camera->GetProjectionMatrix()));

		m_gameContext.renderer->DrawRect({ 0.0f, 0.0f }, { 800.0f, 600.0f }, Dive::Color::White);



		m_gameContext.renderer->DrawQuad({ -200.0f, 100.0f }, { 100.0f, 100.0f }, 45.0f, Dive::Color::Red);
			//Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/crates/crate_01.png"));
		m_gameContext.renderer->DrawQuad({ -100.0f, 100.0f }, { 100.0f, 100.0f }, 90.0f, Dive::Color::Green);
			//Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/crates/crate_02.png"));
		m_gameContext.renderer->DrawQuad({ 0.0f, 100.0f }, { 100.0f, 100.0f }, 135.0f, Dive::Color::Blue);
			//Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/crates/crate_03.png"));
		m_gameContext.renderer->DrawQuad({ 100.0f, 100.0f }, { 100.0f, 100.0f }, 180.0f, Dive::Color::Magenta);
			//Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/crates/crate_04.png"));
		m_gameContext.renderer->DrawQuad({ 200.0f, 100.0f }, { 100.0f, 100.0f }, 225.0f, Dive::Color::Cyan);
			//Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/crates/crate_05.png"));

		m_gameContext.renderer->DrawQuad({ -200.0f, 0.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/environment/environment_01.png"));
		m_gameContext.renderer->DrawQuad({ -100.0f, 0.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/environment/environment_02.png"));
		m_gameContext.renderer->DrawQuad({ 0.0f, 0.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/environment/environment_03.png"));
		m_gameContext.renderer->DrawQuad({ 100.0f, 0.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/environment/environment_04.png"));
		m_gameContext.renderer->DrawQuad({ 200.0f, 0.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/environment/environment_05.png"));

		m_gameContext.renderer->DrawQuad({ -200.0f, -100.0f }, { 100.0f, 100.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/player/player_01.png"));
		m_gameContext.renderer->DrawQuad({ -100.0f, -100.0f }, { 100.0f, 100.0f }, 45.0f,
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/player/player_14.png"));
		m_gameContext.renderer->DrawQuad({ 0.0f, -100.0f }, { 100.0f, 100.0f }, 90.0f,
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/player/player_03.png"));
		m_gameContext.renderer->DrawQuad({ 100.0f, -100.0f }, { 100.0f, 100.0f }, 180.0f,
				Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/player/player_17.png"));
		m_gameContext.renderer->DrawQuad({ 200.0f, -100.0f }, { 100.0f, 100.0f }, 270.0f,
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/png/Default size/player/player_05.png"));
	
		m_gameContext.renderer->EndScene();
	}

	void EditorScene::RenderUI()
	{
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
		
		static ImVec2 lastWorkSize = ImVec2(0, 0);
		static ImVec2 lastWorkPos = ImVec2(0, 0);

		if (lastWorkSize.x != viewport->WorkSize.x || lastWorkSize.y != viewport->WorkSize.y ||
			lastWorkPos.x != viewport->WorkPos.x || lastWorkPos.y != viewport->WorkPos.y)
		{
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);

			lastWorkSize = viewport->WorkSize;
			lastWorkPos = viewport->WorkPos;
		}

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		if (ImGui::Begin("SceneView", nullptr, windowFlags))
		{
			ImVec2 viewportPos = ImGui::GetCursorScreenPos();
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			auto srv = m_gameContext.renderer->GetShaderResourceView();
			ImGui::Image((ImTextureID)srv, viewportSize);
		}

		ImGui::End();

		/*
        ImGui::SetNextWindowPos({ viewport->WorkSize.x - 300.0f, viewport->WorkPos.y });
        ImGui::SetNextWindowSize({ 300.0f, viewport->WorkSize.y });

        ImGuiWindowFlags flags = //ImGuiWindowFlags_NoResize |
            //ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_HorizontalScrollbar;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 0.75f));

        if (ImGui::Begin("Editor", nullptr, flags))// ImGuiWindowFlags_NoCollapse))
        {
			// Name
			{
				std::string name = m_name;
				if (ImGui::InputText("##Name", &name))
				{
					m_name = name;
				}
			}

			// gird size
			if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto size = m_gridSize;
				Dive::ImGuiHelper::DrawVec2Control("Size", size, 10, 50.0f, "R", "C");
				SetGridSize(size);
			}

			// select tile
			if (ImGui::CollapsingHeader("Tile", ImGuiTreeNodeFlags_DefaultOpen))
			{
			}


			if (ImGui::Button("Save", {100, 30}))
			{

			}

			ImGui::SameLine();

			if (ImGui::Button("Exit", { 100, 30 }))
			{
				m_gameContext.changeScene(eSceneType::Title);
			}
        }

        ImGui::End();
        ImGui::PopStyleColor();
		*/
	}

	void EditorScene::SetGridSize(const DirectX::XMUINT2& size)
	{
		if (m_gridSize.x != size.x || m_gridSize.y != size.y)
		{
			m_gridSize = size;
			m_isGridDirty = true;
		}
	}
}