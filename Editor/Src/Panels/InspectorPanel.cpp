#include "InspectorPanel.h"
#include "HierarchyPanel.h"

static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto pBoldFont = io.Fonts->Fonts[1];

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
	ImGui::PushFont(pBoldFont);
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
	ImGui::PushFont(pBoldFont);
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
	ImGui::PushFont(pBoldFont);
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

InspectorPanel::InspectorPanel(Editor* pEditor)
	: Panel(pEditor, "Inspector")
{
	// 최소 크기 확보가 필요하다.
	// 사실 크기를 고정하는 것이 디자인하는데 더 편하다.
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// 추후 ScenePanel에서도 Selected될 수 있다.
	if (!m_pEditor->GetHierarchy())
		return;

	auto pSelectedObject = m_pEditor->GetHierarchy()->GetSeletecedObject();
	if (!pSelectedObject)
		return;

	// draw components
	drawTransform(pSelectedObject);
	drawSpriteRenderer(pSelectedObject);

	// add components
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		// 없는 Component만 가려서 추가 버튼 그리기

		ImGui::EndPopup();
	}
}

void InspectorPanel::drawTransform(Dive::GameObject* pSelectedObject)
{
	auto pTransform = pSelectedObject->GetComponent<Dive::Transform>();
	if (!pTransform)
		return;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// position
		auto position = pTransform->GetLocalPosition();
		DrawVec3Control("Position", position);
		pTransform->SetLocalPosition(position);

		// rotation
		auto rotation = pTransform->GetLocalRotationDegrees();
		DrawVec3Control("Rotation", rotation);
		pTransform->SetLocalRotation(rotation);

		// scale
		auto scale = pTransform->GetLocalScale();
		DrawVec3Control("Scale", scale, 1.0f);
		pTransform->SetLocalScale(scale);
	}

	ImGui::Separator();
}

void InspectorPanel::drawSpriteRenderer(Dive::GameObject* pSelectedObject)
{
	auto pSpriteRenderer = pSelectedObject->GetComponent<Dive::SpriteRenderable>();
	if (!pSpriteRenderer)
		return;

	auto pTexture = pSpriteRenderer->GetTexture();

	if (ImGui::CollapsingHeader("Sprite Renderable", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// enabled
		bool enabled = pSpriteRenderer->IsEnabled();
		ImGui::Checkbox("Enabled", &enabled);
		pSpriteRenderer->SetEnable(enabled);

		// Sprite
		auto name = pTexture->GetName();
		ImGui::Text("Sprite");
		ImGui::SameLine();
		ImGui::InputText("##SpriteName", &name);
		pTexture->SetName(name);
		ImGui::SameLine();
		if (ImGui::Button("*"))
		{
			// Asset/Textures 폴더를 연다.
		}

		// Color
		DirectX::XMFLOAT4 color = pSpriteRenderer->GetColor();
		float col[4] = { color.x, color.y, color.z, color.w };
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit4("##background", col);
		color = DirectX::XMFLOAT4(col[0], col[1], col[2], col[3]);
		pSpriteRenderer->SetColor(color);

		// Flip
		bool flipX = pSpriteRenderer->IsFlipX();
		bool flipY = pSpriteRenderer->IsFlipY();
		ImGui::Text("Flip");
		ImGui::SameLine();
		ImGui::Checkbox("X", &flipX);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &flipY);
		pSpriteRenderer->SetFlipX(flipX);
		pSpriteRenderer->SetFlipY(flipY);
	}

	ImGui::Separator();
}
