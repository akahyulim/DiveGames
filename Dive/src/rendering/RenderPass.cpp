#include "stdafx.h"
#include "RenderPass.h"
#include "Renderer.h"
#include "graphics/Graphics.h"
#include "world/World.h"
#include "world/WorldManager.h"
#include "world/GameObject.h"
#include "world/components/Camera.h"
#include "world/Components/Light.h"
#include "world/components/MeshRenderer.h"


namespace Dive
{
	// GameObject가 아닌 Camera로 받아도 될 듯하다.
	void ForwardLighting::Execute(const Camera* camera)
	{
		assert(camera);

		if (!WorldManager::GetActiveWorld())
			return;

		const auto world = WorldManager::GetActiveWorld();

		// camera
		camera->Bind();

		// pipelineState를 살려야하나..
		Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullBack));
		Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthReadWrite_StencilReadWrite), 0);

		// 일단 한 번에 바인딩 해봤다.
		Renderer::BindSamplerStates();

		//for (auto light : world->GetLights())
		{
			// DirLight는 하나인 것이 좋고
			// 나머지 Light들은 다시 그린 후 합쳐야 한다.
			//light->Bind(deviceContext);

			// opaque
			for (auto meshRenderer : world->GetOpaqueMeshRenderers())
				meshRenderer->Render(camera);
		}
	}
}
