#include "DivePch.h"
#include "Renderer.h"
#include "Graphics_ConstantBuffers.h"
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
#include "D3D11/Dive_Texture.h"
#include "D3D11/PipelineState.h"
#include "D3D11/Command.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"
#include "D3D11/GBuffer.h"
#include "Scene/GameObject.h"
#include "Scene/SceneEvents.h"
#include "Scene/Components/Transform.h"
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
		if (!m_selectedCamera)	return;

		// Camera Color�� Skybox�� ���� ���?
		{
			m_command->ClearRenderTarget(
				m_graphics->GetRenderTargetView(),
				m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());
		}

		// Legacy Shading
		{
			pass_GBuffer();
			pass_Lighting();
		}

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
			
			// Mesh ������ ������� RenderingMode�� �����ص� �ȴ�.
			// ������ �Ʒ��� ������ ���� ������� �Ǿ��ִ�. �� ���� �ϴ� ����� ������ �Ѵ�.
			// GameObject�� ���������� �̷���� ���� ��쵵 �׳� ����� �� �����ѵ�...
			/*
			if (gameObject->HasComponent<RenderableMesh>())
			{
				// �̷��� �ȵȴ�. Matrial�� �ϳ��� ������ GameObject�� ���������� �����ؾ� �Ѵ�.
				for (const auto& material : gameObject->GetComponent<RenderableMesh>()->GetAllMaterial())
				{
					//if (material->GetShaderPass() == eShaderPassType::Legacy)
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
			*/
			// light component ���η� light ����
		}

		// ù ��° ī�޶�� ����Ʈ ����
		m_selectedCamera = m_gameObjects[eRenderableObjectType::Camera][0];
	}

	void Renderer::pass_GBuffer()
	{
		// �̷��� ������ ��� �ʹݿ� �����;��� ��ü�� ��������.
		// ���� ���ڸ� ContantBuffer, Smaplers. DepthStencilStates, RasterizerStates � �����;� �Ѵ�.
		// �ƿ� ��� ������ �δ� ����� ������ Graphics�� �ʹ� �������̴�.
		auto gbuffer = m_graphics->GetGBuffer();
		if (!gbuffer)	return;

		// Pipeline State
		{
			PipelineState state;
			// GBuffer PreRender
			// �ʱ�ȭ
			// render target���� ����

			// Shader�� �������� �����Ű�°� �°�����
			// �Ϲ� Mesh�� SkinnedMesh�� Shader�� �ٸ���.
			// �׷��ٸ� �� ������ �� �� �����ϰų� �̸� ���� �Լ��� �����ϴ°� ������?

			// CBuffer�� �ϴ� ���� �����ϰ�(world, worldViewProj) �����Ͽ� �� ������ ��������.
			// �׷��� ����ü ��ü�� �̸� ����Ǿ� �ְ� ���� ���Ѿ� �Ѵ�.
		}

		// Renderable Render
		for(auto gameObject : m_gameObjects[eRenderableObjectType::Opaque])
		{
			// ������ opaque�� material�� Shader�� ��ġ���η� �������� ����(continue) ��ų �� �ִ�.
			// ������ material�� �� �̻��̶�� ��� �ؾ� �ұ�? => �̰� GameObject�� �������� �����̴�.

			// constant buffer
			{
				auto cbObject = static_cast<CB_OBJECT*>(m_graphics->GetObjectContantBuffer()->Map());
				if(!cbObject)
				{
					CORE_ERROR("");
					return;
				}
				DirectX::XMStoreFloat4x4(&cbObject->world, gameObject->GetTransform()->GetMatrix());
				DirectX::XMStoreFloat4x4(&cbObject->worldViewProjection, m_selectedCamera->GetComponent<Camera>()->GetViewProjectionMatrix());
				m_graphics->GetObjectContantBuffer()->Unmap();
			}

			// ����� Material�� Texture�� Mesh�� Buffer
			// ���������� Draw
		}
	}
	
	// �Ű������� ������ RenderTarget�� �޸� �� �� ���� �� ����.
	void Renderer::pass_Lighting()
	{
		// BackBuffer RenderTargetView�� Set
		// GBuffer ���� Constant Buffer ����?

		// Light Draw
	}
}
