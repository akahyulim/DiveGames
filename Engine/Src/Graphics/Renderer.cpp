#include "DivePch.h"
#include "Renderer.h"
#include "Model.h"
#include "D3D11/Sampler.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/BlendState.h"
#include "D3D11/DepthStencilState.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "Scene/GameObject.h"
#include "Scene/SceneEvents.h"
#include "Scene/Components/Renderable.h"



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
		m_deviceContext = m_graphics.lock()->GetRHIContext();

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

	void Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// 이전 세팅과 비교?

		m_deviceContext->IASetPrimitiveTopology(primitiveTopology);
	}

	// 여러개를 전달할 수 있다. 좀 더 살펴본 후 수정하자.
	void Renderer::setSampler(eSamplerType type)
	{
		if (m_samplerType == type)
			return;

		auto sampler = m_graphics.lock()->GetSampler(type)->GetState();

		m_deviceContext->PSSetSamplers(0, 1, &sampler);

		m_samplerType = type;
	}

	void Renderer::setRasterizerState(eRasterizerState state)
	{
		if (m_rasterizerState == state)
			return;

		auto rasterizerState = m_graphics.lock()->GetRasterizerState(state);
		
		m_deviceContext->RSSetState(rasterizerState->GetState());
		
		m_rasterizerState = state;
	}

	void Renderer::setBlendState(eBlendState state)
	{
		if (m_blendState == state)
			return;

		unsigned int sampleMask = 0xFFFFFFFF;
		auto blendState = m_graphics.lock()->GetBlendState(state);
		
		m_deviceContext->OMSetBlendState(blendState->GetState(), blendState->GetBlendFactor(), sampleMask);

		m_blendState = state;
	}

	void Renderer::setDepthStencilState(bool enabled)
	{
		if (m_bDepthStencilEnabled == enabled)
			return;

		unsigned int stencilRef = 1;
		auto depthStencilState = m_graphics.lock()->GetDepthStencilState(enabled);

		m_deviceContext->OMSetDepthStencilState(depthStencilState->GetState(), stencilRef);

		m_bDepthStencilEnabled = enabled;
	}

	// 일단 Transform의 위치 변환부터 구현해야 한다.
	// 이후 각종 리소스를 Graphics로 부터 어떻게 가져올 지 생각해야 한다.
	// 마지막으로 GameObject로부터 그려질 데이터를 가져와야 한다.
	void Renderer::testRender()
	{
		if (m_graphics.expired())
			return;

		auto immediateContext = m_graphics.lock()->GetRHIContext();
		auto renderableObjects = m_gameObjects[eRenderableObjectType::Opaque];
		if (renderableObjects.empty())
			return;

		// IASetInputLayout
		// VSSetShader
		// VSSetConstantBuffers
		// PSSetShader
		// PSSetConstantBuffers
		// PSSetSamplers
		// RSSetState
		// OMSetRenderTargets
		//SetBlendState(eBlendState::Enabled);
		//SetDepthStencilState(eDepthStencilState::Enabled);
		
		for (const auto& gameObject : renderableObjects)
		{
			auto renderable = gameObject->GetComponent<Renderable>();
			auto model = renderable->GetModel();
			if (!model || !model->GetVertexBuffer() || !model->GetIndexBuffer())
				continue;

			// IASetVerteBuffer
			// IASetIndexBuffer
			setPrimitiveTopology(renderable->GetPrimitiveTopology());
			
			// Draw
		}
	}
}
