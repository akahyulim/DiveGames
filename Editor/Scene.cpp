#include "Scene.h"

// Renderer로부터 Image를 가져와 출력한다.
namespace editor
{
    Scene::Scene(Editor* editor)
        : Widget(editor)
    {
        mTitle = "Scene";
        // 크기 설정에 의미가 있나 싶다.
        mSize = ImVec2(400, 250);
        mFlags |= ImGuiWindowFlags_NoScrollbar;
        mPadding = ImVec2(4.0f, 4.0f);
        
        mScene = &dive::Scene::GetGlobalScene();
        mRenderer = &dive::Renderer::GetInstance();
    }

    void Scene::TickVisible()
    {
        if (!mRenderer)
            return;

        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
    
        // Viewport도 설정해줘야 한다.
        mRenderer->SetViewport(width, height);

        // 변경 여부를 판단해야 한다. 아니면 매번 변경한다. 근데 별 상관 없을수도...
        {
            mRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
        }
    
        ImGui::Image(
            mRenderer->GetFrameTexture()->GetShaderResourceView(),
            //nullptr,
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255),
            ImColor(50, 127, 166, 255)
        );
    }
}