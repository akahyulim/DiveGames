#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"


// 현재 선택된 GameObject의 Inspector들을 보여준다.
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
			// Camera, MeshRenderer, Light는 Enalbe Checkbox가 따로 존재해야 한다.
			// 그렇다면 GameObject와는 별도로 변수를 가진다는 이야기인데...
			showCamera(m_pInspectedTarget->GetComponent<dive::Camera>());
			showMeshRenderer(m_pInspectedTarget->GetComponent<dive::MeshRenderer>());
			showLight(m_pInspectedTarget->GetComponent<dive::Light>());
			// 이하 다른 Components

			showAddComponentButton();
		}
		// else if로 material
	}

	void Inspector::SetInspectGameObject(dive::GameObject* pTarget)
	{
		m_pInspectedTarget = pTarget;

		// hint인가 뭔가 설정
	}
	
	void Inspector::showGameObject()
	{
		if (ImGui::CollapsingHeader("GameObject", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 가져오기
			bool bActive = m_pInspectedTarget->IsActive();

			// 보여주기
			ImGui::Checkbox("##", &bActive); ImGui::SameLine();
			ImGui::Text(m_pInspectedTarget->GetName().c_str());

			ImGui::Text("Tag");	ImGui::SameLine();	ImGui::Text("Layer");

			// 설정하기
			// 현재 계층구조일 경우 하위 계층에 적용이 안되는 것 같다.
			m_pInspectedTarget->SetActive(bActive);
		}

		ImGui::Separator();
	}

	void Inspector::showTransform(dive::Transform* pTransform)
	{
		// assert가 맞나?
		if (!pTransform)
			return;

		// 이렇게 접는다. 뭐... 특별한 건 없는데 굳이 프레임 함수까지 만들어야 할까?
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 rot;
			DirectX::XMFLOAT3 scl;

			pos = pTransform->GetLocalPosition();
			rot = pTransform->GetLocalRotationEulerAngles();
			scl = pTransform->GetLocalScale();
			
			ImGui::PushItemWidth(75.0f);

			// 좀 더 깔끔히 다듬고 싶다.
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

			// 실행 중이 아닐 때 변경 가능
			{
				// 계층 구조일 경우 부모의 좌표계에서 설정되는 것이 맞다.
				pTransform->SetLocalPosition(pos);
				pTransform->SetLocalRotationEulerAngles(rot);
				pTransform->SetLocalScale(scl);
			}
		}

		ImGui::Separator();

	}

	// 아직 구현하지 않은 것이 많다.
	void Inspector::showCamera(dive::Camera* pCamera)
	{
		if (!pCamera)
			return;

		// 이게 좀 이상하다. 되다 안되다 한다.
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

				// 중앙 정렬 같은건 없나...?
				ImGui::Combo("##proejctionTypeCombe", &projectionType, "Perspective\0Orthographic");

				pCamera->SetProjectionType(static_cast<dive::eProjectionType>(projectionType));
			}

			// Field Of View
			{
				float fov = pCamera->GetFieldOfView();

				ImGui::Text("Field Of View");

				ImGui::SameLine();

				// 실제로는 min, max를 얻어와서 적용해야 한다.
				ImGui::SliderFloat("##fov", &fov, 0.0f, 180.0f);

				pCamera->SetFieldOfView(fov);
			}

			// Clipping Planes
			// 한계값이 있다면 위의 SliderFloat이 어울린다.
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
				// 그냥 Text가 아니라 박스로 둘러쌓여야 하는데...
				// InputText 같다?

				// 옆의 단추를 누르면 리소를 찾겠지?
			}
			// Occlusion Culling
			//ImGui::Text("Occlusion Culling"); ImGui::SameLine(); ImGui::Checkbox("##Shap", &bOcclusionCulling);

			// HDR
			//ImGui::Text("HDR"); ImGui::SameLine(); ImGui::Checkbox("##Nothing", &bHdr);

			// pTarget display???

			// 설정하기
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

			// 설정은 몰아서?
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
			// 적용 유무 라디오 박스가 먼저다.
			// -> 이게 바로 Behavior이다.

			{
				int type = static_cast<int>(pLight->GetType());

				ImGui::Text("Type"); 

				ImGui::SameLine();
				
				// 중앙 정렬 같은건 없나...?
				ImGui::Combo("##combo", &type, "Directional\0Spot\0Point");

				pLight->SetType(static_cast<dive::eLightType>(type));
			}

			// Range
			{
				const float f32_zero = 0.f, f32_one = 10.f;

				float range = pLight->GetRange();

				ImGui::Text("Range");

				ImGui::SameLine();

				// Transform의 좌표도 이게 나을 것 같기도...
				// 그런데 min, max가 없어도 되나?
				// label은 다 달라야 하는 것 같다.
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

				// 실제로는 min, max를 얻어와서 적용해야 한다.
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