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
				ImGui::PushID("Tag");
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
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
				ImGui::PopID();
			}

			// Layer
			{
			}
		}

		// Transform
		if (EditorContext::Selected->HasComponent<Transform>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("TRANSFORM"))
			{
				auto transform = EditorContext::Selected->GetTransform();

				auto position = transform->GetLocalPosition();
				DrawVec3Control("Position", position, 0.0f, 150.0f);
				transform->SetLocalPosition(position);

				auto rotation = transform->GetLocalRotationDegrees();
				DrawVec3Control("Rotation", rotation, 0.0f, 150.0f);
				transform->SetLocalRotationByDegrees(rotation);

				auto scale = transform->GetLocalScale();
				DrawVec3Control("Scale", scale, 1.0f, 150.0f);
				transform->SetLocalScale(scale);
			}
		}

		// Camera
		if(EditorContext::Selected->HasComponent<Camera>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("CAMERA"))
			{
				auto camera = EditorContext::Selected->GetComponent<Camera>();

				// type

				// Field of View
				ImGui::PushID("FieldOfView");
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Field of View");
				ImGui::NextColumn();
				static int fov = static_cast<int>(camera->GetFieldOfView());
				ImGui::SliderInt("##Field of View", &fov, 1, 160);
				camera->SetFieldOfView(static_cast<float>(fov));
				ImGui::Columns(1);
				ImGui::PopID();

				ImGui::Separator();

				// Clipping Planes
				static DirectX::XMFLOAT2 clippingPlanes = { 0.0f, 0.0f };

				clippingPlanes.x = camera->GetNearClipPlane();
				clippingPlanes.y = camera->GetFarClipPlane();
				DrawVec2Control("Clipping Planes", clippingPlanes, 0.0f, 150.0f, "N", "F");
				camera->SetNearClipPlane(clippingPlanes.x);
				camera->SetFarClipPlane(clippingPlanes.y);

				ImGui::Separator();

				// Viewport Rect
				static DirectX::XMFLOAT2 topLeft = { 0.0f, 0.0f };
				static DirectX::XMFLOAT2 bottomRight = { 1.0f, 1.0f };

				camera->GetViewportRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				DrawVec2Control("Viewport Rect", topLeft, 0.0f, 150.0f);
				DrawVec2Control("", bottomRight, 1.0f, 150.0f, "W", "H");
				if (topLeft.x >= 0.0f && topLeft.x <= bottomRight.x &&
					topLeft.y >= 0.0f && topLeft.y <= bottomRight.y &&
					bottomRight.x >= topLeft.x && bottomRight.x <= 1.0f &&
					bottomRight.y >= topLeft.y && bottomRight.y <= 1.0f)
				{
					camera->SetViewportRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
				}
				ImGui::Separator();

				// Background
				ImGui::PushID("Background");
				//static float clearColor[4] = camera->GetBackgroundColor();
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Background");
				ImGui::NextColumn();
				ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit3("##Background", (float*)camera->GetBackgroundColor(), flags);
				//camera->SetBackgroundColor(ImVec4ToXMFloat4(clearColor));
				// 굳이 이렇게 두 개로 나눌 필요가 있나 하는 생각이 든다.
				// 렌더타겟까지 동일하게 설정하고 카메라의 위치만 바꾸면 충분히 가능할 것 같은데
				if (EditorContext::Selected == EditorContext::MainCamera)
					EditorContext::EditorCamera->GetComponent<Camera>()->SetBackgroundColor(camera->GetBackgroundColor());
				ImGui::Columns(1);
				ImGui::PopID();
			}
		}
		
		// mesh render
		if (EditorContext::Selected->HasComponent<MeshRenderer>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("MESH RENDERER"))
			{
				auto staticMeshRender = EditorContext::Selected->GetComponent<MeshRenderer>();

				// mesh
				{
					auto mesh = staticMeshRender->GetStaticMesh();

					ImGui::PushID("MeshName");
					std::string meshName = mesh->GetName();
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					ImGui::Text("Mesh");
					ImGui::NextColumn();
					ImGui::InputText("##MeshName", &meshName);
					mesh->SetName(meshName);
					ImGui::Columns(1);
					ImGui::PopID();
				}

				// material
				{
					auto material = staticMeshRender->GetMaterial();

					// name
					ImGui::PushID("MaterialName");
					std::string mtrlName = material->GetName();
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					ImGui::Text("Material");
					ImGui::NextColumn();
					ImGui::InputText("##MtrlName", &mtrlName);
					material->SetName(mtrlName);
					ImGui::Columns(1);
					ImGui::PopID();

					// shader program
					std::string selectedShaderName = material->GetShaderProgram()->GetName();
					const auto& programs = ShaderManager::GetAllPrograms();

					ImGui::PushID("Shader");
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					ImGui::Text("Shader");
					ImGui::NextColumn();
					if (ImGui::BeginCombo("##Shader", selectedShaderName.c_str()))
					{
						for (const auto& [name, program] : programs)
						{
							bool isSelected = (selectedShaderName == name);
							if (ImGui::Selectable(name.c_str(), isSelected))
							{
								selectedShaderName = name;
								material->SetShaderProgram(program);
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::Columns(1);
					ImGui::PopID();

					// rendering mode
					ImGui::PushID("RenderingMode");
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					ImGui::Text("Rendering Mode");
					ImGui::NextColumn();
					std::vector<const char*> renderingModes;
					renderingModes.push_back("Opaque");
					renderingModes.push_back("Transparent");
					int currentMode = static_cast<int>(material->GetRenderingMode());
					ImGui::Combo("##RenderingMode", &currentMode, renderingModes.data(), static_cast<int>(renderingModes.size()));
					material->SetRenderingMode(static_cast<eRenderingMode>(currentMode));
					ImGui::Columns(1);
					ImGui::PopID();

					// albedo
					ImGui::PushID("Albedo");
					ImVec4 albedo = XMFloat4ToImVec4(material->GetDiffuseColor());
					ImTextureID textureID = (ImTextureID)(material->GetMap(eMapType::Diffuse) ? 
						material->GetMap(eMapType::Diffuse)->GetShaderResourceView() : 0);
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					if (ImGui::ImageButton("", textureID, ImVec2(20, 20)))
					{

					}
					ImGui::SameLine();
					ImGui::Text("Albedo");
					ImGui::NextColumn();
					ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
					ImGui::ColorEdit4("##AlbedoColor", (float*)&albedo, flags);
					material->SetDiffuseColor(ImVec4ToXMFloat4(albedo));
					ImGui::Columns(1);
					ImGui::PopID();
					
					// normal map
					ImGui::PushID("NormalMap");
					textureID = (ImTextureID)(material->GetMap(eMapType::Normal) ?
						material->GetMap(eMapType::Normal)->GetShaderResourceView() : 0);
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					if (ImGui::ImageButton("", textureID, ImVec2(20, 20)))
					{

					}
					ImGui::SameLine();
					ImGui::Text("Normal Map");
					ImGui::NextColumn();
					ImGui::Columns(1);
					ImGui::PopID();
				}
			}
		}

		if (EditorContext::Selected->HasComponent<Light>())
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("LIGHT"))
			{
				auto light = EditorContext::Selected->GetComponent<Light>();

				// type
				ImGui::PushID("LightType");
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
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
				ImGui::PopID();

				if (currentType != static_cast<int>(eLightType::Directional))
				{
					// range
					ImGui::PushID("LightRange");
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150.0f);
					ImGui::Text("Range");
					ImGui::NextColumn();
					float range = light->GetRange();
					ImGui::DragFloat("##LightRange", &range, 0.1f, 0.0f, 0.0f, "%.2f");
					light->SetRange(range);
					ImGui::Columns(1);
					ImGui::PopID();

					if (currentType == static_cast<int>(eLightType::Spot))
					{
						// spot angles
						static DirectX::XMFLOAT2 spotAngles = { 0.0f, 0.0f };
						spotAngles.x = light->GetInnerAngleDegrees();
						spotAngles.y = light->GetOuterAngleDegrees();
						DrawVec2Control("Spot Angles", spotAngles, 0.0f, 150.0f, "I", "O");
						light->SetInnerAngleDegrees(spotAngles.x);
						light->SetOuterAngleDegrees(spotAngles.y);
					}
				}

				// color
				ImGui::PushID("LightColor");
				ImVec4 color = { light->GetColor().x, light->GetColor().y, light->GetColor().z, 1.0f };
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Color");
				ImGui::NextColumn(); 
				ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit3("##LightColor", (float*)&color, flags);
				light->SetColor(color.x, color.y, color.z);
				ImGui::Columns(1);
				ImGui::PopID();

				// intensity
				ImGui::PushID("LightIntensity");
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 150.0f);
				ImGui::Text("Intensity");
				ImGui::NextColumn();
				float intensity = light->GetIntensity();
				ImGui::DragFloat("##LightIntensity", &intensity, 0.1f, 0.0f, 8.0f, "%.2f");
				light->SetIntensity(intensity);
				ImGui::Columns(1);
				ImGui::PopID();
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
