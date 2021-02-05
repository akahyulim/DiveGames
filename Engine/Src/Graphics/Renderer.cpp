#include "DivePch.h"
#include "Renderer.h"
#include "GraphicsEnums.h"
#include "Material.h"
#include "Legacy.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "D3D11/Sampler.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/BlendState.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/Shader.h"
#include "D3D11/ConstantBuffer.h"
#include "D3D11/Texture2D.h"
#include "D3D11/PipelineState.h"
#include "D3D11/Command.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"
#include "Scene/GameObject.h"
#include "Scene/SceneEvents.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/RenderableMesh.h"
#include "Scene/Components/SkinnedRenderableMesh.h"
#include "Geometry/Mesh.h"


namespace Dive
{
	Renderer::Renderer(Context* context)
		: Object(context),
		m_bInitialized(false),
		m_selectedCamera(nullptr)
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

		m_command = std::make_unique<Command>(m_context);

		m_bInitialized = true;
	}

	void Renderer::Render()
	{
		if (!m_selectedCamera)
		{
			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f));

			return;
		}

		// �� �κ��� �ϴ� �Ʒ��� �з� �Լ��� �������̴�.
		if (m_gameObjects[eRenderableObjectType::Light].empty())
		{
			// ��ī�� �ڽ��� ������ �� �ִ�. �̰� Scene�� Skybox�ʹ� �����̴�.

			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - �� ������ ����
		{

		}

		// ���ϳ����� Shader���� ���� Command�� �������� �׸���.

		// frame count ++
	}

	// Scene���κ��� �׷��� �� ������ ������ �� �з��Ѵ�.
	void Renderer::OnAcquireRenderable(const E_UPDATE_SCENE* evnt)
	{
		if (!evnt || evnt->GetGameObjects().empty())
			return;

		auto gameObjects = evnt->GetGameObjects();
		for (auto gameObject : gameObjects)
		{
			if (gameObject->HasComponent<Camera>())
			{
				m_gameObjects[eRenderableObjectType::Camera].push_back(gameObject.get());
			}
			
			// �̷����ϸ� �ϴ� Renderable�� ���� �� �� ������ �ϰ�
			// �ϳ��� GameObject�� �� �� �̻� ����� �� �ִ�...
			if (gameObject->HasComponent<RenderableMesh>())
			{
				for (const auto& material : gameObject->GetComponent<RenderableMesh>()->GetAllMaterial())
				{
					if (material->GetShaderPass() == eShaderPassType::Legacy)
					{
						switch (dynamic_cast<Legacy*>(material)->GetRenderingMode())
						{
						case eRenderingMode::Opaque:		m_gameObjects[eRenderableObjectType::Opaque].push_back(gameObject.get());		break;
						case eRenderingMode::Transparent:	m_gameObjects[eRenderableObjectType::Transparent].push_back(gameObject.get());	break;
						default:							CORE_ERROR("");																	break;
						}
					}
				}
			}

			// light component ���η� light ����
		}

		// ù ��° ī�޶�� ����Ʈ ����
		m_selectedCamera = m_gameObjects[eRenderableObjectType::Camera][0];
	}

	// �����δ� technique�̴�. ���ο��� �ٽ� pass�� �������� �Ѵ�.
	void Renderer::pass_Color()
	{
		if (m_gameObjects[eRenderableObjectType::Opaque].empty())
			return;

		// PipelineState
		{
			PipelineState state;
			// MeshRenderer, Material�� ����
			// �� ���� ����(DepthStencil, Blending, Rasterizer ��)�� ������ ����ؾ� �Ѵ�.
			// shader�� ������ �� ����. shader ���� ��������, �Ʒ��� �������� material�κ��� shader�� �����ؾ� �Ѵ�.
			// �ƴϸ� �� ������ shader�� ����� ��, �Ʒ��� �������� ������ Shader�� ������� �ʴ� ��� continue�� �Ѱܾ� �Ѵ�.

			m_command->SetPipelineState(state);
		}

		for(const auto& gameObject : m_gameObjects[eRenderableObjectType::Opaque])
		{
			// �⺻������ Renderable�� ����� ��ü���� ����Ѵ�.
			// �׷��ٸ� command�� Renderable�� �ѱ�°� ���� ������?
			auto renderable = gameObject->GetComponent<RenderableMesh>();
			auto mesh = renderable->GetMesh();

			// material shader resource view
			// material constant buffer
			// world position constant buffer? => �������� Transform�� viewProj�� �Ѱ� ���� ����� ����� buffer�� �־���.
			m_command->SetVertexBuffer(mesh->GetVertexBuffer());
			m_command->SetIndexBuffer(mesh->GetIndexBuffer());
			m_command->DrawIndexed(mesh->GetIndexBuffer()->GetCount());
		}
	}
}
