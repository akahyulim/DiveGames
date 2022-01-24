#include "ScenePanel.h"



// Renderer�κ��� Image�� ������ ����Ѵ�.
namespace Dive
{
    ScenePanel::ScenePanel(Editor* pEditor)
        : Panel(pEditor)
    {
        m_Title = "Scene";
        // ũ�� ������ �ǹ̰� �ֳ� �ʹ�.
        m_Size = ImVec2(400, 250);
        m_Flags |= ImGuiWindowFlags_NoScrollbar;
        m_Padding = ImVec2(4.0f, 4.0f);
        
        //m_pScene = &ScenePanel::GetGlobalScene();
        m_pRenderer = &Renderer::GetInstance();

        // test 
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/choa.jpg");
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/dmc5.jpg");
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/iu.jpg");
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/winter.jpg");
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/bluePrint.jpg");
        ResourceManager::GetInstance().Load<dvTexture2D>("../Assets/Textures/stars.jpg");
    }

    //===================================================//
    // ���ο� Scene�� �����ϸ�                           //
    // ���ʿ��� ���� Scene ȭ�� ����� �����ǰ� �ִ�.    //
    //===================================================//
    void ScenePanel::TickVisible()
    {
        if (!m_pScene)
            return;

        // �׳� �����ϸ� �ȵȴ�.
        // => ��?
        if (!m_pRenderer)
            return;

        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
        
        if (m_Size.x != width || m_Size.y != height)
        {
            
            //if (m_Size.x != width || m_Size.y != height)
            {
                APP_TRACE("Scene Panel Size = {0:d} x {1:d} : {2:d} x {3:d}", (int)m_Size.x, (int)m_Size.y, (int)width, (int)height);

                m_Size = ImVec2(width, height);
            }

            //================================================================================================================//
            // ���⳪ �����ڿ��� ũ�⸦ ����� �� GBuffer�� RenderTexture�� ���� Ȥ�� Resize�ϴ� �Լ��� ȣ���ϴ� �� ���?   //
            // �׷��� RenderTexture�� RenderTarget���� �����Ϸ��� �ᱹ �ش� �κ��� ������ Shader�� ����ϰ�,                  //
            // RenderTexture�� Shader�� �����ϴ� ������ �ش� Pass�� ���� �־�� �ϴµ�..?                                     //
            //================================================================================================================//

            // ���⿡�� �����ϴ°� ���ٰ���...
            //m_pRenderer->SetViewport(width, height);

            // ���� ���θ� �Ǵ��ؾ� �Ѵ�. �ƴϸ� �Ź� �����Ѵ�. �ٵ� �� ��� ��������...
            // �ֳĸ� �Ʒ��� SetResolution() ���ο��� Ȯ���� �ϹǷ�...?
            {
                // �̰� ����� ����̱� �ѵ�... �ᱹ�� Renderer�� Resolution�� �� ���� �����ϴ� ���´�...
                m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
            }
        }

        // ���⿡�� �����ϴ°� ���ٰ���...
        // �׷��� �� ������ ���� �Ǿ�� �Ѵ�.
        // ��� � Path���� �̰� ����Ǵ����� �𸣰ڴ�.
        m_pRenderer->SetViewport(width, height);

        static dvTexture2D* pSelectedTex = nullptr;
        if(!pSelectedTex)
            pSelectedTex = ResourceManager::GetInstance().GetByName<dvTexture2D>("bluePrint");

        ImGui::Image(
            //pSelectedTex ? pSelectedTex->GetShaderResourceView() :
           m_pRenderer->GetRenderTarget()->GetColorShaderResourceView(),
            //m_pRenderer->GetFrameTexture()->GetShaderResourceView(),
            //nullptr,
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255),
            ImColor(50, 127, 166, 255)
        );
    }
}