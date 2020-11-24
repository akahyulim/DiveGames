#include "Scene.h"
#include "../Editor.h"


Scene::Scene(Editor* editor)
	: IWidget(editor)
{
	m_Title = "Scene";
	m_Size = DirectX::XMFLOAT2(400.0f, 250.0f);
	m_WndFlags |= ImGuiWindowFlags_NoScrollbar;
	m_Padding = DirectX::XMFLOAT2(4.0f, 4.0f);

	// shared_ptr을 리턴하도록 수정해야 한다.
	//m_Renderer = m_Editor->GetSystemManager()->GetSystem<Dive::Renderer>();
}

void Scene::Tick(float deltaTime)
{
	// Draw Target
	{
		auto width	= static_cast<unsigned int>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
		auto height = static_cast<unsigned int>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

		width -= (width % 2 != 0) ? 1 : 0;
		height -= (height % 2 != 0) ? 1 : 0;

		// 1. Renderer로부터 Texture2D를 가져와야 한다.
		// 2. 해당 Texture는 
		//		Viewport를 가져야 한다.
		//		ShaderResourceView를 가져야 한다.
		/*
		m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::Composition);

		if (m_pTexView)
		{
			if (m_pTexView->GetWidth() != width || m_pTexView->GetHeight() != height)
			{
				if (m_tickCount >= 0.250f)
				{
					m_pRenderer->ResizeRenderTextures(width, height);
					// Resize 과정에서 Texture를 재생성하므로 포인터를 다시 받아줘야 한다.
					m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::Composition);
					m_tickCount = 0.0f;
				}
				m_tickCount += deltaTime;
			}

			//const auto viewport = m_pTexView->GetViewport();
			//m_pRenderer->GetRenderDevice()->GetImmediateContext()->RSSetViewports(1, &viewport);
		}
		*/

		ImGui::Image(
			//m_pTexView ? m_pTexView->GetShaderResourceView() : nullptr,
			nullptr,
			ImVec2(static_cast<float>(width), static_cast<float>(height)),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImColor(255, 255, 255, 255),
			ImColor(50, 127, 166, 255)
		);
	}
}

