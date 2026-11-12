#include "TitleScene.h"
#include "Core/Window.h"
#include "Graphics/TextureManager.h"
#include <imgui.h>

namespace Sokoban
{
	TitleScene::TitleScene(const GameContext& context)
        : Scene(context)
	{
	}

	TitleScene::~TitleScene()
	{
	}
	
	void TitleScene::Initialize()
	{
        Dive::TextureManager::Get().LoadTexture("Assets/Textures/dmc.jpg");
	}
	
	void TitleScene::Cleanup()
	{
	}
	
	void TitleScene::Update(float deltaTime)
	{
	}
	
	void TitleScene::Render()
	{
	}
	
	void TitleScene::RenderUI()
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
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoInputs;

		if (ImGui::Begin("SceneView", nullptr, windowFlags))
		{
			ImVec2 viewportPos = ImGui::GetCursorScreenPos();
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			auto srv = Dive::TextureManager::Get().GetTextureViewByPath("Assets/Textures/dmc.jpg");
			ImGui::Image((ImTextureID)srv, viewportSize);
		}
		ImGui::End();

        ImVec2 windowSize = { 240.0f, 220.0f };
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(
            {
                viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
                viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f
            },
            ImGuiCond_Always
        );

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollbar;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 9.0f));

        if (ImGui::Begin("##Title Menu", nullptr, flags))
        {
            float buttonWidth = 202.5f;
            float buttonHeight = 39.0f;
            float windowWidth = ImGui::GetWindowSize().x;

            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button("Start", ImVec2(buttonWidth, buttonHeight)))
            {
				m_gameContext.changeScene(eSceneType::Play);
            }

            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button("Load", ImVec2(buttonWidth, buttonHeight)))
            {
            }

            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button("Editor", ImVec2(buttonWidth, buttonHeight)))
            {
				m_gameContext.changeScene(eSceneType::Editor);
            }

            ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
            if (ImGui::Button("Quit", ImVec2(buttonWidth, buttonHeight)))
            {
                m_gameContext.window->Close();
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
	}
}