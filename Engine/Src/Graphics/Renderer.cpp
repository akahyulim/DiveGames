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
#include "Scene/Components/Camera.h"


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

		// 동적생성이 맞을까?...
		m_command = new Command(m_context);

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		//legacyShader();
		renderEditor();

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
			if (gameObject->HasComponent<Camera>())
			{
				// 여러개일 수 있다. 그렇다면 벡터에 저장하고, selected를 따로 관리해야 한다.
				m_selectedCamera = gameObject.get();
			}
			
			if (gameObject->HasComponent<Renderable>())
			{
				// opaqu, transparent를 구분하여 저장해야 한다.
				// 추후 vertex type별로도 구분해야 한다.
				//m_gameObjects[eRenderableObjectType::Opaque].emplace_back(gameObject.get());
			}
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

	// RenderTargetView = ImGUI + renderEditor(view window)
	// Editor 전용으로 gizmo가 추가 될 수 있다. 일반적으론 DebugRender라 칭하는 것 같다. 예) grid
	void Renderer::renderEditor()
	{
		// 카메라가 존재하지 않으면 RenderTarget만 Clear
		if (!m_selectedCamera)
		{
			auto view = m_graphics.lock()->GetEditorTexture()->GetRenderTargetView();
			float clearColor[4] = { 0.5f, 0.5f, 0.0f, 1.0f };
			m_deviceContext->ClearRenderTargetView(view, clearColor);

			return;
		}

		// Renderable이 존재하지 않으면 Camera Color로 RenderTarget Clear
		if (m_gameObjects[eRenderableObjectType::Light].empty() && m_gameObjects[eRenderableObjectType::Opaque].empty()
			&& m_gameObjects[eRenderableObjectType::Transparent].empty())
		{
			// 역시 command로 설정한다. 색상만 camera 것으로 전달한다.
			// 이 부분은 추후 수정이 필요하다. 스카이 박스가 설정될 수 있기 때문이다.

			// 왜 직접 만든 텍스쳐는 안될까?
			auto view = m_graphics.lock()->GetEditorTexture()->GetRenderTargetView();
			if (!view)
			{
				CORE_ERROR("NOT FOUND RENDER TARGET VIEW");
				return;
			}
			DirectX::XMFLOAT4 color = m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor();
			float clearColor[4] = { color.x, color.y, color.z, color.w };
			m_deviceContext->ClearRenderTargetView(view, clearColor);
			return;
		}

		// constant buffer - frame?
		{

		}
	}
}
