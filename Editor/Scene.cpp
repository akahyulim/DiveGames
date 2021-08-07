#include "Scene.h"

// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace editor
{
    Scene::Scene(Editor* pEditor)
        : Widget(pEditor)
    {
        m_Title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_Size = ImVec2(400, 250);
        m_Flags |= ImGuiWindowFlags_NoScrollbar;
        m_Padding = ImVec2(4.0f, 4.0f);
        
        //m_pScene = &dive::Scene::GetGlobalScene();
        m_pRenerer = &dive::Renderer::GetInstance();
    }

    //===================================================//
    // ���ο� Scene�� �����ϸ�                           //
    // ���ʿ��� ���� Scene ȭ�� ����� �����ǰ� �ִ�.    //
    //===================================================//
    void Scene::TickVisible()
    {
        if (!m_pScene)
            return;

        // �׳� �����ϸ� �ȵȴ�.
        // => ��?
        if (!m_pRenerer)
            return;

        
        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    
        //================================================================================================================//
        // ���⳪ �����ڿ��� ũ�⸦ ����� �� GBuffer�� RenderTexture�� ���� Ȥ�� Resize�ϴ� �Լ��� ȣ���ϴ� �� ���?   //
        // �׷��� RenderTexture�� RenderTarget���� �����Ϸ��� �ᱹ �ش� �κ��� ������ Shader�� ����ϰ�,                  //
        // RenderTexture�� Shader�� �����ϴ� ������ �ش� Pass�� ���� �־�� �ϴµ�..?                                     //
        //================================================================================================================//

        // Viewport�� ��������� �Ѵ�.
        m_pRenerer->SetViewport(width, height);

        // ���� ���θ� �Ǵ��ؾ� �Ѵ�. �ƴϸ� �Ź� �����Ѵ�. �ٵ� �� ��� ��������...
        {
            m_pRenerer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
        }
    
        ImGui::Image(
            m_pRenerer->GetFrameTexture()->GetShaderResourceView(),
            //nullptr,
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255),
            ImColor(50, 127, 166, 255)
        );
    }
}