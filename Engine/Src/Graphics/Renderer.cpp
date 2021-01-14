#include "DivePch.h"
#include "Renderer.h"
#include "MeshFilter.h"
#include "D3D11/Sampler.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/BlendState.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/Shader.h"
#include "D3D11/ConstantBuffer.h"
#include "D3D11/Texture2D.h"
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
		m_bInitialized(false),
		m_meshFilter(nullptr)
	{
		DIVE_SUBSCRIBE_TO_EVENT(this, &Renderer::OnAcquireRenderable);
	}

	Renderer::~Renderer()
	{
		DIVE_UNSUBSCRIBE_FROM_EVENT(Renderer, E_UPDATE_SCENE, this);
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
		testRender();
	}

	void Renderer::OnAcquireRenderable(const E_UPDATE_SCENE* evnt)
	{
		if (!evnt || evnt->GetGameObjects().empty())
			return;

		// 일단 그냥 저장
		auto gameObjects = evnt->GetGameObjects();
		for (auto gameObject : gameObjects)
		{
			m_gameObjects[eRenderableObjectType::Opaque].emplace_back(gameObject.get());
		}
	}

	// 버퍼가 다수라면?
	// dynamic buffer는 따로?
	void Renderer::setBuffers(Renderable* renderable)
	{
		if (!renderable || !renderable->GetMeshFilter())
			return;
		
		//if(m_meshFilter->GetID() == renderable->GetMeshFilter()->GetID())
		//	return;

		m_meshFilter = renderable->GetMeshFilter();

		auto vertexBuffer = m_meshFilter->GetVertexBuffer();
		auto indexBuffer = m_meshFilter->GetIndexBuffer();
		if (!vertexBuffer || !indexBuffer)
			return;

		// set vertex buffers
		{
			auto buffer = vertexBuffer->GetBuffer();
			auto stride = vertexBuffer->GetStride();
			auto offset = renderable->GetVertexBufferOffset();

			m_deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
		}

		// set index buffers
		{
			auto offset = renderable->GetIndexBufferOffset();
			m_deviceContext->IASetIndexBuffer(indexBuffer->GetBuffer(), indexBuffer->GetFormat(), offset);
		}
	}

	// 이것두 그냥 renderable을 받게?
	void Renderer::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// 이전 세팅과 비교?

		m_deviceContext->IASetPrimitiveTopology(primitiveTopology);
	}

	// 일단 shader와 input layout을 모두 전달한다.
	// constantbuffer까지...
	void Renderer::setShader()
	{
		auto shader = m_graphics.lock()->GetBaseShader();

		m_deviceContext->IASetInputLayout(shader->GetInputLayout());
		m_deviceContext->VSSetShader(shader->GetVertexShader(), nullptr, 0);
		m_deviceContext->PSSetShader(shader->GetPixelShader(), nullptr, 0);

		auto matrixBuffer = m_graphics.lock()->GetConstantBuffer()->GetBuffer();
		m_deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	}

	// 여러개를 전달할 수 있다. 좀 더 살펴본 후 수정하자.
	void Renderer::setSampler(eSamplerType type)
	{
		//if (m_samplerType == type)
		//	return;

		auto sampler = m_graphics.lock()->GetSampler(type)->GetState();

		m_deviceContext->PSSetSamplers(0, 1, &sampler);

		//m_samplerType = type;
	}

	void Renderer::setRasterizerState(eRasterizerState state)
	{
		//if (m_rasterizerState == state)
		//	return;

		auto rasterizerState = m_graphics.lock()->GetRasterizerState(state);
		
		m_deviceContext->RSSetState(rasterizerState->GetState());
		
		//m_rasterizerState = state;
	}

	// 현재 직접 만든 RenderTargetView가 화면에 나타나지 않는다.
	// 이는 Backbuffer를 기반으로 하지 않아서일 수 있다.
	void Renderer::setRenderTarget()
	{
		auto renderTargetView = m_graphics.lock()->GetRenderTarget()->GetRenderTargetView();//m_graphics.lock()->GetRenderTargetView();
		float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

		m_deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
		m_deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	}

	void Renderer::setBlendState(eBlendState state, unsigned int sampleMask)
	{
		if (m_blendState == state)
			return;

		auto blendState = m_graphics.lock()->GetBlendState(state);
		
		m_deviceContext->OMSetBlendState(blendState->GetState(), blendState->GetBlendFactor(), sampleMask);

		m_blendState = state;
	}

	// m_bDepthStencilEnabled를 초기화하지 않아 사용 불가
	void Renderer::setDepthStencilState(bool enabled)
	{
		if (m_bDepthStencilEnabled == enabled)
			return;

		unsigned int stencilRef = 1;
		auto depthStencilState = m_graphics.lock()->GetDepthStencilState(enabled);

		m_deviceContext->OMSetDepthStencilState(depthStencilState->GetState(), stencilRef);

		m_bDepthStencilEnabled = enabled;
	}

	void Renderer::drawIndexed(Renderable* renderable)
	{
		auto meshFilter = renderable->GetMeshFilter();
		auto indexCount = meshFilter->GetIndexCount();
		auto indexOffset = renderable->GetIndexBufferOffset();
		auto vertexOffset = renderable->GetVertexBufferOffset();

		m_deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
	}

	// 일단 commandLine을 설정 한 후 Draw하자.
	// 이후 좀 더 세분화하여 overhead를 줄여나가자.
	void Renderer::testRender()
	{
		if (m_graphics.expired())
			return;

		auto immediateContext = m_graphics.lock()->GetRHIContext();
		auto renderableObjects = m_gameObjects[eRenderableObjectType::Opaque];
		if (renderableObjects.empty())
			return;

		//setRenderTarget();

		// viewport, constant buffer
		{
			D3D11_VIEWPORT viewport;
			viewport.Width = (float)m_graphics.lock()->GetWidth();
			viewport.Height = (float)m_graphics.lock()->GetHeight();
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			// Create the viewport.
			immediateContext->RSSetViewports(1, &viewport);

		}

		setShader();
		// VSSetConstantBuffers => world, view, proj
		// PSSetConstantBuffers
		//setSampler(eSamplerType::Linear);
		setRasterizerState(eRasterizerState::CullBackSolid);
		//setBlendState(eBlendState::Disabled, 0xFFFFFFFF);
		setDepthStencilState(true);
		
		// 엄밀하게 따지자면 Shader도 Renderable의 Material에 포함되었을 것이다...
		for (const auto& gameObject : renderableObjects)
		{
			auto renderable = gameObject->GetComponent<Renderable>();
			if (!renderable)
				continue;

			setBuffers(renderable);
			setPrimitiveTopology(renderable->GetPrimitiveTopology());
			drawIndexed(renderable);
		}
	}
}
