#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"


// ���� ���õ� GameObject�� Inspector���� �����ش�.
namespace editor
{
	dive::GameObject* Inspector::m_pInspectedTarget = nullptr;

	Inspector::Inspector(Editor* pEditor)
		: Widget(pEditor)
	{
		m_Title = "Inspector";
	}

	void Inspector::TickVisible()
	{
		if (m_pInspectedTarget)
		{
			showGameObject();

			showTransform(m_pInspectedTarget->GetTransform());
			showCamera(m_pInspectedTarget->GetComponent<dive::Camera>());
			showMeshRenderer(m_pInspectedTarget->GetComponent<dive::MeshRenderer>());
			// ���� �ٸ� Components

			showAddComponentButton();
		}
		// else if�� material
	}

	void Inspector::SetInspectGameObject(dive::GameObject* pTarget)
	{
		m_pInspectedTarget = pTarget;

		// hint�ΰ� ���� ����
	}
	
	void Inspector::showGameObject()
	{
		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ��������
			bool bActive = m_pInspectedTarget->IsActive();

			// �����ֱ�
			ImGui::Checkbox("##", &bActive); ImGui::SameLine();
			ImGui::Text(m_pInspectedTarget->GetName().c_str());

			ImGui::Text("Tag");	ImGui::SameLine();	ImGui::Text("Layer");

			// �����ϱ�
			m_pInspectedTarget->SetActive(bActive);
		}

		ImGui::Separator();
	}

	void Inspector::showTransform(dive::Transform* pTransform)
	{
		// assert�� �³�?
		if (!pTransform)
			return;

		// �̷��� ���´�. ��... Ư���� �� ���µ� ���� ������ �Լ����� ������ �ұ�?
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			float posX, posY, posZ;
			float degreeX, degreeY, degreeZ;
			float sclX, sclY, sclZ;

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 rot;
			DirectX::XMFLOAT3 scl;

			pos = pTransform->GetLocalPosition();
			rot = pTransform->GetLocalRotationEulerAngles();
			scl = pTransform->GetLocalScale();
			
			// ImGui::InputFloat�� ������ �Լ���.
			auto showFloat = [](const char* pID, const char* pLabel, float* pValue)
			{
				float step = 0.0f;
				float step_fast = 0.0f;
				const char* pDecimals = "%.4f";
				auto inputTextFlags = ImGuiInputTextFlags_CharsDecimal;

				ImGui::PushItemWidth(75.0f);
				ImGui::PushID(pID);
				ImGui::InputFloat(pLabel, pValue, step, step_fast, pDecimals, inputTextFlags);
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
				pTransform->SetLocalPosition(pos);
				pTransform->SetLocalRotationEulerAngles(rot);
				pTransform->SetLocalScale(scl);
			}
		}

		ImGui::Separator();

	}

	// ���� �������� ���� ���� ����.
	void Inspector::showCamera(dive::Camera* pCamera)
	{
		if (!pCamera)
			return;

		// �̰� �� �̻��ϴ�. �Ǵ� �ȵǴ� �Ѵ�.
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ��������
			bool bEnabled = false;// pCamera->Enabled();
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

			// pTarget display???

			// �����ϱ�
			//pCamera->SetEnable(bEnabled);
		}

		ImGui::Separator();
	}

	void Inspector::showMeshRenderer(dive::MeshRenderer* pMeshRenderer)
	{
		if (!pMeshRenderer)
			return;

		if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto meshName = pMeshRenderer->GetMesh()->GetName();

			ImGui::Text("Mesh");	ImGui::SameLine();	ImGui::InputText("##Edit", &meshName);	
			ImGui::Text("Cast Shadows"); //on / off ? ?
			bool bReceiveShadows = pMeshRenderer->IsReceiveShadows();
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
			pMeshRenderer->SetReceiveShadows(bReceiveShadows);
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