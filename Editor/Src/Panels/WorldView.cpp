#include "WorldView.h"
#include "../Editor.h"

namespace Dive
{
	WorldView::WorldView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "World";

		// 유니티의 경우 카메라에서 렌더타겟을 가진다.
		// 따라서 카메라별 리소스 뷰를 가져와서 그릴 수 있다.
		// 좀 더 자세히 설명하자면 카메라의 렌더타겟이 백버퍼 바로 전에 출력되는 타겟이다.
		// 실제로 Camera.Render()가 존재하는 듯 한데 이는 카메라 기준으로 렌더링이 수행된다고 볼 수도 있다.
		// => Camera의 Render()는 자동으로 호출되나 사용자가 특정 시점에 직접 호출할 수도 있다.
		// => 이 경우 호출이 중복될 수 있는데 이를 피하는 플래그가 존재한다.
		// + Camera가 렌더타겟을 가지지 않을 수도 있다. 이 경우 백버퍼에 렌더링을 수행한다.
	}

	void WorldView::drawView()
	{
		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		auto pActiveWorld = m_pEditor->GetActiveWorld();
		if(pActiveWorld)
		{
			ImGui::PushFont(m_pEditor->GetFont(eFontTypes::Default));
			ImGui::Text("name: %s", pActiveWorld->GetName().c_str());
			ImGui::Text("size: %dx%d", static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));
			ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
			ImGui::Text("num gameObjects: %d", pActiveWorld->GetGameObjectsCount());
			ImGui::PopFont();
		}
	}
}