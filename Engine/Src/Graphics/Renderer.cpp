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
				// Material에 저장된 Rendering Mode로 구분
				// 기본적으로 opaque, transparent 추후 cutout, fade 추가?
			}

			// light component 여부로 light 구분
		}
	}

	// 일단 책의 내용을 따라 가자.
	// 그림자와 후처리까지 진행한 후 펄어비스 구현을 적용해보자.
	void Renderer::legacyShader()
	{
		// 잊어버리기 전에...
		// 펄어비스는 DepthBuffer를 ShaderResourceView로 만들지 않은 것 같다.
		// => 다른 RenderTargetView들의 설명을 제외한 것일 수 있다.

		// Scene Render
		{

		}

		// Light Render
		{

		}
	}

	// RenderTargetView = ImGUI + renderEditor(view window)
	// Editor 전용으로 gizmo가 추가 될 수 있다. 일반적으론 DebugRender라 칭하는 것 같다. 예) grid
	// 스파르탄은 Debug Render 함수들이 따로 구현되어 이다.
	void Renderer::renderEditor()
	{
		// 카메라가 존재하지 않으면 RenderTarget만 Clear
		if (!m_selectedCamera)
		{
			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f));

			return;
		}

		// Renderable이 존재하지 않으면 Camera Color로 RenderTarget Clear
		if (m_gameObjects[eRenderableObjectType::Light].empty() && m_gameObjects[eRenderableObjectType::Opaque].empty()
			&& m_gameObjects[eRenderableObjectType::Transparent].empty())
		{
			// 이 부분은 추후 수정이 필요하다. 스카이 박스가 설정될 수 있기 때문이다.

			m_command->ClearRenderTarget(m_graphics->GetRenderTexture(eRenderTextureType::EditorView),
				m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - frame?
		{

		}

		// 이하내용은 Shader별로 각종 Command의 조합으로 그린다.

		base();
	}

	void Renderer::base()
	{
		// input layout에 일치하는 오브젝트가 없다면 리턴해야 한다.
		// 이건 결국 Shader에 저장된 InputLayout과 Object의 묶음으로 비교해야 한다.
		// 덧붙여서 Transparent까지 생각한다면 VertexTypeCount * 2개의 조합이 나와버린다.
		// 물론 이때 State는 Shader를 제외한다면 전부 동일할 수 있다.

		// state 설정
		{
			// graphics로부터 resource들을 가져와 state를 구성한 후
			// command에 전달하여 gpu로 전달한다.
			PipelineState state;
			state.depthStencilState		= m_graphics->GetDepthStencilState(true);
			state.primitiveTopology		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			state.depthStencilTexture	= m_graphics->GetRenderTexture(eRenderTextureType::DepthStencil);
			state.renderTargets.push_back(m_graphics->GetRenderTexture(eRenderTextureType::EditorView));

			m_command->SetPipelineState(state);

			// 현재 pass에선 이미 Clear된 EditorView 하나만 사용하지만,
			// deferred 처럼 다수의 render target이 사용되는 경우 최초 한 번 clear해야 한다.
		}

		// data 설정
		// state의 shader와 input layout에 따라 object type을 구분해야 한다.
		{
			// type별로 구분된 buffer들을 command를 통해 gpu로 전달한다.
		}
	}

	void Renderer::testTemplate()
	{
		// pass에서 그려져야 할 object를 우선 확인한다. 없다면 리턴
		// 예를 들면 opaque와 transparent 둘 다 비어있다면 리턴
		// 바로 리턴하면 안되는 것일 수 있다. 예를 들면 RenderTargetView 같은건 아래에서 Clear해주어야 한다.

		// pass 의도에 맞게 기본적인 state를 설정
		
		// 남은 state와 data를 구성하는 람다 함수 생성
		// 매개변수로 material과 renderable을 전달하여 shader와 vertex type을 매칭?
		// 이 경우 material의 매개변수가 동일해야 한다.

		// 대상 object vector를 위의 함수로 draw
		// 문제는 VertexType별 구분없이 RenderableType으로만 접근한다면 오버헤드가 발생할 수 있다.
	}
}
