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
			
			// Renderable이 Rigid, Skinned 두 개라 분류 과정 역시 두 번 진행해야 한다. => 이건 가능
			// Material이 둘 이상일 경우 하나의 GameObject가 중복으로 분류될 수 있다. => Material별 Mesh로 분류? 그런데 Material != Shader이다.
			// Shader가 다르다면 매 번 설정을 달리 해야 한다.
			// => 결국 Shader와 Rendering Mode 둘 다 염두해서 나누어야 한다.
			// 일단 1순위는 Rendering Mode일 듯 하다.
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

	// 일단 RenderingMode로만 나누고 Shader는 그냥 Material에서 참조하여 계속 변하도록 해볼까?
	// 추후 프로파일링을 통해 분석을 해보도록 하고 말야...
	// 그런데 이러한 구현은 Command와 State의 효용성이 거의 없는데...
	// 일단 Opaque만 적용
	void Renderer::pass_GBuffer()
	{
		// GBuffer PreRender
		// GBuffer RenderTargetView, DepthStencilView Clear
		// GBuffer RenderTargetView, DepthStencilState Set
		// => GBuffer Class를 만든 후 Graphics에서 관리토록 하면 된다.

		// Renderable Render
		// Shader, InputLayout, ConstantBuffer 관리 => ConstantBuffer는 Transform과 Material이 구분되어야 한다.
		// Renderable Draw(Buffer, PrimitiveTopology) => 이건 Mesh 단위로 처리하는 게 나을 것 같은데...

		// GBuffer PostRender
		// GBuffer RenderTargetView의 내용을 파일화할 것인가
	}
	
	void Renderer::pass_Lighting()
	{
		// BackBuffer RenderTargetView를 Set
		// GBuffer 계산용 Constant Buffer 전달?

		// Light Draw
	}
}
