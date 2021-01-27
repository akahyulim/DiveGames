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

		// ���� ä��������...

		// ���������� ������?...
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

		// light, camera, object(opaque, transparent), etc�� ����
		// object�� �ٽ� rigid, skinned �̷� ������ ���� �� ���� ������?
		// ���� �� ��� vertex type�� �� ���� �ٿ����ƾ� �Ѵ�.
		auto gameObjects = evnt->GetGameObjects();
		for (auto gameObject : gameObjects)
		{
			if (gameObject->HasComponent<Camera>())
			{
				// �������� �� �ִ�. �׷��ٸ� ���Ϳ� �����ϰ�, selected�� ���� �����ؾ� �Ѵ�.
				m_selectedCamera = gameObject.get();
			}
			
			if (gameObject->HasComponent<Renderable>())
			{
				// opaqu, transparent�� �����Ͽ� �����ؾ� �Ѵ�.
				// ���� vertex type���ε� �����ؾ� �Ѵ�.
				//m_gameObjects[eRenderableObjectType::Opaque].emplace_back(gameObject.get());
			}
		}
	}

	// �ϴ� å�� ������ ���� ����.
	// �׸��ڿ� ��ó������ ������ �� �޾�� ������ �����غ���.
	void Renderer::legacyShader()
	{
		// �ؾ������ ����...
		// �޾�񽺴� DepthBuffer�� ShaderResourceView�� ������ ���� �� ����.

		// Scene Render
		{

		}

		// Light Render
		{

		}
	}

	// RenderTargetView = ImGUI + renderEditor(view window)
	// Editor �������� gizmo�� �߰� �� �� �ִ�. �Ϲ������� DebugRender�� Ī�ϴ� �� ����. ��) grid
	void Renderer::renderEditor()
	{
		// ī�޶� �������� ������ RenderTarget�� Clear
		if (!m_selectedCamera)
		{
			auto view = m_graphics.lock()->GetEditorTexture()->GetRenderTargetView();
			float clearColor[4] = { 0.5f, 0.5f, 0.0f, 1.0f };
			m_deviceContext->ClearRenderTargetView(view, clearColor);

			return;
		}

		// Renderable�� �������� ������ Camera Color�� RenderTarget Clear
		if (m_gameObjects[eRenderableObjectType::Light].empty() && m_gameObjects[eRenderableObjectType::Opaque].empty()
			&& m_gameObjects[eRenderableObjectType::Transparent].empty())
		{
			// ���� command�� �����Ѵ�. ���� camera ������ �����Ѵ�.
			// �� �κ��� ���� ������ �ʿ��ϴ�. ��ī�� �ڽ��� ������ �� �ֱ� �����̴�.

			// �� ���� ���� �ؽ��Ĵ� �ȵɱ�?
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
