#include "WorldView.h"
#include "../Editor.h"

namespace Dive
{
	WorldView::WorldView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "World";

		// ����Ƽ�� ��� ī�޶󿡼� ����Ÿ���� ������.
		// ���� ī�޶� ���ҽ� �並 �����ͼ� �׸� �� �ִ�.
		// �� �� �ڼ��� �������ڸ� ī�޶��� ����Ÿ���� ����� �ٷ� ���� ��µǴ� Ÿ���̴�.
		// ������ Camera.Render()�� �����ϴ� �� �ѵ� �̴� ī�޶� �������� �������� ����ȴٰ� �� ���� �ִ�.
		// => Camera�� Render()�� �ڵ����� ȣ��ǳ� ����ڰ� Ư�� ������ ���� ȣ���� ���� �ִ�.
		// => �� ��� ȣ���� �ߺ��� �� �ִµ� �̸� ���ϴ� �÷��װ� �����Ѵ�.
		// + Camera�� ����Ÿ���� ������ ���� ���� �ִ�. �� ��� ����ۿ� �������� �����Ѵ�.
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