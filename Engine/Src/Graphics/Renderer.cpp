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

		// ���� ä��������...

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		// �⺻ ���� �� Pass�� ������� ȣ���Ѵ�.
		testRender();
	}

	void Renderer::OnAcquireRenderable(const E_UPDATE_SCENE* evnt)
	{
		if (!evnt || evnt->GetGameObjects().empty())
			return;

		// Ÿ���� ���� �� �����Ѵ�.
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
