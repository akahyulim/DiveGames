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

		// 뭘로 채워나가나...

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

		// 이 부분은 일단 아래의 분류 함수에 종속적이다.
		if (m_gameObjects[eRenderableObjectType::Light].empty())
		{
			// 스카이 박스를 적용할 수 있다. 이건 Scene의 Skybox와는 별개이다.

			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - 매 프레임 갱신
		{

		}

		// 이하내용은 Shader별로 각종 Command의 조합으로 그린다.

		// frame count ++
	}

	// Scene으로부터 그려야 할 대상들을 가져온 후 분류한다.
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
			
			// 이렇게하면 일단 Renderable에 따라 두 번 나눠야 하고
			// 하나의 GameObject가 두 곳 이상에 저장될 수 있다...
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

			// light component 여부로 light 구분
		}

		// 첫 번째 카메라로 디폴트 설정
		m_selectedCamera = m_gameObjects[eRenderableObjectType::Camera][0];
	}

	// 실제로는 technique이다. 내부에서 다시 pass로 갈라져야 한다.
	void Renderer::pass_Color()
	{
		if (m_gameObjects[eRenderableObjectType::Opaque].empty())
			return;

		// PipelineState
		{
			PipelineState state;
			// MeshRenderer, Material을 참조
			// 그 외의 설정(DepthStencil, Blending, Rasterizer 등)은 이전에 등록해야 한다.
			// shader를 전달할 수 없다. shader 별로 나누던지, 아래의 루프에서 material로부터 shader를 참조해야 한다.
			// 아니면 이 곳에서 shader를 등록한 후, 아래의 루프에서 동일한 Shader를 사용하지 않는 경우 continue로 넘겨야 한다.

			m_command->SetPipelineState(state);
		}

		for(const auto& gameObject : m_gameObjects[eRenderableObjectType::Opaque])
		{
			// 기본적으로 Renderable에 저장된 객체들을 사용한다.
			// 그렇다면 command에 Renderable을 넘기는게 낫지 않을까?
			auto renderable = gameObject->GetComponent<RenderableMesh>();
			auto mesh = renderable->GetMesh();

			// material shader resource view
			// material constant buffer
			// world position constant buffer? => 예전에는 Transform에 viewProj를 넘겨 직접 계산한 결과를 buffer에 넣었다.
			m_command->SetVertexBuffer(mesh->GetVertexBuffer());
			m_command->SetIndexBuffer(mesh->GetIndexBuffer());
			m_command->DrawIndexed(mesh->GetIndexBuffer()->GetCount());
		}
	}
}
