#include "Viewport.h"
#include "../MenuBar.h"
#include "../Editor.h"

namespace Dive
{
	constexpr float BOOST_SPEED = 5.0f;
	constexpr float MIN_SPEED = 0.5f;
	constexpr float MAX_SPEED = 99.0f;

	Viewport::Viewport(Editor* editor)
		: View(editor)
	{
		m_title = "View";
		m_flags |= ImGuiWindowFlags_NoScrollbar;
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

	void Viewport::renderContent()
	{
		if (!EditorContext::ActiveWorld || !EditorContext::EditorCamera)
			return;

		cameraControll();

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();
		
		m_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
		m_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

		if (!m_renderTarget)
		{
			m_renderTarget = std::make_unique<RenderTexture>(
				static_cast<uint32_t>(m_width), 
				static_cast<uint32_t>(m_height),
				eDepthFormat::Depth24Stencil8);
			m_renderTarget->Create();

			EditorContext::EditorCamera->GetComponent<Camera>()->SetRenderTarget(m_renderTarget.get());
		}

		if (m_renderTarget->GetWidth() != static_cast<uint32_t>(m_width) || m_renderTarget->GetHeight() != static_cast<uint32_t>(m_height))
			m_renderTarget->Resize(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));

		ImTextureID textureID = (ImTextureID)(m_renderTarget->GetShaderResourceView());
		ImGui::Image(textureID, ImVec2(m_width, m_height));		
	}

	void Viewport::cameraControll()
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
			}
			else
			{
				auto mouseWheelDelta = Input::GetMouseWheelDelta();
				if (mouseWheelDelta != 0.0f)
				{
					AddCameraSpeed(mouseWheelDelta);
				}
			}
		}
	}
}
