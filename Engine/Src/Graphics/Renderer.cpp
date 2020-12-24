#include "DivePch.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "Scene/GameObject.h"
#include "Scene/SceneEvents.h"


namespace Dive
{
	Renderer::Renderer(Context* context)
		: Object(context),
		m_bInitialized(false)
	{
	}

	Renderer::~Renderer()
	{
		CORE_TRACE("Call Renderer's Desctructor =====================");
	}

	void Renderer::Initialize()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
			return;
		m_graphics = graphics;

		// 뭘로 채워나가나...

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		// 기본 설정 후 Pass를 순서대로 호출한다.
		testRender();
	}

	void Renderer::OnAcquireRenderable(const E_UPDATE_SCENE* evnt)
	{
		if (!evnt || evnt->GetGameObjects().empty())
			return;

		// 타입을 나눈 후 저장한다.
	}
	
	void Renderer::testRender()
	{
		if (m_graphics.expired())
			return;

		auto immediateContext = m_graphics.lock()->GetRHIContext();
		auto renderableObjects = m_gameObjects[eRenderableObjectType::Opaque];
		if (renderableObjects.empty())
			return;

		// VSSetShader
		// VSSetConstantBuffers
		// PSSetShader
		// PSSetConstantBuffers
		// PSSetSamplers
		// RSSetState
		// OMSetRenderTargets
		// OMSetBlendState
		// OMSetDepthStencilState
		
		for (const auto& gameObject : renderableObjects)
		{
			// IASetInputLayout
			// IASetVerteBuffer
			// IASetIndexBuffer
			// IASetPrimitiveTopology
			//immediateContext->IASetPrimitiveTopology()
			
		}
	}
}
