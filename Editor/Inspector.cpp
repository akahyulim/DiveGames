#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"


// ���� ���õ� GameObject�� Inspector���� �����ش�.
namespace editor
{
	dive::GameObject* Inspector::m_InspectedTarget = nullptr;

	Inspector::Inspector(Editor* editor)
		: Widget(editor)
	{
		m_Title = "Inspector";
	}

	void Inspector::TickVisible()
	{
		if (m_InspectedTarget)
		{
			showGameObject();

			showTransform(m_InspectedTarget->GetTransform());
			showCamera(m_InspectedTarget->GetComponent<dive::Camera>());
			showMeshRenderer(m_InspectedTarget->GetComponent<dive::MeshRenderer>());
			// ���� �ٸ� Components

			showAddComponentButton();
		}
		// else if�� material
	}

	void Inspector::SetInspectGameObject(dive::GameObject* target)
	{
		m_InspectedTarget = target;

		// hint�ΰ� ���� ����
	}
	
	void Inspector::showGameObject()
	{
		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ��������
			bool bActive = m_InspectedTarget->IsActive();

			// �����ֱ�
			ImGui::Checkbox("##", &bActive); ImGui::SameLine();
			ImGui::Text(m_InspectedTarget->GetName().c_str());

			ImGui::Text("Tag");	ImGui::SameLine();	ImGui::Text("Layer");

			// �����ϱ�
			m_InspectedTarget->SetActive(bActive);
		}

		ImGui::Separator();
	}

	void Inspector::showTransform(dive::Transform* transform)
	{
		// assert�� �³�?
		if (!transform)
			return;

		// �̷��� ���´�. ��... Ư���� �� ���µ� ���� ������ �Լ����� ������ �ұ�?
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 pos, scl;
			DirectX::XMFLOAT3 rot;
			pos = transform->GetLocalPositionFloat3();
			rot = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	// ��� degree�� ��� �� �����Ǿ�� �Ѵ�.
			scl = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

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

			// �� �� ����� �ٵ�� �ʹ�.
			// Position
			ImGui::Text("Pos");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("PosX", "##", &pos.x);
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("PosY", "##", &pos.y);
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("PosZ", "##", &pos.z);

			// Rotation
			ImGui::Text("Rot");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("RotX", "##", &rot.x);
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("RotY", "##", &rot.y);
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("RotZ", "##", &rot.z);

			// Scale
			ImGui::Text("Scl");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 showFloat("SclX", "##", &scl.x);
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 showFloat("SclY", "##", &scl.y);
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();	 showFloat("SclZ", "##", &scl.z);

			// ���� ���� �ƴ� �� ���� ����
			{
				// ���� ������ ��� �θ��� ��ǥ�迡�� �����Ǵ� ���� �´�.
				transform->SetLocalPositionByFloat3(pos);
				//pTransform->SetLocalScale(XMLoadFloat3(&scl));
			//	pTransform->SetLocalRotation(XMLoadFloat3(&rot));
				//pTransform->SetPosition(XMLoadFloat3(&pos));
				//pTransform->SetScale(XMLoadFloat3(&scl));
			}
		}

		ImGui::Separator();

	}

	// ���� �������� ���� ���� ����.
	void Inspector::showCamera(dive::Camera* camera)
	{
		if (!camera)
			return;

		// �̰� �� �̻��ϴ�. �Ǵ� �ȵǴ� �Ѵ�.
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ��������
			bool bEnabled = false;// camera->Enabled();
			bool bOcclusionCulling = false;
			bool bHdr = false;

			// �����ֱ� 
			// enabled
			ImGui::Text("Enable"); ImGui::SameLine(); ImGui::Checkbox("##Nothing", &bEnabled);

			// Clear Flags
			ImGui::Text("Clear Flags");

			// Backgound Color
			ImGui::Text("Background Color");

			// Culling Mask
			ImGui::Text("Culling Mask");

			// Projection
			ImGui::Text("Projection");

			// Field Of View
			ImGui::Text("Field Of View");

			// Clipping Planes
			ImGui::Text("Clipping Planes");
			ImGui::SameLine();	ImGui::Text("Near");
			ImGui::Text("Far");

			// Viewport
			ImGui::Text("Viewport");
			ImGui::Text("X");	ImGui::SameLine();	ImGui::Text("Y");
			ImGui::Text("W");	ImGui::SameLine();	ImGui::Text("H");

			// Detph	
			ImGui::Text("Depth");

			// rendering path???

			// Target Texture
			ImGui::Text("Target Texture");

			// Occlusion Culling
			ImGui::Text("Occlusion Culling"); ImGui::SameLine(); ImGui::Checkbox("##Shap", &bOcclusionCulling);

			// HDR
			ImGui::Text("HDR"); ImGui::SameLine(); ImGui::Checkbox("##Nothing", &bHdr);

			// target display???

			// �����ϱ�
			//pCamera->SetEnable(bEnabled);
		}

		ImGui::Separator();
	}

	void Inspector::showMeshRenderer(dive::MeshRenderer* meshRenderer)
	{
		if (!meshRenderer)
			return;

		if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto meshName = meshRenderer->GetMesh()->GetName();

			ImGui::Text("Mesh");	ImGui::SameLine();	ImGui::InputText("##Edit", &meshName);	
			ImGui::Text("Cast Shadows"); //on / off ? ?
			bool bReceiveShadows = meshRenderer->IsReceiveShadows();
			ImGui::Text("Receive Shaodws");	ImGui::SameLine(); ImGui::Checkbox("##Receive Shadows", &bReceiveShadows);
			// Materials
			if (ImGui::TreeNode("Materials"))
			{
				ImGui::TreePop();
				ImGui::Separator();
			}
			// Use Light Probes - radio button
			// Replection Probes 
			// Anchor Override

			// ������ ���Ƽ�?
			meshRenderer->SetReceiveShadows(bReceiveShadows);
		}

		ImGui::Separator();
	}
	
	void Inspector::showAddComponentButton()
	{
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 150);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		if (ImGui::Button("Add Component", ImVec2(300, 40)))
		{
			//ImGui::OpenPopup("##ComponentContextMenu_Add");
		}
		//ComponentContextMenu_Add();
	}
}