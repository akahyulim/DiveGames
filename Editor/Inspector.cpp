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
			// Camera, MeshRenderer, Light�� Enalbe Checkbox�� ���� �����ؾ� �Ѵ�.
			// �׷��ٸ� GameObject�ʹ� ������ ������ �����ٴ� �̾߱��ε�...
			showCamera(m_pInspectedTarget->GetComponent<dive::Camera>());
			showMeshRenderer(m_pInspectedTarget->GetComponent<dive::MeshRenderer>());
			showLight(m_pInspectedTarget->GetComponent<dive::Light>());
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
			// ���� ���������� ��� ���� ������ ������ �ȵǴ� �� ����.
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
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 rot;
			DirectX::XMFLOAT3 scl;

			pos = pTransform->GetLocalPosition();
			rot = pTransform->GetLocalRotationEulerAngles();
			scl = pTransform->GetLocalScale();
			
			ImGui::PushItemWidth(75.0f);

			// �� �� ����� �ٵ�� �ʹ�.
			// Position
			ImGui::Text("Pos");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();	 
			ImGui::DragScalar("##posX", ImGuiDataType_Float, &pos.x, 0.01f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();	 
			ImGui::DragScalar("##posY", ImGuiDataType_Float, &pos.y, 0.01f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
			ImGui::DragScalar("##posZ", ImGuiDataType_Float, &pos.z, 0.01f, nullptr, nullptr, "%.4f");

			// Rotation
			ImGui::Text("Rot");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();
			ImGui::DragScalar("##rotX", ImGuiDataType_Float, &rot.x, 1.0f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();
			ImGui::DragScalar("##rotY", ImGuiDataType_Float, &rot.y, 1.0f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
			ImGui::DragScalar("##rotZ", ImGuiDataType_Float, &rot.z, 1.0f, nullptr, nullptr, "%.4f");

			// Scale
			ImGui::Text("Scl");
			ImGui::SameLine();	ImGui::Text("X");	ImGui::SameLine();
			ImGui::DragScalar("##sclX", ImGuiDataType_Float, &scl.x, 0.01f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Y");	ImGui::SameLine();
			ImGui::DragScalar("##sclY", ImGuiDataType_Float, &scl.y, 0.01f, nullptr, nullptr, "%.4f");
			ImGui::SameLine();	ImGui::Text("Z");	ImGui::SameLine();
			ImGui::DragScalar("##sclZ", ImGuiDataType_Float, &scl.z, 0.01f, nullptr, nullptr, "%.4f");

			ImGui::PopItemWidth();

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
			// Clear Flags
			{
				ImGui::Text("Clear Flags");
			}

			// Backgound Color
			{
				DirectX::XMFLOAT4 color = pCamera->GetBackgroundColor();
				float col[4] = { color.x, color.y, color.z, color.w };

				ImGui::Text("Background");

				ImGui::SameLine();

				ImGui::ColorEdit4("##background", col);

				color = DirectX::XMFLOAT4(col[0], col[1], col[2], col[3]);
				pCamera->SetBackgroundColor(color);
			}

			// Culling Mask
			{
				ImGui::Text("Culling Mask");
			}

			// Projection
			{
				int projectionType = static_cast<int>(pCamera->GetProjectionType());

				ImGui::Text("Projection");
				ImGui::SameLine();

				// �߾� ���� ������ ����...?
				ImGui::Combo("##proejctionTypeCombe", &projectionType, "Perspective\0Orthographic");

				pCamera->SetProjectionType(static_cast<dive::eProjectionType>(projectionType));
			}

			// Field Of View
			{
				float fov = pCamera->GetFieldOfView();

				ImGui::Text("Field Of View");

				ImGui::SameLine();

				// �����δ� min, max�� ���ͼ� �����ؾ� �Ѵ�.
				ImGui::SliderFloat("##fov", &fov, 0.0f, 180.0f);

				pCamera->SetFieldOfView(fov);
			}

			// Clipping Planes
			// �Ѱ谪�� �ִٸ� ���� SliderFloat�� ��︰��.
			{
				ImGui::Text("Clipping Planes");	ImGui::SameLine();
				float nearPlane = pCamera->GetNearPlane();
				ImGui::Text("Near"); ImGui::SameLine();	ImGui::DragScalar("##nearPlane", ImGuiDataType_Float, &nearPlane, 0.1f, nullptr, nullptr, "%.2f");
				float farPlane = pCamera->GetFarPlane();
				ImGui::Text("Far");	ImGui::SameLine();	ImGui::DragScalar("##farPlane", ImGuiDataType_Float, &farPlane, 0.1f, nullptr, nullptr, "%.2f");

				pCamera->SetNearPlane(nearPlane);
				pCamera->SetFarPlane(farPlane);
			}

			// Viewport
			{
				ImGui::Text("Viewport Rect");

				auto screenRect = pCamera->GetScreenRect();
				float x = screenRect.x;
				float y = screenRect.y;
				float width = screenRect.width;
				float height = screenRect.height;

				ImGui::Text("X");	ImGui::SameLine();	ImGui::SliderFloat("##x", &x, 0.0f, 1.0f);	//ImGui::SameLine();
				ImGui::Text("Y");	ImGui::SameLine();	ImGui::SliderFloat("##y", &y, 0.0f, 1.0f);
				ImGui::Text("W");	ImGui::SameLine();	ImGui::SliderFloat("##width", &width, 0.0f, 1.0f);	//ImGui::SameLine();
				ImGui::Text("H");	ImGui::SameLine();	ImGui::SliderFloat("##height", &height, 0.0f, 1.0f);

				pCamera->SetScreenRect(x, y, width, height);
			}

			
			// Detph	
			ImGui::Text("Depth");

			// rendering path???

			// Target Texture
			{
				auto pTargetTexture = pCamera->GetTargetTexture();

				ImGui::Text("Target Texture"); //ImGui::SameLine();
				// �׳� Text�� �ƴ϶� �ڽ��� �ѷ��׿��� �ϴµ�...
				// InputText ����?

				// ���� ���߸� ������ ���Ҹ� ã����?
			}
			// Occlusion Culling
			//ImGui::Text("Occlusion Culling"); ImGui::SameLine(); ImGui::Checkbox("##Shap", &bOcclusionCulling);

			// HDR
			//ImGui::Text("HDR"); ImGui::SameLine(); ImGui::Checkbox("##Nothing", &bHdr);

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

	void Inspector::showLight(dive::Light* pLight)
	{
		if (!pLight)
			return;

		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ���� ���� ���� �ڽ��� ������.
			// -> �̰� �ٷ� Behavior�̴�.

			{
				int type = static_cast<int>(pLight->GetType());

				ImGui::Text("Type"); 

				ImGui::SameLine();
				
				// �߾� ���� ������ ����...?
				ImGui::Combo("##combo", &type, "Directional\0Spot\0Point");

				pLight->SetType(static_cast<dive::eLightType>(type));
			}

			// Range
			{
				const float f32_zero = 0.f, f32_one = 10.f;

				float range = pLight->GetRange();

				ImGui::Text("Range");

				ImGui::SameLine();

				// Transform�� ��ǥ�� �̰� ���� �� ���⵵...
				// �׷��� min, max�� ��� �ǳ�?
				// label�� �� �޶�� �ϴ� �� ����.
				ImGui::DragScalar("##drag float", ImGuiDataType_Float, &range, 0.05f, &f32_zero, nullptr, "%.2f");

				pLight->SetRange(range);
			}

			// Color
			{
				DirectX::XMFLOAT4 color = pLight->GetColor();
				float col[4] = { color.x, color.y, color.z, color.w };

				ImGui::Text("Color");

				ImGui::SameLine();

				ImGui::ColorEdit4("##color", col);

				color = DirectX::XMFLOAT4(col[0], col[1], col[2], col[3]);
				pLight->SetColor(color);
			}
		
			// Intensity
			{
				float intensity = pLight->GetIntensity();

				ImGui::Text("Intensity");

				ImGui::SameLine();

				// �����δ� min, max�� ���ͼ� �����ؾ� �Ѵ�.
				ImGui::SliderFloat("##Intensity", &intensity, 0.0f, 10.0f);

				pLight->SetIntensity(intensity);
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
			//ImGui::OpenPopup("##ComponentContextMenu_Add");
		}
		//ComponentContextMenu_Add();
	}
}