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
#include "D3D11/PipelineState.h"
#include "D3D11/Command.h"
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
		// 전처리 존재
		{
			// 카메라가 존재하지 않으면 RenderTarget만 Clear
			// Renderable이 존재하지 않으면 Camera Color로 RenderTarget Clear
			// constant buffer - frame?
		}

		legacyShader();

		// frame count ++
	}

	void Renderer::OnAcquireRenderable(const E_UPDATE_SCENE* evnt)
	{
		if (!evnt || evnt->GetGameObjects().empty())
			return;

		// light, camera, object(opaque, transparent), etc로 구분
		// object는 다시 rigid, skinned 이런 식으로 나눌 수 있지 않을까?
		// 물론 이 경우 vertex type은 두 개로 줄여놓아야 한다.
		auto gameObjects = evnt->GetGameObjects();
		for (auto gameObject : gameObjects)
		{
			m_gameObjects[eRenderableObjectType::Opaque].emplace_back(gameObject.get());
		}
	}

	// 일단 책의 내용을 따라 가자.
	// 그림자와 후처리까지 진행한 후 펄어비스 구현을 적용해보자.
	void Renderer::legacyShader()
	{
		// 잊어버리기 전에...
		// 펄어비스는 DepthBuffer를 ShaderResourceView로 만들지 않은 것 같다.

		// Scene Render
		{

		}

		// Light Render
		{

		}
	}
}
