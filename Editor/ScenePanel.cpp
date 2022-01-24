#include "ScenePanel.h"



// Renderer로부터 Image를 가져와 출력한다.
namespace Dive
{
    ScenePanel::ScenePanel(Editor* pEditor)
        : Panel(pEditor)
    {
        m_Title = "Scene";
        // 크기 설정에 의미가 있나 싶다.
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
    // 새로운 Scene를 생성하면                           //
    // 최초에는 기존 Scene 화면 출력이 유지되고 있다.    //
    //===================================================//
    void ScenePanel::TickVisible()
    {
        if (!m_pScene)
            return;

        // 그냥 리턴하면 안된다.
        // => 왜?
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
            // 여기나 생성자에서 크기를 계산한 후 GBuffer와 RenderTexture를 생성 혹은 Resize하는 함수를 호출하는 건 어떨까?   //
            // 그런데 RenderTexture를 RenderTarget으로 설정하려면 결국 해당 부분이 수정된 Shader를 사용하고,                  //
            // RenderTexture를 Shader에 전달하는 구문도 해당 Pass에 끼워 넣어야 하는데..?                                     //
            //================================================================================================================//

            // 여기에서 설정하는건 에바같다...
            //m_pRenderer->SetViewport(width, height);

            // 변경 여부를 판단해야 한다. 아니면 매번 변경한다. 근데 별 상관 없을수도...
            // 왜냐면 아래의 SetResolution() 내부에서 확인을 하므로...?
            {
                // 이게 깔끔한 방법이긴 한데... 결국은 Renderer가 Resolution을 두 개로 관리하는 형태다...
                m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
            }
        }

        // 여기에서 설정하는건 에바같다...
        // 그런데 매 프레임 갱신 되어야 한다.
        // 사실 어떤 Path에서 이게 적용되는지도 모르겠다.
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