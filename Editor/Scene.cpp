#include "Scene.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace editor
{
    Scene::Scene(Editor* editor)
        : Widget(editor)
    {
        mTitle = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        mSize = ImVec2(400, 250);
        mFlags |= ImGuiWindowFlags_NoScrollbar;
        mPadding = ImVec2(4.0f, 4.0f);
        
        m_Scene = &dive::Scene::GetGlobalScene();
        m_Renderer = &dive::Renderer::GetInstance();
    }

    void Scene::TickVisible()
    {
        // �׳� �����ϸ� �ȵȴ�.
        if (!m_Renderer)
            return;

        //m_Scene = dive::SceneManager::GetInstance().GetActiveScene();
        //if (!m_Scene)
         //   return;

        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    
        // Viewport�� ��������� �Ѵ�.
        m_Renderer->SetViewport(width, height);

        // ���� ���θ� �Ǵ��ؾ� �Ѵ�. �ƴϸ� �Ź� �����Ѵ�. �ٵ� �� ��� ��������...
        {
            m_Renderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
        }
    
        ImGui::Image(
            m_Renderer->GetFrameTexture()->GetShaderResourceView(),
            //nullptr,
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255),
            ImColor(50, 127, 166, 255)
        );
    }
}