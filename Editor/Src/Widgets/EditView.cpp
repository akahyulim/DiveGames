#include "EditView.h"


EditView::EditView(Editor * editor)
	: Widget(editor),
	m_graphics(nullptr),
	m_texture(nullptr)
{
	m_title = "Edit View";
	m_size = DirectX::XMFLOAT2(400.0f, 250.0f);
	m_wndFlags |= ImGuiWindowFlags_NoScrollbar;
	m_padding = DirectX::XMFLOAT2(4.0f, 4.0f);

	m_graphics = m_editor->GetContext()->GetSubsystem<Graphics>().get();
}

EditView::~EditView()
{
}

void EditView::Tick(float deltaTime)
{
	if (!m_graphics)
		return;

	drawTexture(deltaTime);
}

void EditView::drawTexture(float deltaTime)
{
	auto width = static_cast<unsigned int>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<unsigned int>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	width -= (width % 2 != 0) ? 1 : 0;
	height -= (height % 2 != 0) ? 1 : 0;

	//m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::GBuffer_Albedo);
	//m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::Light_Diffuse);
	//m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::Composition);
	m_texture = m_graphics->GetEditorTexture();

	if (m_texture)
	{
		//if (m_texture->GetWidth() != width || m_texture->GetHeight() != height)
		{
			//if (m_tickCount >= 0.250f)
			{
				//m_pRenderer->ResizeRenderTextures(width, height);
				// Resize 과정에서 Texture를 재생성하므로 포인터를 다시 받아줘야 한다.
				//m_pTexView = m_pRenderer->GetRenderTextures(Dive::eRenderTargetType::Composition);
				//m_tickCount = 0.0f;
			}
			//m_tickCount += deltaTime;
		}

		//const auto viewport = m_pTexView->GetViewport();
		//m_pRenderer->GetRenderDevice()->GetImmediateContext()->RSSetViewports(1, &viewport);
	}

	ImGui::Image(
		m_texture ? m_texture->GetShaderResourceView() : nullptr,
		ImVec2(static_cast<float>(width), static_cast<float>(height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
