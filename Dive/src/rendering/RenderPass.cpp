#include "stdafx.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "graphics/Graphics.h"
#include "world/World.h"
#include "world/GameObject.h"
#include "world/Components/Camera.h"
#include "world/Components/MeshRenderer.h"


namespace Dive
{
	// GameObject가 아닌 Camera로 받아도 될 듯하다.
	void TestPass::Execute(ID3D11DeviceContext* deviceContext, const Camera* camera)
	{
		assert(deviceContext);
		assert(camera);

		if (!WorldManager::GetActiveWorld())
			return;

		const auto world = WorldManager::GetActiveWorld();
		
		// camera
		camera->Bind(deviceContext);
		
		// pipelineState를 살려야하나..
		deviceContext->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
		deviceContext->OMSetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite_StencilReadWrite), 0);
		
		// 일단 한 번에 바인딩 해봤다.
		Renderer::BindSamplerStates();
		
		// opaque
		for (auto meshRenderer : world->GetOpaqueMeshRenderers())
			meshRenderer->Render(deviceContext, camera);
	}
}
