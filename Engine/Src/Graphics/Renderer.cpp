#include "DivePch.h"
#include "Renderer.h"
#include "GraphicsEnums.h"
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
		m_graphics = graphics.get();
		m_deviceContext = m_graphics->GetRHIContext();

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
				// Material�� ����� Rendering Mode�� ����
				// �⺻������ opaque, transparent ���� cutout, fade �߰�?
			}

			// light component ���η� light ����
		}
	}

	// �ϴ� å�� ������ ���� ����.
	// �׸��ڿ� ��ó������ ������ �� �޾�� ������ �����غ���.
	void Renderer::legacyShader()
	{
		// �ؾ������ ����...
		// �޾�񽺴� DepthBuffer�� ShaderResourceView�� ������ ���� �� ����.
		// => �ٸ� RenderTargetView���� ������ ������ ���� �� �ִ�.

		// Scene Render
		{

		}

		// Light Render
		{

		}
	}

	// RenderTargetView = ImGUI + renderEditor(view window)
	// Editor �������� gizmo�� �߰� �� �� �ִ�. �Ϲ������� DebugRender�� Ī�ϴ� �� ����. ��) grid
	// ���ĸ�ź�� Debug Render �Լ����� ���� �����Ǿ� �̴�.
	void Renderer::renderEditor()
	{
		// ī�޶� �������� ������ RenderTarget�� Clear
		if (!m_selectedCamera)
		{
			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f));

			return;
		}

		// Renderable�� �������� ������ Camera Color�� RenderTarget Clear
		if (m_gameObjects[eRenderableObjectType::Light].empty() && m_gameObjects[eRenderableObjectType::Opaque].empty()
			&& m_gameObjects[eRenderableObjectType::Transparent].empty())
		{
			// �� �κ��� ���� ������ �ʿ��ϴ�. ��ī�� �ڽ��� ������ �� �ֱ� �����̴�.

			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - frame?
		{

		}

		// ���ϳ����� Shader���� ���� Command�� �������� �׸���.

		base();
	}

	void Renderer::base()
	{
		// input layout�� ��ġ�ϴ� ������Ʈ�� ���ٸ� �����ؾ� �Ѵ�.
		// �̰� �ᱹ Shader�� ����� InputLayout�� Object�� �������� ���ؾ� �Ѵ�.
		// ���ٿ��� Transparent���� �����Ѵٸ� VertexTypeCount * 2���� ������ ���͹�����.
		// ���� �̶� State�� Shader�� �����Ѵٸ� ���� ������ �� �ִ�.

		// state ����
		{
			// graphics�κ��� resource���� ������ state�� ������ ��
			// command�� �����Ͽ� gpu�� �����Ѵ�.
			PipelineState state;
			state.depthStencilState		= m_graphics->GetDepthStencilState(true);
			state.primitiveTopology		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			state.depthStencilTexture	= m_graphics->GetRenderTexture(eRenderTextureType::DepthStencil);
			state.renderTargets.push_back(m_graphics->GetRenderTexture(eRenderTextureType::EditorView));

			m_command->SetPipelineState(state);

			// ���� pass���� �̹� Clear�� EditorView �ϳ��� ���������,
			// deferred ó�� �ټ��� render target�� ���Ǵ� ��� ���� �� �� clear�ؾ� �Ѵ�.
		}

		// data ����
		// state�� shader�� input layout�� ���� object type�� �����ؾ� �Ѵ�.
		{
			// type���� ���е� buffer���� command�� ���� gpu�� �����Ѵ�.
		}
	}

	void Renderer::testTemplate()
	{
		// pass���� �׷����� �� object�� �켱 Ȯ���Ѵ�. ���ٸ� ����
		// ���� ��� opaque�� transparent �� �� ����ִٸ� ����
		// �ٷ� �����ϸ� �ȵǴ� ���� �� �ִ�. ���� ��� RenderTargetView ������ �Ʒ����� Clear���־�� �Ѵ�.

		// pass �ǵ��� �°� �⺻���� state�� ����
		
		// ���� state�� data�� �����ϴ� ���� �Լ� ����
		// �Ű������� material�� renderable�� �����Ͽ� shader�� vertex type�� ��Ī?
		// �� ��� material�� �Ű������� �����ؾ� �Ѵ�.

		// ��� object vector�� ���� �Լ��� draw
		// ������ VertexType�� ���о��� RenderableType���θ� �����Ѵٸ� ������尡 �߻��� �� �ִ�.
	}
}
