#include "stdafx.h"
#include "RenderingSystem.h"
#include "graphics/Graphics.h"
#include "rendering/RenderingPipeline.h"
#include "world/World.h"
#include "world/Components.h"

namespace Dive
{
	void RenderMeshSystem::OnUpdate()
	{
		if (!m_World)
			return;

		auto& entityManager = m_World->GetEntityManager();

		// buffer 생성 및 갱신
	}

	void EntitiesGraphicsSystem::OnUpdate()
	{
		if (!m_World)
			return;

		auto& entityManager = m_World->GetEntityManager();
		auto view = entityManager.GetEntityView<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraCom = entityManager.GetComponent<CameraComponent>(entity);
			auto renderTargetView = cameraCom.RenderTarget ? cameraCom.RenderTarget->GetRenderTargetView() : Graphics::GetBackBufferRTV();
			auto clearColor = cameraCom.RenderTarget ? cameraCom.ClearColor : DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f };

			Graphics::SetRenderTarget(renderTargetView, nullptr);
			Graphics::ClearRenderTargetView(renderTargetView, clearColor);

			// constant buffer: camera view
			// constant buffer: camera projection
			
			// model
			{
				// mesh
				{
					// vertex buffer
					// index buffer
					// constant buffer
					// vertex shader: mesh type에 따라 vertex shader를 선택
				}

				// matrial
				{
					// texture
					// constant buffer
					// pixel shader: material에서 관리
				}

				// draw
			}
			
		}
	}
}