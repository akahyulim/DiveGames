#include "ProjectView.h"

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
                    DisplayFolder(entry.path());  // ��������� ���� ������ Ž��
                    ImGui::TreePop();
                }
            }
            else 
            {
                ImGui::BulletText("%s", entry.path().stem().string().c_str());
            }
        }
    }

	ProjectView::ProjectView()
	{
		m_Title = "Project Browser";
	}

	ProjectView::~ProjectView()
	{
	}

	void ProjectView::drawView()
	{
        DisplayFolder(Editor::GetInstance().GetProjectDir());
    }
}