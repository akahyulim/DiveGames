#include "stdafx.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "graphics/Graphics.h"
#include "graphics/RenderTexture.h"
#include "graphics/ShaderManager.h"
#include "world/World.h"
#include "world/WorldManager.h"
#include "world/GameObject.h"
#include "world/components/Camera.h"
#include "world/Components/Light.h"
#include "world/components/MeshRenderer.h"


namespace Dive
{
	// Main Directional Light만 적용한다.
	void ForwardBase::Execute(const Camera* cameraCom)
	{
		assert(cameraCom);

		if (!WorldManager::GetActiveWorld())
			return;

		const auto world = WorldManager::GetActiveWorld();
		if (!world->GetOpaqueMeshRenderers().empty())
		{
			Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite), 1);
			Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));

			for (auto meshRenderer : world->GetOpaqueMeshRenderers())
				meshRenderer->Render(cameraCom->GetViewMatrix(), cameraCom->GetProjectionMatrix());
		}
	}

	// 나머지 Light들을 차례로 적용한다.
	void ForwardAdd::Execute(const Camera* cameraCom)
	{
		// 현재 Light를 하나의 ConstantBuffer에 모아서 전달하기 때문에
		// 단순히 MainDirectional과 Add를 나눈다고 적용할 수 있는 문제가 아니다.
	}

	void Transparent::Execute(const Camera* cameraCom)
	{
		assert(cameraCom);

		if (!WorldManager::GetActiveWorld())
			return;
		
		auto world = WorldManager::GetActiveWorld();
		if (!world->GetTransparentMeshRenderers().empty())
		{
			Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(eBlendState::AlphaEnabled).Get(), nullptr, 0xffffffff);
			Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::Transparent), 1);
			Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullNone));

			for (auto meshRenderer : world->GetTransparentMeshRenderers())
				meshRenderer->Render(cameraCom->GetViewMatrix(), cameraCom->GetProjectionMatrix());

			Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(eBlendState::AlphaDisabled).Get(), nullptr, 0xffffffff);
		}
	}

	void ResolveScene::Execute(const Camera* cameraCom)
	{
		assert(cameraCom);

		auto renderTargetView = Graphics::GetRenderTargetView();
		auto depthStencilView = Graphics::GetDepthStencilView();
		Graphics::GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, nullptr);
		Graphics::GetDeviceContext()->ClearRenderTargetView(renderTargetView, cameraCom->GetBackgroundColor());

		auto rt = cameraCom->GetRenderTarget();
		rt->Bind(eShaderResourceSlot::SceneColor);

		ShaderManager::GetProgram("ResolveScene")->Bind();

		Graphics::GetDeviceContext()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		Graphics::GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Graphics::GetDeviceContext()->Draw(4, 0);
	}
}
