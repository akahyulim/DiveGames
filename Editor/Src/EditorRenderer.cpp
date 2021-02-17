#include "Dive.h"
#include "EditorRenderer.h"

namespace Dive
{
	EditorRenderer::EditorRenderer(Context * context)
		: Renderer(context)
	{
	}

	EditorRenderer::~EditorRenderer()
	{
	}
	
	void EditorRenderer::Initialize()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
			return;
		m_graphics = graphics.get();
		m_deviceContext = m_graphics->GetRHIContext();

		// 디버그용 리소스 생성?
		// Viewport용 RenderTargetView를 생성하려면 일단 크기를 받아야 한다.
		m_EditorViewRenderTarget = new Dive_Texture(m_context);
		m_EditorViewRenderTarget->CreateRenderTexture(800, 600, DXGI_FORMAT_R8G8B8A8_UNORM);

		m_command = std::make_unique<Command>(m_context);

		m_bInitialized = true;
	}

	void EditorRenderer::Render()
	{
		if (!m_selectedCamera)
		{
			m_command->ClearRenderTarget(m_EditorViewRenderTarget, DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f));

			return;
		}

		// 이 부분은 일단 아래의 분류 함수에 종속적이다.
		if (m_gameObjects[eRenderableObjectType::Light].empty())
		{
			// 스카이 박스를 적용할 수 있다. 이건 Scene의 Skybox와는 별개이다.

			m_command->ClearRenderTarget(m_EditorViewRenderTarget, m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - 매 프레임 갱신
		{

		}

		// 이하내용은 Shader별로 각종 Command의 조합으로 그린다.

		// frame count ++
	}

	void EditorRenderer::ResizeRenderTarget(unsigned int width, unsigned int height)
	{
		SAFE_DELETE(m_EditorViewRenderTarget);

		m_EditorViewRenderTarget = new Dive_Texture(m_context);
		m_EditorViewRenderTarget->CreateRenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	}
}