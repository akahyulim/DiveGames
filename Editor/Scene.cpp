#include "Scene.h"

// Renderer로부터 Image를 가져와 출력한다.
namespace editor
{
    Scene::Scene(Editor* editor)
        : Widget(editor)
    {
        m_Title = "Scene";
        // 크기 설정에 의미가 있나 싶다.
        m_Size = ImVec2(400, 250);
        m_Flags |= ImGuiWindowFlags_NoScrollbar;
        m_Padding = ImVec2(4.0f, 4.0f);
        
        //m_Scene = &dive::Scene::GetGlobalScene();
        m_Renderer = &dive::Renderer::GetInstance();
    }

    //===================================================//
    // 새로운 Scene를 생성하면                           //
    // 최초에는 기존 Scene 화면 출력이 유지되고 있다.    //
    //===================================================//
    void Scene::TickVisible()
    {
        if (!m_Scene)
            return;

        // 그냥 리턴하면 안된다.
        // => 왜?
        if (!m_Renderer)
            return;

        //if(dive::SceneManager::GetInstance().GetActiveScene())
        //   m_Scene = dive::SceneManager::GetInstance().GetActiveScene();

        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    
        // Viewport도 설정해줘야 한다.
        m_Renderer->SetViewport(width, height);

        // 변경 여부를 판단해야 한다. 아니면 매번 변경한다. 근데 별 상관 없을수도...
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