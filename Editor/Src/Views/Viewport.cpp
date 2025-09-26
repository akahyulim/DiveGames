#include "Viewport.h"
#include "../MenuBar.h"
#include "../Editor.h"
#include <ImGuizmo.h>

using namespace DirectX;

namespace Dive
{
	constexpr float BOOST_SPEED = 10.0f;
	constexpr float MIN_SPEED = 0.5f;
	constexpr float MAX_SPEED = 99.0f;

	Viewport::Viewport(Editor* editor)
		: View(editor)
	{
		m_Title = "View";
		m_Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus;
	}

	void Viewport::SetCameraSpeed(float speed)
	{
		if (speed < MIN_SPEED)
			m_cameraSpeed = MIN_SPEED;
		else if (speed > MAX_SPEED)
			m_cameraSpeed = MAX_SPEED;
		else
			m_cameraSpeed = speed;
	}

	void Viewport::AddCameraSpeed(int delta)
	{
		float speed = m_cameraSpeed + static_cast<float>(delta);
		SetCameraSpeed(speed);
	}

	void Viewport::CameraControll()
	{
		if (ImGui::IsWindowFocused())
		{
			auto transform = EditorContext::EditorCamera->GetTransform();
			float moveSpeed = m_cameraSpeed * Timer::GetDeltaTimeSec();
			if (Input::KeyPress(DIK_LSHIFT))
				moveSpeed *= BOOST_SPEED;

			if (Input::MouseButtonPress(1))
			{
				if (ImGui::IsWindowHovered())
				{
					auto mouseMoveDelta = Input::GetMouseMoveDelta();
					if (mouseMoveDelta.x != 0.0f || mouseMoveDelta.y != 0.0f)
					{
						transform->RotateByDegrees(DirectX::XMFLOAT3(0.0f, mouseMoveDelta.x * moveSpeed, 0.0f));
						transform->RotateByDegrees(DirectX::XMFLOAT3(mouseMoveDelta.y * moveSpeed, 0.0f, 0.0f));
					}

					auto mouseWheelDelta = Input::GetMouseWheelDelta();
					if (mouseWheelDelta != 0.0f)
						transform->Translate(DirectX::XMFLOAT3(0.0f, 0.0f, mouseWheelDelta * moveSpeed));
				}
			}

			if (Input::MouseButtonDown(0))
			{
				if (ImGui::IsWindowHovered())
				{
					Ray ray = EditorContext::EditorCamera->GetComponent<Camera>()->ScreenPointToRay(GetMousePosition());
					RaycastHit hit;
					EditorContext::Selected =
						EditorContext::ActiveWorld->Raycast(ray, &hit) ? hit.hitObject : nullptr;
				}
			}

			if (Input::KeyPress(DIK_W))
			{
				transform->Translate(DirectX::XMFLOAT3(0.0f, 0.0f, moveSpeed));
			}
			if (Input::KeyPress(DIK_S))
			{
				transform->Translate(DirectX::XMFLOAT3(0.0f, 0.0f, -moveSpeed));
			}
			if (Input::KeyPress(DIK_A))
			{
				transform->Translate(DirectX::XMFLOAT3(-moveSpeed, 0.0f, 0.0f));
			}
			if (Input::KeyPress(DIK_D))
			{
				transform->Translate(DirectX::XMFLOAT3(moveSpeed, 0.0f, 0.0f));
			}
			if (Input::KeyPress(DIK_Q))
			{
				transform->Translate(DirectX::XMFLOAT3(0.0f, -moveSpeed, 0.0f));
			}
			if (Input::KeyPress(DIK_E))
			{
				transform->Translate(DirectX::XMFLOAT3(0.0f, moveSpeed, 0.0f));
			}
			//	}
				//else
			{
				auto mouseWheelDelta = Input::GetMouseWheelDelta();
				if (mouseWheelDelta != 0.0f)
				{
					AddCameraSpeed(mouseWheelDelta);
				}

				if (Input::KeyPress(DIK_LEFT))
				{
					transform->RotateByDegrees(DirectX::XMFLOAT3(0.0f, -moveSpeed * 2.0f, 0.0f));
				}
				if (Input::KeyPress(DIK_RIGHT))
				{
					transform->RotateByDegrees(DirectX::XMFLOAT3(0.0f, moveSpeed * 2.0f, 0.0f));
				}
				if (Input::KeyPress(DIK_UP))
				{
					transform->RotateByDegrees(DirectX::XMFLOAT3(-moveSpeed * 2.0f, 0.0f, 0.0f));
				}
				if (Input::KeyPress(DIK_DOWN))
				{
					transform->RotateByDegrees(DirectX::XMFLOAT3(moveSpeed * 2.0f, 0.0f, 0.0f));
				}
			}
		}
	}

	void Viewport::DrawTransformGizmo()
	{
		if (!EditorContext::Selected)
			return;

		ImGuizmo::Enable(true);
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(
			ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x,
			ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y,
			m_Width, m_Height
		);

		auto transform = EditorContext::Selected->GetComponent<Transform>();
		auto camera = EditorContext::EditorCamera->GetComponent<Camera>();

		XMMATRIX view = camera->GetViewMatrix();
		XMMATRIX proj = camera->GetProjectionMatrix();
		XMMATRIX model = transform->GetLocalTransformMatrix();

		// 유니티의 snap: ctrl + drag
		// translate 1, rotate 15, scale 0.1
		// 이외에도 툴바를 통해 translate, rotate, scale 및 local/world 설정
		ImGuizmo::Manipulate(
			reinterpret_cast<float*>(&view),
			reinterpret_cast<float*>(&proj),
			ImGuizmo::TRANSLATE,
			ImGuizmo::LOCAL,
			reinterpret_cast<float*>(&model)
		);

		if (ImGuizmo::IsUsing())
			transform->SetLocalTransformMatrix(model);
	}

	void Viewport::renderContent()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::EditorCamera)
			return;

		m_Width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_Height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_renderTarget)
		{
			m_renderTarget = std::make_unique<RenderTexture>(
				static_cast<uint32_t>(m_Width),
				static_cast<uint32_t>(m_Height),
				eDepthFormat::Depth24Stencil8);
			m_renderTarget->Create();

			EditorContext::EditorCamera->GetComponent<Camera>()->SetRenderTarget(m_renderTarget.get());
		}

		if (m_renderTarget->GetWidth() != static_cast<uint32_t>(m_Width) || m_renderTarget->GetHeight() != static_cast<uint32_t>(m_Height))
			m_renderTarget->Resize(static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

		ImTextureID textureID = (ImTextureID)(m_renderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_Width, m_Height));
		ImGui::SetItemAllowOverlap();

		CameraControll();
		DrawTransformGizmo();
	}
}
