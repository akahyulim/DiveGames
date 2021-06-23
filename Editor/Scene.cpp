#include "Scene.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace Editor
{
    Scene::Scene(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_size = ImVec2(400, 250);
        m_flags |= ImGuiWindowFlags_NoScrollbar;
        m_padding = ImVec2(4.0f, 4.0f);
        
        m_pScene = &dive::Scene::GetGlobalScene();
        m_pRenderer = &dive::Renderer::GetInstance();
    }

    void Scene::TickVisible()
    {
        if (!m_pRenderer)
            return;

        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    
        // Viewport�� ��������� �Ѵ�.
        m_pRenderer->SetViewport(width, height);

        // ���� ���θ� �Ǵ��ؾ� �Ѵ�. �ƴϸ� �Ź� �����Ѵ�. �ٵ� �� ��� ��������...
        {
            m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
        }
    
        ImGui::Image(
            m_pRenderer->GetFrameTexture()->GetShaderResourceView(),
            //nullptr,
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255),
            ImColor(50, 127, 166, 255)
        );
    }
}