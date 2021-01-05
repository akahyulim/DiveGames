#include "Inspector.h"



Inspector::Inspector(Editor * editor)
	: Widget(editor),
	m_selected(nullptr)
{
	m_title = std::move("Inspector");
}

void Inspector::Tick(float deltaTime)
{
	// ����� ������ �ٲٱ�
	ImGui::PushItemWidth(500);

	if (m_selected)
	{
		showGameObject();
		showTransform();

		showAddComponentButton();
	}

	ImGui::PopItemWidth();
}

void Inspector::showGameObject()
{
	if (!m_selected)
		return;

	if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// ��������
		bool bActive = m_selected->IsActive();

		// �����ֱ�
		ImGui::Checkbox("##", &bActive); ImGui::SameLine();
		ImGui::Text(m_selected->GetName().c_str());

		ImGui::Text("Tag");	ImGui::SameLine();	ImGui::Text("Layer");

		// �����ϱ�
		m_selected->SetActive(bActive);
	}

	ImGui::Separator();
}

void Inspector::showTransform()
{
	auto transform = m_selected->GetTransform();
	if(!transform)
		return;
	
	// �̷��� ���´�. ��... Ư���� �� ���µ� ���� ������ �Լ����� ������ �ұ�?
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		DirectX::XMFLOAT3 pos, scl;
		//XMFLOAT3 rot;
		DirectX::XMStoreFloat3(&pos, transform->GetLocalPosition());
		//XMStoreFloat3(&rot, pTransform->GetLocalRotation());	// �̰� ������ �ȸ�����.
		//DirectX::XMStoreFloat3(&scl, transform->GetLocalScale());

		//float startColume = ;

		// ImGui::InputFloat�� ������ �Լ���.
		auto showFloat = [](const char* id, const char* label, float* value)
		{
			float step = 0.0f;
			float step_fast = 0.0f;
			const char* decimals = "%.4f";
			auto inputTextFlags = ImGuiInputTextFlags_CharsDecimal;

			ImGui::PushItemWidth(75.0f);
			ImGui::PushID(id);
			ImGui::InputFloat(label, value, step, step_fast, decimals, inputTextFlags);
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		// ������ �� �� ����� �ٵ�� �ʹ�.
		// Position
		ImGui::Text("Pos");
		ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("PosX", "##", &pos.x);
		ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("PosY", "##", &pos.y);
		ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("PosZ", "##", &pos.z);

		// Rotation
		//ImGui::Text("Rot");
		//ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("RotX", "##", &rot.x);
		//ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("RotY", "##", &rot.y);
		//ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("RotZ", "##", &rot.z);

		// Scale
		//ImGui::Text("Scl");
		//ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("SclX", "##", &scl.x);
		//ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("SclY", "##", &scl.y);
		//ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("SclZ", "##", &scl.z);

		// ���� ���� �ƴ� �� ���� ����
		{
			// ���� ������ ��� �θ��� ��ǥ�迡�� �����Ǵ� ���� �´�.
			transform->SetLocalPosition(XMLoadFloat3(&pos));
			//pTransform->SetLocalScale(XMLoadFloat3(&scl));
			//transform->SetLocalRotation(XMLoadFloat3(&rot));
			//pTransform->SetPosition(XMLoadFloat3(&pos));
			//pTransform->SetScale(XMLoadFloat3(&scl));
		}
	}

	ImGui::Separator();
}

void Inspector::showAddComponentButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 150);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

	if (ImGui::Button("Add Component", ImVec2(300, 40)))
	{
		ImGui::OpenPopup("##ComponentContextMenu_Add");
	}
	//ComponentContextMenu_Add();
}
