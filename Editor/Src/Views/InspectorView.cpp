#include "InspectorView.h"
#include "imgui_internal.h"
#include "../Editor.h"

using namespace DirectX;

namespace Dive
{
	static void DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawVec2Control(const std::string& label, DirectX::XMFLOAT2& values, float resetValue = 0.0f, float columnWidth = 100.0f, const std::string& a = "X", const std::string& b = "Y")
	{
		ImGuiIO& io = ImGui::GetIO();
		auto pBoldFont = io.Fonts->Fonts[1];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(pBoldFont);
		if (ImGui::Button(a.c_str(), buttonSize))
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
		ImGui::PushFont(pBoldFont);
		if (ImGui::Button(b.c_str(), buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

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

	static DirectX::XMFLOAT4 ImVec4ToXMFloat4(const ImVec4& value)
	{
		return { value.x, value.y, value.z, value.w };
	}

	static ImVec4 XMFloat4ToImVec4(const DirectX::XMFLOAT4& value)
	{
		return { value.x, value.y, value.z, value.w };
	}

	InspectorView::InspectorView(Editor* editor)
		: View(editor)
		, m_TagList{ "Untagged", "EditorOnly", "MainCamera", "Player" }
	{
		m_title = "Inspector";
	}

	InspectorView::~InspectorView()
	{
	}
	
	void InspectorView::renderContent()
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
				std::string goName = EditorContext::Selected->GetName();
				if (ImGui::InputText("##Name", &goName))
				{
					EditorContext::Selected->SetName(goName);
				}
			}

			// Tag
			{
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 115.0f);
				ImGui::Text("Tag");
				ImGui::NextColumn();

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

				if (ImGui::Combo("##Tag", &currentTagIndex, tagLabels.data(), static_cast<int>(tagLabels.size())))
				{
					if (currentTagIndex == static_cast<int>(tagLabels.size()) - 1)
						ImGui::OpenPopup("Add Tag");
					else
					{
						EditorContext::Selected->SetTag(m_TagList[currentTagIndex]);

						if (m_TagList[currentTagIndex] == "MainCamera")
						{
							if(EditorContext::MainCamera)
								EditorContext::MainCamera->SetTag("Untagged");
							EditorContext::MainCamera = EditorContext::Selected;
						}
					}
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
				ImGui::Columns(1);
			}

			// Layer
			{
			}
		}

		// Transform
		if (EditorContext::Selected->HasComponent<Transform>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("Transform"))
			{
				auto transform = EditorContext::Selected->GetTransform();

				auto position = transform->GetLocalPosition();
				DrawVec3Control("Position", position, 0.0f, 115.0f);
				transform->SetLocalPosition(position);

				auto rotation = transform->GetLocalRotationDegrees();
				DrawVec3Control("Rotation", rotation, 0.0f, 115.0f);
				transform->SetLocalRotationByDegrees(rotation);

				auto scale = transform->GetLocalScale();
				DrawVec3Control("Scale", scale, 1.0f, 115.0f);
				transform->SetLocalScale(scale);
			}
		}

		// Camera
		if(EditorContext::Selected->HasComponent<Camera>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("Camera"))
			{
				auto camera = EditorContext::Selected->GetComponent<Camera>();

				// type

				// Field of View
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 115.0f);
				ImGui::Text("Field of View");
				ImGui::NextColumn();
				static int fov = static_cast<int>(camera->GetFieldOfView());
				ImGui::SliderInt("##Field of View", &fov, 1, 160);
				camera->SetFieldOfView(static_cast<float>(fov));
				ImGui::Columns(1);

				ImGui::Separator();

				// Clipping Planes
				static DirectX::XMFLOAT2 clippingPlanes = { 0.0f, 0.0f };

				clippingPlanes.x = camera->GetNearClipPlane();
				clippingPlanes.y = camera->GetFarClipPlane();
				DrawVec2Control("Clipping Planes", clippingPlanes, 0.0f, 115.0f, "N", "F");
				camera->SetNearClipPlane(clippingPlanes.x);
				camera->SetFarClipPlane(clippingPlanes.y);

				ImGui::Separator();

				// Viewport Rect
				static DirectX::XMFLOAT2 topLeft = { 0.0f, 0.0f };
				static DirectX::XMFLOAT2 bottomRight = { 1.0f, 1.0f };

