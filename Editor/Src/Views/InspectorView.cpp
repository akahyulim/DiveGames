#include "InspectorView.h"
#include "imgui_internal.h"
#include "../Editor.h"

namespace Dive
{
	static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	InspectorView::InspectorView(Editor* editor)
		: View(editor)
		, m_TagList{ "Untagged", "EditorOnly", "MainCamera", "Player" }
	{
		m_Title = "Inspector";
	}

	InspectorView::~InspectorView()
	{
	}
	
	void InspectorView::drawView()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::Selected)
			return;

		// Base Info
		{
			// Active
			{
				bool isActive = EditorContext::Selected->IsActiveSelf();
				if (ImGui::Checkbox("##Active", &isActive))
				{
					EditorContext::Selected->SetActive(isActive);
				}
			}

			ImGui::SameLine();

			// Name
			{
				char buffer[256]{};
				auto& name = EditorContext::Selected->GetName();
				strncpy_s(buffer, sizeof(buffer), name.c_str(), sizeof(buffer));
				if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
				{
					EditorContext::Selected->SetName(std::string(buffer));
				}
			}

			// Tag
			{
				const std::string& currentTag = EditorContext::Selected->GetTag();
				if (std::find(m_TagList.begin(), m_TagList.end(), currentTag) == m_TagList.end())
					m_TagList.push_back(currentTag);

				std::vector<const char*> tagLabels;
				for (const auto& tag : m_TagList)
					tagLabels.push_back(tag.c_str());
				tagLabels.push_back("Add Tag...");

				static int currentTagIndex = 0;
				for (size_t i = 0; i < m_TagList.size(); ++i)
				{
					if (currentTag == m_TagList[i])
					{
						currentTagIndex = static_cast<int>(i);
						break;
					}
				}

				if (ImGui::Combo("Tag", &currentTagIndex, tagLabels.data(), static_cast<int>(tagLabels.size())))
				{
					if (currentTagIndex == static_cast<int>(tagLabels.size()) - 1)
						ImGui::OpenPopup("Add Tag");
					else
						EditorContext::Selected->SetTag(m_TagList[currentTagIndex]);
				}

				if (ImGui::BeginPopup("Add Tag"))
				{
					static char newTagBuffer[64]{};

					ImGui::InputText("New Tag", newTagBuffer, IM_ARRAYSIZE(newTagBuffer));

					if (ImGui::Button("Add") && strlen(newTagBuffer) > 0)
					{
						std::string newTag = newTagBuffer;

						if (std::find(m_TagList.begin(), m_TagList.end(), newTag) == m_TagList.end())
						{
							m_TagList.push_back(newTag);
							EditorContext::Selected->SetTag(newTag);
							currentTagIndex = static_cast<int>(m_TagList.size()) - 1;
						}

						newTagBuffer[0] = '\0';
						ImGui::CloseCurrentPopup();
					}

					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
					{
						newTagBuffer[0] = '\0';
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			// Layer
			{
			}
		}

		// Transform
		if (EditorContext::Selected->HasComponent<Transform>())
		{
			ImGui::Separator();

			auto tc = EditorContext::Selected->GetTransform();

			auto position = tc->GetLocalPosition();
			DrawVec3Control("Position", position);
			tc->SetLocalPosition(position);

			auto rotation = Math::QuaternionToDegrees(tc->GetLocalRotation());
			DrawVec3Control("Rotation", rotation);
			tc->SetLocalRotation(Math::DegreesToQuaternion(rotation));

			auto scale = tc->GetLocalScale();
			DrawVec3Control("Scale", scale, 1.0f);
			tc->SetLocalScale(scale);
		}

		ImGui::Separator();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			//displayAddComponentEntry<CameraComponent>("Camera");

			ImGui::EndPopup();
		}
	}	
}
