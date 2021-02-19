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

		// 뭘로 채워나가나...

		m_command = std::make_unique<Command>(m_context);

		m_bInitialized = true;
	}


	void Renderer::Render()
	{
		if (!m_selectedCamera)	return;

		// Camera Color은 Skybox가 없는 경우?
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
			
			// Mesh 종류에 상관없이 RenderingMode로 구분해도 된다.
			// 하지만 아래의 구현은 각각 구분토록 되어있다. 한 번에 하는 방법을 만들어야 한다.
			// GameObject가 계층구조로 이루어져 있을 경우도 그냥 적용될 거 같긴한데...
			/*
			if (gameObject->HasComponent<RenderableMesh>())
			{
				// 이러면 안된다. Matrial은 하나씩 가지고 GameObject의 계층구조로 관리해야 한다.
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
			// light component 여부로 light 구분
		}

		// 첫 번째 카메라로 디폴트 설정
		m_selectedCamera = m_gameObjects[eRenderableObjectType::Camera][0];
	}

	void Renderer::pass_GBuffer()
	{
		// 이렇게 구현할 경우 초반에 가져와야할 객체가 많아진다.
		// 예를 들자면 ContantBuffer, Smaplers. DepthStencilStates, RasterizerStates 등도 가져와야 한다.
		// 아에 멤버 변수로 두는 방법도 있지만 Graphics에 너무 의존적이다.
		auto gbuffer = m_graphics->GetGBuffer();
		if (!gbuffer)	return;

		// Pipeline State
		{
			PipelineState state;
			// GBuffer PreRender
			// 초기화
			// render target으로 설정

			// Shader를 공통으로 적용시키는게 맞겠지만
			// 일반 Mesh와 SkinnedMesh는 Shader가 다르다.
			// 그렇다면 이 과정을 두 번 구분하거나 이름 없는 함수로 구현하는게 맞을까?

			// CBuffer는 일단 나도 간단하게(world, worldViewProj) 구성하여 이 곳에서 전달하자.
			// 그런데 구조체 자체는 미리 선언되어 있고 생성 시켜야 한다.
		}

		// Renderable Render
		for(auto gameObject : m_gameObjects[eRenderableObjectType::Opaque])
		{
			// 동일한 opaque라도 material의 Shader로 일치여부로 루프에서 제외(continue) 시킬 수 있다.
			// 하지만 material이 둘 이상이라면 어떻게 해야 할까? => 이건 GameObject의 계층구조 문제이다.

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

			// 대상은 Material의 Texture와 Mesh의 Buffer
			// 마지막에는 Draw
		}
	}
	
	// 매개변수의 유무로 RenderTarget을 달리 할 수 있을 것 같다.
	void Renderer::pass_Lighting()
	{
		// BackBuffer RenderTargetView를 Set
		// GBuffer 계산용 Constant Buffer 전달?

		// Light Draw
	}
}