				camera->GetViewportRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				DrawVec2Control("Viewport Rect", topLeft, 0.0f, 115.0f);
				DrawVec2Control("", bottomRight, 1.0f, 115.0f, "W", "H");
				if (topLeft.x >= 0.0f && topLeft.x <= bottomRight.x &&
					topLeft.y >= 0.0f && topLeft.y <= bottomRight.y &&
					bottomRight.x >= topLeft.x && bottomRight.x <= 1.0f &&
					bottomRight.y >= topLeft.y && bottomRight.y <= 1.0f)
				{
					camera->SetViewportRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				}
				ImGui::Separator();

				// Background
				static ImVec4 clearColor = XMFloat4ToImVec4(camera->GetBackgroundColor());
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 115.0f);
				ImGui::Text("Background");
				ImGui::NextColumn();
				ImGui::ColorEdit3("##Background", (float*)&clearColor);
				camera->SetBackgroundColor(ImVec4ToXMFloat4(clearColor));
				ImGui::Columns(1);
			}
		}
		
		// mesh render
		if (EditorContext::Selected->HasComponent<MeshRenderer>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("Mesh Render"))
			{
				auto staticMeshRender = EditorContext::Selected->GetComponent<MeshRenderer>();

				// material
				{
					auto material = staticMeshRender->GetMaterial();

					// name
					std::string mtrlName = material->GetName();
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 115.0f);
					ImGui::Text("Material");
					ImGui::NextColumn();
					ImGui::InputText("##MtrlName", &mtrlName);
					material->SetName(mtrlName);
					ImGui::Columns(1);

					// diffuse color
					ImVec4 diffuseColor = XMFloat4ToImVec4(material->GetDiffuseColor());
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 115.0f);
					ImGui::Text("Diffuse Color");
					ImGui::NextColumn();
					ImGui::ColorEdit3("##DiffuseColor", (float*)&diffuseColor);
					material->SetDiffuseColor(ImVec4ToXMFloat4(diffuseColor));
					ImGui::Columns(1);
				}

				// mesh
				{
					auto mesh = staticMeshRender->GetStaticMesh();

					std::string meshName = mesh->GetName();
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 115.0f);
					ImGui::Text("Mesh");
					ImGui::NextColumn();
					ImGui::InputText("##MeshName", &meshName);
					mesh->SetName(meshName);
					ImGui::Columns(1);
				}
			}
		}

		if (EditorContext::Selected->HasComponent<Light>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("Light"))
			{
				auto light = EditorContext::Selected->GetComponent<Light>();

				// type
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 115.0f);
				ImGui::Text("Type");
				ImGui::NextColumn();
				std::vector<const char*> lightTypes;
				lightTypes.push_back("Direcitonal");
				lightTypes.push_back("Point");
				lightTypes.push_back("Spot");
				int currentType = static_cast<int>(light->GetLightType());
				ImGui::Combo("##LightType", &currentType, lightTypes.data(), static_cast<int>(lightTypes.size()));
				light->SetLightType(static_cast<eLightType>(currentType));
				ImGui::Columns(1);

				if (currentType != static_cast<int>(eLightType::Directional))
				{
					// range
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 115.0f);
					ImGui::Text("Range");
					ImGui::NextColumn();
					float range = light->GetRange();
					ImGui::DragFloat("##LightRange", &range, 0.1f, 0.0f, 0.0f, "%.2f");
					range = std::max(range, 0.0f); // 음수 보정
					light->SetRange(range);
					ImGui::Columns(1);

					if (currentType == static_cast<int>(eLightType::Spot))
					{
						// spot angles
						static DirectX::XMFLOAT2 spotAngles = { 0.0f, 0.0f };
						spotAngles.x = light->GetInnerAngleDegrees();
						spotAngles.y = light->GetOuterAngleDegrees();
						DrawVec2Control("Spot Angles", spotAngles, 0.0f, 115.0f, "I", "O");
						light->SetInnerAngleDegrees(spotAngles.x);
						light->SetOuterAngleDegrees(spotAngles.y);
					}
				}

				// color
				ImVec4 color = { light->GetColor().x, light->GetColor().y, light->GetColor().z, 1.0f };
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 115.0f);
				ImGui::Text("Color");
				ImGui::NextColumn();
				ImGui::ColorEdit3("##LightColor", (float*)&color);
				light->SetColor(color.x, color.y, color.z);
				ImGui::Columns(1);
			}
		}

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			//displayAddComponentEntry<CameraComponent>("Camera");

			ImGui::EndPopup();
		}
	}	
}
