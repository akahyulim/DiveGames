#include "ProjectView.h"
#include "../Editor.h"

namespace Dive
{
    void DisplayFolder(const std::filesystem::path& path) 
    {
        if (path.empty())
            return;

        for (const auto& entry : std::filesystem::directory_iterator(path)) 
        {
            if (entry.is_directory()) 
            {
                if (ImGui::TreeNode(entry.path().stem().string().c_str())) 
                {
                    DisplayFolder(entry.path());  // 재귀적으로 하위 폴더를 탐색
                    ImGui::TreePop();
                }
            }
            else 
            {
                ImGui::BulletText("%s", entry.path().stem().string().c_str());
            }
        }
    }

	ProjectView::ProjectView(Editor* pEditor)
		: Panel(pEditor)
	{
		m_Title = "Project";
	}

	ProjectView::~ProjectView()
	{
	}

	void ProjectView::drawView()
	{
        DisplayFolder(m_pEditor->GetProjectDir());
    }
}