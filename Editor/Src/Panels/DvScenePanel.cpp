#include "DvScenePanel.h"

namespace Editor
{
#include "ImGuizmo/ImGuizmo.h"

	DvScenePanel::DvScenePanel(DvEditor* pEditor)
		: DvPanel(pEditor, "Scene")
	{
		//SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
	}

	DvScenePanel::~DvScenePanel()
	{
	}
	/*
	void DvScenePanel::OnActiveScene(const Dive::Event& e)
	{
		const auto& evt = dynamic_cast<const Dive::CreateSceneEvent&>(e);
		m_pActiveScene = const_cast<Dive::Scene*>(evt.GetCreatedScene());
	}
	*/
	// Scene이 있다면
	// Enigne의 Backbuffer에서 rtv를 얻어와 그린다.
	void DvScenePanel::renderWindow()
	{
		/*
		if (!m_pActiveScene)
			return;

		// 크기를 계산한다. => 잘 모르겠다.
		auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		// renderer의 viewport를 설정한다.
		// Editor용 Camera도 함께 설정??
		m_pEditor->GetSceneViewCamera()->SetViewportSize(width, height);

		// 이전 ContentRegion과의 비교가 필요하다.
		Dive::Renderer::SetTextures(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
		auto pTexture = Dive::Renderer::GetGBufferAlbedo();
		pTexture = Dive::Renderer::GetGBufferNormal();
		//pTexture = Dive::Renderer::GetGBufferMaterial();
		//pTexture = Dive::Renderer::GetDepthStencilTexture();
		pTexture = Dive::Renderer::GetFrameOutput();

		// 매번 그리는 게 맞다.
		ImGui::Image(
			pTexture ? pTexture->GetShaderResourceView() : nullptr,
			ImVec2(width, height),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImColor(255, 255, 255, 255),
			ImColor(50, 127, 166, 255)
		);

		// guizmos
		auto offset = ImGui::GetWindowPos();
		ImGuizmo::SetRect(ImGui::GetWindowContentRegionMin().x + offset.x, ImGui::GetWindowContentRegionMin().y + offset.y, width, height);

		// test
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, m_pEditor->GetSceneViewCamera()->GetViewMatrix());
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMStoreFloat4x4(&proj, m_pEditor->GetSceneViewCamera()->GetProjectionMatrix());
		//DirectX::XMFLOAT4X4 mat; 
		//DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixIdentity());

		float* pCameraView = &view._11;
		float* pCameraProjection = &proj._11;
		//float* pMat = &mat._11;

		static const float identityMatrix[16] =
		{ 1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f };

		// 계속 오브젝트를 가린다.
		// 아마도 이미지 위에 그리기 때문인 것 같다.
		// 그렇다면 렌더링 순서를 바꿔도 의미가 없다.
		// 렌더 타겟을 바꾸면 될 것 같은데... 솔직히 어렵다.
		// 그렇다고 직접 만들면 Scene에 포함되어 버린다.
		//ImGuizmo::DrawGrid(pCameraView, pCameraProjection, identityMatrix, 100.f);

		auto pSelected = m_pEditor->GetHierarchy()->GetSeletecedObject();
		if (pSelected != nullptr)
		{
			if (!pSelected->HasComponent<Dive::Transform>())
				return;

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			// 대상
			DirectX::XMFLOAT4X4 matrix;
			DirectX::XMStoreFloat4x4(&matrix, pSelected->GetComponent<Dive::Transform>()->GetLocalMatrix());
			float* pTransform = &matrix._11;

			float snapValue = 0.5f;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			// 이미지 위에 그려지므로 순서는 상관없다.
			ImGuizmo::Manipulate(pCameraView, pCameraProjection,
				ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, pTransform,
				nullptr, snapValues, nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				auto pTransformComponent = pSelected->GetComponent<Dive::Transform>();
				matrix._11 = *pTransform;			matrix._12 = *(pTransform + 1);		matrix._13 = *(pTransform + 2);		matrix._14 = *(pTransform + 3);
				matrix._21 = *(pTransform + 4);		matrix._22 = *(pTransform + 5);		matrix._23 = *(pTransform + 6);		matrix._24 = *(pTransform + 7);
				matrix._31 = *(pTransform + 8);		matrix._32 = *(pTransform + 9);		matrix._33 = *(pTransform + 10);	matrix._34 = *(pTransform + 11);
				matrix._41 = *(pTransform + 12);	matrix._42 = *(pTransform + 13);	matrix._43 = *(pTransform + 14);	matrix._44 = *(pTransform + 15);

				DirectX::XMVECTOR translation, rotation, scale;
				DirectX::XMMatrixDecompose(&scale, &rotation, &translation, DirectX::XMLoadFloat4x4(&matrix));

				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, translation);
				pTransformComponent->SetLocalPosition(pos);
			}
		}
		*/
	}
}