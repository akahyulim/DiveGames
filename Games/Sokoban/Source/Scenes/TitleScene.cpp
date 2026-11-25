#include "TitleScene.h"
#include "Core/Window.h"
#include "Graphics/TextureManager.h"
#include "Rendering/Renderer2D.h"
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
	
	// 카메라는 한 곳에서만 설정하면 될 거 같다.
	void TitleScene::Initialize()
	{
		auto camera = m_gameContext.camera;// ->LookAt({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		// 예시: (x, y) 위치에 있는 2D 카메라
		camera->LookAt(
			{ 0.0f, 0.0f, -10.0f }, // eye
			{ 0.0f, 0.0f,   0.0f }, // target
			{ 0.0f, 1.0f,   0.0f }  // up (+Y가 위쪽)
		);
		camera->SetOrthographic(1280, 720, 0.1f, 100.0f);
	}
	
	void TitleScene::Cleanup()
	{
	}
	
	void TitleScene::Update(float deltaTime)
	{
	}
	
	void TitleScene::Render()
	{
		if (m_gameContext.renderer == nullptr)
			return;

		auto graphics = m_gameContext.graphics;

		m_gameContext.renderer->BeginScene(
			DirectX::XMMatrixTranspose(
				m_gameContext.camera->GetViewMatrix() *
				m_gameContext.camera->GetProjectionMatrix()));

		m_gameContext.renderer->DrawQuad({ 0.0f, 0.0f }, { 1280.0f, 720.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/dokev.jpeg"));

		m_gameContext.renderer->EndScene();
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

			//auto srv = Dive::TextureManager::Get().GetTextureViewByPath("Assets/Textures/DokeV.jpeg");
			auto srv = m_gameContext.renderer->GetShaderResourceView();
			ImGui::Image((ImTextureID)srv, viewportSize);
		}
		ImGui::End();
		*/
        ImVec2 windowSize = { 240.0f, 260.0f };
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(
            {
                viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
                viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f + 150.0f
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
				m_gameContext.changeScene(eSceneType::Play2D);
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
			if (ImGui::Button("Score Board", ImVec2(buttonWidth, buttonHeight)))
			{
				// 그냥 현재 화면에서 board를 출력??
				//m_gameContext.changeScene(eSceneType::ScoreBoard);
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