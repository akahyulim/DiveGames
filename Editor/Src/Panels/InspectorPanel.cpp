#include "InspectorPanel.h"
#include "HierarchyPanel.h"

// ������ ���� ������ �޾Ҵ�.
// ��, c#ó�� ������ public���� ����� �����Ѵٴ� �Ҹ���.
static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	// ���� ��Ʈ ������ �ȵȴ�.
	auto pBoldFont = io.Fonts->Fonts[0];

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
	// �ּ� ũ�� Ȯ���� �ʿ��ϴ�.
	// ��� ũ�⸦ �����ϴ� ���� �������ϴµ� �� ���ϴ�.
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// ���� ScenePanel������ Selected�� �� �ִ�.
	if (!m_pEditor->GetHierarchy())
		return;

	auto pSelectedObject = m_pEditor->GetHierarchy()->GetSeletecedObject();
	if (!pSelectedObject)
		return;

	// draw components
	drawTransform(pSelectedObject);

	// add components
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		// ���� Component�� ������ �߰� ��ư �׸���

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
		DrawVec3Control("Position", pTransform->m_LocalPosition);
		DrawVec3Control("Rotation", pTransform->m_LocalRotation);
		DrawVec3Control("Scale", pTransform->m_LocalScale);

		/*
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 rot;
		DirectX::XMFLOAT3 scl;

		pos = pTransform->GetLocalPosition();
		//rot = pTransform->GetLocalRotationEulerAngles();
		//scl = pTransform->GetLocalScale();
		rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		scl = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		ImGui::PushItemWidth(75.0f);

		// �� �� ����� �ٵ�� �ʹ�.
		// Position
		ImGui::Text("Pos");
		ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();
		ImGui::DragScalar("##posX", ImGuiDataType_Float, &pos.x, 0.01f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();
		ImGui::DragScalar("##posY", ImGuiDataType_Float, &pos.y, 0.01f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
		ImGui::DragScalar("##posZ", ImGuiDataType_Float, &pos.z, 0.01f, nullptr, nullptr, "%.2f");

		// Rotation
		ImGui::Text("Rot");
		ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();
		ImGui::DragScalar("##rotX", ImGuiDataType_Float, &rot.x, 1.0f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();
		ImGui::DragScalar("##rotY", ImGuiDataType_Float, &rot.y, 1.0f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
		ImGui::DragScalar("##rotZ", ImGuiDataType_Float, &rot.z, 1.0f, nullptr, nullptr, "%.2f");

		// Scale
		ImGui::Text("Scl");
		ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();
		ImGui::DragScalar("##sclX", ImGuiDataType_Float, &scl.x, 0.01f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();
		ImGui::DragScalar("##sclY", ImGuiDataType_Float, &scl.y, 0.01f, nullptr, nullptr, "%.2f");
		ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
		ImGui::DragScalar("##sclZ", ImGuiDataType_Float, &scl.z, 0.01f, nullptr, nullptr, "%.2f");

		ImGui::PopItemWidth();

		// ���� ���� �ƴ� �� ���� ����??
		{
			// ���� ������ ��� �θ��� ��ǥ�迡�� �����Ǵ� ���� �´�.
			pTransform->SetLocalPosition(pos);
			//pTransform->SetLocalRotationEulerAngles(rot);
			//pTransform->SetLocalScale(scl);
		}
		*/
	}
	ImGui::Separator();
}
