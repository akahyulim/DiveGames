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

		// ���� ä��������...

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		// ��ó�� ����
		{
			// ī�޶� �������� ������ RenderTarget�� Clear
			// Renderable�� �������� ������ Camera Color�� RenderTarget Clear
			// constant buffer - frame?
		}

		legacyShader();

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
			m_gameObjects[eRenderableObjectType::Opaque].emplace_back(gameObject.get());
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
}
