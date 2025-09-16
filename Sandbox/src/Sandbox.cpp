#include "Sandbox.h"

namespace Dive
{
	LRESULT CALLBACK SandboxMessageHandler(HWND hWnd, UINT32 msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
		{
			DV_FIRE_EVENT(Dive::eEventType::WindowResized);
			return 0;
		}
		case WM_CLOSE:
		{
			Dive::Window::Close();
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Sandbox::Sandbox()
	{
		DV_SUBSCRIBE_EVENT(eEventType::Update, DV_EVENT_HANDLER(cameraControll));
	}

	void Sandbox::Setup()
	{
		LogManager::SetFilename("sandbox.log");
	}
	
	void Sandbox::Start()
	{
		Window::SetMessageCallback((LONG_PTR)SandboxMessageHandler);
		Window::SetTitle(L"Sandbox");
		Window::Resize(1600, 900);

		// texture2D
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/dmc.jpg");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/DokeV.jpeg");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/relaxed_morning.jpg");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/sky_daytime_blue.jpg");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/stone01.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/normal01.tga");

		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/WornWood/WornWood_Albedo.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Textures/WornWood/WornWood_Normal.tga");

		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/tree/Textures/Leaf_Oak_Gum_AE3D_10302022-A.png");

		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/lion.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/lion_ddn.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/spnza_bricks_a_diff.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/spnza_bricks_a_ddn.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/sponza_ceiling_a_diff.tga");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Sponza/Textures/sponza_ceiling_a_ddn.tga");

		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Cube/Textures/Cube_diffuse.jpg");
		ResourceManager::GetOrLoad<Texture2D>("Assets/Models/Cube/Textures/Cube_normal.jpg");

		auto world = WorldManager::LoadWorld("Assets/Worlds/NewWorld.dive");
		for (auto cameraCom : Camera::GetAllCameras())
		{
			if (cameraCom->GetGameObject()->GetTag() == "MainCamera")
			{
				m_camera = cameraCom->GetGameObject();
				m_renderTarget = std::make_unique<RenderTexture>(
					static_cast<uint32_t>(Graphics::GetWidth()),
					static_cast<uint32_t>(Graphics::GetHeight()),
					eDepthFormat::Depth24Stencil8);
				m_renderTarget->Create();
				cameraCom->SetRenderTarget(m_renderTarget.get());
				break;
			}
		}

		DV_LOG(Sandbox, info, "Start Complete");
	}
	
	void Sandbox::Stop()
	{
	}

	void Sandbox::cameraControll()
	{
		auto transform = m_camera->GetTransform();
		float moveSpeed = Timer::GetDeltaTimeSec();
		if (Input::KeyPress(DIK_LSHIFT))
			moveSpeed *= 5.0f;

		if (Input::MouseButtonPress(1))
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
				//AddCameraSpeed(mouseWheelDelta);
			}
		}
	}
}