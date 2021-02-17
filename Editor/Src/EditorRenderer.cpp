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

		// ����׿� ���ҽ� ����?
		// Viewport�� RenderTargetView�� �����Ϸ��� �ϴ� ũ�⸦ �޾ƾ� �Ѵ�.
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

		// �� �κ��� �ϴ� �Ʒ��� �з� �Լ��� �������̴�.
		if (m_gameObjects[eRenderableObjectType::Light].empty())
		{
			// ��ī�� �ڽ��� ������ �� �ִ�. �̰� Scene�� Skybox�ʹ� �����̴�.

			m_command->ClearRenderTarget(m_EditorViewRenderTarget, m_selectedCamera->GetComponent<Camera>()->GetBackgroundColor());

			return;
		}

		// constant buffer - �� ������ ����
		{

		}

		// ���ϳ����� Shader���� ���� Command�� �������� �׸���.

		// frame count ++
	}

	void EditorRenderer::ResizeRenderTarget(unsigned int width, unsigned int height)
	{
		SAFE_DELETE(m_EditorViewRenderTarget);

		m_EditorViewRenderTarget = new Dive_Texture(m_context);
		m_EditorViewRenderTarget->CreateRenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	}
}