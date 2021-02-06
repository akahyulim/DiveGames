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
			
			// Renderable�� Rigid, Skinned �� ���� �з� ���� ���� �� �� �����ؾ� �Ѵ�. => �̰� ����
			// Material�� �� �̻��� ��� �ϳ��� GameObject�� �ߺ����� �з��� �� �ִ�. => Material�� Mesh�� �з�? �׷��� Material != Shader�̴�.
			// Shader�� �ٸ��ٸ� �� �� ������ �޸� �ؾ� �Ѵ�.
			// => �ᱹ Shader�� Rendering Mode �� �� �����ؼ� ������� �Ѵ�.
			// �ϴ� 1������ Rendering Mode�� �� �ϴ�.
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

	// �ϴ� RenderingMode�θ� ������ Shader�� �׳� Material���� �����Ͽ� ��� ���ϵ��� �غ���?
	// ���� �������ϸ��� ���� �м��� �غ����� �ϰ� ����...
	// �׷��� �̷��� ������ Command�� State�� ȿ�뼺�� ���� ���µ�...
	// �ϴ� Opaque�� ����
	void Renderer::pass_GBuffer()
	{
		// GBuffer PreRender
		// GBuffer RenderTargetView, DepthStencilView Clear
		// GBuffer RenderTargetView, DepthStencilState Set
		// => GBuffer Class�� ���� �� Graphics���� ������� �ϸ� �ȴ�.

		// Renderable Render
		// Shader, InputLayout, ConstantBuffer ���� => ConstantBuffer�� Transform�� Material�� ���еǾ�� �Ѵ�.
		// Renderable Draw(Buffer, PrimitiveTopology) => �̰� Mesh ������ ó���ϴ� �� ���� �� ������...

		// GBuffer PostRender
		// GBuffer RenderTargetView�� ������ ����ȭ�� ���ΰ�
	}
	
	void Renderer::pass_Lighting()
	{
		// BackBuffer RenderTargetView�� Set
		// GBuffer ���� Constant Buffer ����?

		// Light Draw
	}
}
